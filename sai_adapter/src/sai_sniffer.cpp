#include "../inc/sai_adapter.h"
#include <sched.h>

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
  (*logger)->info("release pcap lock");
  std::unique_lock<std::mutex> lk(m);
  pcap_loop_started = true;
  lk.unlock();
  cv.notify_one();
}

void sai_adapter::PacketSniffer() {
  const char *dev = "host_port";

  char errbuf[PCAP_ERRBUF_SIZE];

  (*logger)->info("pcap started on dev {}", dev);
  adapter_pcap = pcap_open_live(dev, BUFSIZ, 0, -1, errbuf);
  if (adapter_pcap == NULL) {
    (*logger)->error("pcap_open_live() failed: {}", errbuf);
    release_pcap_lock();
    return;
  }

  release_pcap_lock();

  if (pcap_loop(adapter_pcap, 0, packetHandler, (u_char *)this) == -1) {
    (*logger)->error("pcap_loop() failed: {}", pcap_geterr(adapter_pcap));
  }
  (*logger)->info("pcap loop ended");
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

void sai_adapter::packetHandler(u_char *userData,
                                const struct pcap_pkthdr *pkthdr,
                                const u_char *packet) {

  sai_adapter *adapter = (sai_adapter *)userData;
  cpu_hdr_t *cpu = (cpu_hdr_t *)packet;
  // ReverseBytes((uint8_t *)cpu, CPU_HDR_LEN);
  cpu->trap_id = ntohs(cpu->trap_id);
  (*logger)->info("CPU packet captured. trap_id = {}. ingress_port = {}", cpu->trap_id, cpu->ingress_port);
  u_char* decap_packet = (u_char*) (packet + CPU_HDR_LEN);
  HostIF_Table_Entry_obj *hostif_table_entry =
      switch_metadata_ptr->GetTableEntryFromTrapID(cpu->trap_id);
  if (hostif_table_entry == nullptr) {
    (*logger)->error("CPU packet recieved with unknown trap_id");
    return;
  }
  switch (hostif_table_entry->entry_type) {
  case SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID:
    adapter->lookup_hostif_trap_id_table(decap_packet, cpu, pkthdr->len - CPU_HDR_LEN);
    break;
  }
  // if (cpu_hdr->trap_id == 512) {
  //   adapter->learn_mac(ether, cpu_hdr);
  // }
}

void sai_adapter::learn_mac(u_char* packet, cpu_hdr_t *cpu, int pkt_len) {
  uint32_t ingress_port = cpu->ingress_port;
  (*logger)->info("learn_mac from port {}", ingress_port);
  ethernet_hdr_t *ether = (ethernet_hdr_t *) packet;
  ether->ether_type = ntohs(ether->ether_type);
  ReverseBytes(ether->dst_addr, 6);
  ReverseBytes(ether->src_addr, 6);
  uint8_t *src_mac = ether->src_addr;
  print_mac_to_log(src_mac, *logger);
  BridgePort_obj *bridge_port;
  Bridge_obj *bridge;
  sai_object_id_t port_id;
  for (port_id_map_t::iterator it = switch_metadata_ptr->ports.begin();
       it != switch_metadata_ptr->ports.end(); ++it) {
    if (it->second->hw_port == ingress_port) {
      port_id = it->first;
      (*logger)->info("learn_mac from port_id {}", port_id);
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
        (*logger)->info("MAC learning from ingress lag {}", it->first);
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
  }
  adapter_create_fdb_entry(bridge_port->sai_object_id, src_mac, bridge_type,
                           bridge_port->vlan_id, bridge->sai_object_id);
}