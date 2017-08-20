#include <sched.h>
#include "../inc/sai_adapter.h"

void ReverseBytes(uint8_t *byte_arr, int size) {
  uint8_t tmp;
  for (int lo = 0, hi = size - 1; hi > lo; lo++, hi--) {
    tmp = byte_arr[lo];
    byte_arr[lo] = byte_arr[hi];
    byte_arr[hi] = tmp;
  }
}

void print_mac_to_log(const uint8_t *mac,
                      std::shared_ptr<spdlog::logger> logger) {
  logger->info("{0:02X}:{1:02X}:{2:02X}:{3:02X}:{4:02X}:{5:02X}", mac[5],
               mac[4], mac[3], mac[2], mac[1], mac[0]);
}

void sai_adapter::release_pcap_lock() {
  std::unique_lock<std::mutex> lk(m);
  pcap_loop_started = true;
  lk.unlock();
  cv.notify_one();
  (*logger)->info("pcap lock released");
}

void sai_adapter::PacketSniffer() {
  char errstr[1024];
  int maxfd = 0;
  fd_set sniff_set;
  int i;
  int num_of_devs = 2; //TODO: vector?
  cpu_port[0].dev = "switch_port";
  cpu_port[1].dev = "router_port";

  for (i = 0; i < num_of_devs; i++) {
    cpu_port[i].pcap = pcap_open_live(cpu_port[i].dev, BUFSIZ, 0, -1, errstr);
    if (cpu_port[i].pcap == NULL) {
      (*logger)->error("pcap_open_live() failed: {}", errstr);
      release_pcap_lock();
      return;
    }
    if (pcap_setnonblock(cpu_port[i].pcap, 1, errstr) == -1) {
      (*logger)->error("pcap_setnonblock() failed: {}", errstr);
      release_pcap_lock();
      return;
    }
    cpu_port[i].fd = pcap_get_selectable_fd(cpu_port[i].pcap);
  }

  int break_sniff_loop = 0;
  // if (pipe2(sniff_pipe_fd, O_NONBLOCK) != 0) {
  if (pipe(sniff_pipe_fd) != 0) {
    (*logger)->error("error creating pipe");
    return;
  }
  struct pcap_pkthdr *pcap_header;
  const u_char *pcap_packet;
  (*logger)->info("start sniffing on {}, {}", cpu_port[0].dev, cpu_port[1].dev);
  release_pcap_lock();
  while (break_sniff_loop == 0) {
    FD_ZERO(&sniff_set);
    FD_SET(sniff_pipe_fd[0], &sniff_set);
    maxfd = sniff_pipe_fd[0];
    for (i = 0; i < num_of_devs; i++) {
      FD_SET(cpu_port[i].fd, &sniff_set);
      maxfd = std::max(maxfd, cpu_port[i].fd);
    }
    for (std::vector<netdev_fd_t>::iterator it = active_netdevs.begin(); it!=active_netdevs.end(); it++) {
      FD_SET(it->fd, &sniff_set);
      maxfd = std::max(maxfd, it->fd);
    }
    if (select(maxfd + 1, &sniff_set, NULL, NULL, NULL) < 0) {
      (*logger)->error("select error");
      exit(1);
    }
    for (i = 0; i < num_of_devs; i++) {
      if (FD_ISSET(cpu_port[i].fd, &sniff_set)) {
        switch (pcap_next_ex(cpu_port[i].pcap, &pcap_header, &pcap_packet)) {
          case -1:
            /* Error */
            (*logger)->error("pcap_next_ex failed: {}", pcap_geterr(cpu_port[i].pcap));
            exit(2);
            break;

          case 0:
            /* No more packets to read for now */
            (*logger)->info("No packets recieved");
            break;
          case -2:
            /* Somebody called pcap_breakloop() */
            (*logger)->info("recieved breakloop");
            break_sniff_loop = 1;
            break;

          default:
            cpu_port_packetHandler((u_char*) this, pcap_header, pcap_packet);
            break;
        }
      }
    }
    for (std::vector<netdev_fd_t>::iterator it = active_netdevs.begin(); it!=active_netdevs.end(); it++) {
      if (FD_ISSET(it->fd, &sniff_set)) {
        u_char buffer[1000];
        int length = read(it->fd, &buffer, 1000);
        (*logger)->info("packet recieved of length {}", length);
        switch (it->type) {
          case SAI_OBJECT_TYPE_VLAN:
            vlan_netdev_packet_handler((it->data).vid, length, buffer);
            break;
          case SAI_OBJECT_TYPE_PORT:
            phys_netdev_packet_handler((it->data).hw_port, length, buffer);
            break;
          // case SAI_OBJECT_TYPE_LAG:
            // vlan_netdev_packet_handler(it->vid, length, buffer);
            // break;
        }
      }
    }
    if (FD_ISSET(sniff_pipe_fd[0], &sniff_set)) {
      // TODO: read?
      char ch;
      read(sniff_pipe_fd[0], &ch, 1);
      if (ch == 'c') {
        (*logger)->info("recieved breakloop");
        break_sniff_loop = 1;
      }
    }
  }

  for (i=0; i<num_of_devs; i++) {
    (*logger)->info("closing pcap on dev {}", cpu_port[i].dev);
    pcap_close(cpu_port[i].pcap);
  }
  close(sniff_pipe_fd[0]);
  close(sniff_pipe_fd[1]);
  return;
}

void sai_adapter::adapter_create_fdb_entry(
    sai_object_id_t bridge_port_id, sai_mac_t mac,
    sai_fdb_entry_bridge_type_t bridge_type, sai_vlan_id_t vlan_id,
    sai_object_id_t bridge_id) {
  sai_attribute_t attr[3];
  attr[0].id = SAI_FDB_ENTRY_ATTR_TYPE;
  attr[0].value.s32 = SAI_FDB_ENTRY_TYPE_STATIC;

  attr[1].id = SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID;
  attr[1].value.oid = bridge_port_id;

  attr[2].id = SAI_FDB_ENTRY_ATTR_PACKET_ACTION;
  attr[2].value.s32 = SAI_PACKET_ACTION_FORWARD;

  sai_fdb_entry_t sai_fdb_entry;
  sai_fdb_entry.switch_id = 0;
  for (int i = 0; i < ETHER_ADDR_LEN; i++) {
    sai_fdb_entry.mac_address[i] = mac[i];
  }
  sai_fdb_entry.bridge_type = bridge_type;
  if (bridge_type == SAI_FDB_ENTRY_BRIDGE_TYPE_1Q) {
    sai_fdb_entry.vlan_id = vlan_id;
  }
  sai_fdb_entry.bridge_id = bridge_id;
  create_fdb_entry(&sai_fdb_entry, 3, attr);
}

void sai_adapter::cpu_port_packetHandler(u_char *userData,
                                const struct pcap_pkthdr *pkthdr,
                                const u_char *packet) {
  sai_adapter *adapter = (sai_adapter *)userData;
  cpu_hdr_t *cpu = (cpu_hdr_t*) packet;
  cpu->trap_id = ntohs(cpu->trap_id);
  cpu->dst = ntohs(cpu->dst);
  std::string type_str = (cpu->type == PORT) ? "port" : (cpu->type == LAG) ? "lag" : "vlan";
  (*logger)->info("CPU packet captured from netdev: type {}, trap_id = {}, src = {}",
                  type_str, cpu->trap_id, cpu->dst);
  u_char *decap_packet = (u_char *)(packet + CPU_HDR_LEN);
  HostIF_Table_Entry_obj *hostif_table_entry =
      switch_metadata_ptr->GetTableEntryFromTrapID(cpu->trap_id);
  if (hostif_table_entry == nullptr) {
    (*logger)->error("CPU packet recieved with unknown trap_id");
    return;
  }
  switch (hostif_table_entry->entry_type) {
    case SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID:
      adapter->lookup_hostif_trap_id_table(decap_packet, cpu,
                                           pkthdr->len - CPU_HDR_LEN);
      break;
  }
  // if (cpu_hdr->trap_id == 512) {
  //   adapter->learn_mac(ether, cpu_hdr);
  // }
}

void sai_adapter::learn_mac(u_char *packet, cpu_hdr_t *cpu, int pkt_len) {
  uint32_t ingress_port =  cpu->dst;
  (*logger)->info("learn_mac from port {}", ingress_port);
  ethernet_hdr_t *ether = (ethernet_hdr_t *)packet;
  ether->ether_type = ntohs(ether->ether_type);
  ReverseBytes(ether->dst_addr, 6);
  ReverseBytes(ether->src_addr, 6);
  uint8_t *src_mac = ether->src_addr;
  print_mac_to_log(src_mac, *logger);
  BridgePort_obj *bridge_port;
  Bridge_obj *bridge;
  sai_object_id_t port_id;
  uint16_t vlan_id;
  for (port_id_map_t::iterator it = switch_metadata_ptr->ports.begin();
       it != switch_metadata_ptr->ports.end(); ++it) {
    if (it->second->hw_port == ingress_port) {
      port_id = it->first;
      vlan_id = it->second->pvid;
      (*logger)->info("MAC learning from ingress port {} (sai_object_id)",
                      port_id);
      break;
    }
  }
  for (lag_id_map_t::iterator it = switch_metadata_ptr->lags.begin();
       it != switch_metadata_ptr->lags.end(); ++it) {
    for (std::vector<sai_object_id_t>::iterator mem_it =
             it->second->lag_members.begin();
         mem_it != it->second->lag_members.end(); ++mem_it) {
      if (switch_metadata_ptr->lag_members[*mem_it]->port->hw_port ==
          ingress_port) {
        (*logger)->info("MAC learning from ingress lag {} (sai_object_id)",
                        it->first);
        port_id = it->first;
        break;
      }
    }
  }
  for (bridge_port_id_map_t::iterator it =
           switch_metadata_ptr->bridge_ports.begin();
       it != switch_metadata_ptr->bridge_ports.end(); ++it) {
    if (it->second->port_id == port_id) {
      bridge_port = it->second;
      bridge = switch_metadata_ptr->bridges[it->second->bridge_id];
      (*logger)->info("bridge_port_id {}", bridge_port->sai_object_id);
      break;
    }
  }
  (*logger)->info("MAC learned (bridge sai_object_id {}):",
                  bridge->sai_object_id);
  print_mac_to_log(src_mac, *logger);
  sai_fdb_entry_bridge_type_t bridge_type;
  if (bridge->bridge_type == SAI_BRIDGE_TYPE_1Q) {
    bridge_type = SAI_FDB_ENTRY_BRIDGE_TYPE_1Q;
  } else {
    bridge_type = SAI_FDB_ENTRY_BRIDGE_TYPE_1D;
    vlan_id = bridge_port->vlan_id;
  }
  // if (ether->ether_type == 0x8100) {
  // TODO: get vlan from packet
  // }
  adapter_create_fdb_entry(bridge_port->sai_object_id, src_mac, bridge_type,
                           vlan_id, bridge->sai_object_id);
}