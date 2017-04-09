#include "../inc/sai_adapter.h"
// extern "C" {
//   int tun_alloc(char*);
// }
void callback(u_char *out_port,const struct pcap_pkthdr* pkthdr,const u_char* packet)
{
  // pcap_t* out_pcap = (pcap_t*) out;
  int hw_port = *((int*) out_port)
  if (pcap_inject(adapter_pcap, packet, pkthdr->len) == -1) {
    printf("error on injecting packet [%s]\n", pcap_geterr(adapter_pcap));
  }
}

int pktinit(char *in_dev, char *out_dev, int* hw_port) 
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *in_pcap, *out_pcap;
    char dev_buff[64] = {0};
    int i =0;
    // pcap_lookupnet(dev, &pNet, &pMask, errbuf);
    in_pcap = pcap_open_live(in_dev, BUFSIZ, 0,-1, errbuf);
    if(in_pcap == NULL)
    {
        printf("pcap_open_live() failed due to [%s] on dev %s\n", errbuf, in_dev);
        return -1;
    }
    // out_pcap = pcap_open_live(out_dev, BUFSIZ, 0,-1, errbuf);
    // if(out_pcap == NULL)
    // {
        // printf("pcap_open_live() failed due to [%s] on dev %s\n", errbuf, out_dev);
        // return -1;
    // }
    // printf("started sinffing @ dev %s. mirror to dev %s\n", in_dev, out_dev);
    if (pcap_loop(in_pcap, 0, callback, (u_char*) hw_port) == -1) {
        printf("pcap_loop() failed: %s\n", pcap_geterr(in_pcap));
    }
    return 0;
}

sai_status_t sai_adapter::create_hostif(sai_object_id_t *hif_id,
                                        sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list) {
  (*logger)->info("create_hostif");
  HostIF_obj *hostif = new HostIF_obj(sai_id_map_ptr);
  switch_metadata_ptr->hostifs[hostif->sai_object_id] = hostif;
  char *netdev_name;
  char cpu_port_name[] = "host_port";
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_HOSTIF_ATTR_TYPE:
      hostif->hostif_type = (sai_hostif_type_t)attribute.value.s32;
      break;
    case SAI_HOSTIF_ATTR_OBJ_ID:
      hostif->port = switch_metadata_ptr->ports[attribute.value.oid];
      break;
    case SAI_HOSTIF_ATTR_NAME:
      hostif->netdev_name = string(attribute.value.chardata);
      netdev_name = attribute.value.chardata;
      break;
    }
  }
  if (hostif->hostif_type == SAI_HOSTIF_TYPE_NETDEV) {
    if (hostif->netdev_name == "") {
      (*logger)->error("trying to create netdev wihout any name");
    }
    (*logger)->info("creating netdev {}", hostif->netdev_name);
    hostif->netdev_fd = tun_alloc(netdev_name, 1);
    hostif->netdev_thread = std::thread(pktinit, netdev_name, &hostif->port->hw_port);
  }
  *hif_id = hostif->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_hostif(sai_object_id_t hif_id) {
  (*logger)->info("remove_hostif");
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::create_hostif_table_entry(
    sai_object_id_t *hif_table_entry, sai_object_id_t switch_id,
    uint32_t attr_count, const sai_attribute_t *attr_list) {
  (*logger)->info("create_hostif_table_entry");
  HostIF_Table_Entry_obj *hostif_table_entry =
      new HostIF_Table_Entry_obj(sai_id_map_ptr);
  switch_metadata_ptr->hostif_table_entries[hostif_table_entry->sai_object_id] =
      hostif_table_entry;

  // parse attribute
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_HOSTIF_TABLE_ENTRY_ATTR_TYPE:
      hostif_table_entry->entry_type = (sai_hostif_table_entry_type_t) attribute.value.s32;
      break;
    case SAI_HOSTIF_TABLE_ENTRY_ATTR_TRAP_ID:
    	(*logger)->info("trap oid {}", attribute.value.oid);
      	hostif_table_entry->trap_id = switch_metadata_ptr->hostif_traps[attribute.value.oid]->trap_id;
      	(*logger)->info("trap id {}", hostif_table_entry->trap_id);
      break;
    case SAI_HOSTIF_TABLE_ENTRY_ATTR_CHANNEL_TYPE:
      hostif_table_entry->channel_type = (sai_hostif_table_entry_channel_type_t) attribute.value.s32;
      break;
    }
  }
  (*logger)->info("after parsing attr");
  adapter_packet_handler_fn handler_fn;
  switch (hostif_table_entry->channel_type) {
  	case SAI_HOSTIF_TABLE_ENTRY_CHANNEL_TYPE_NETDEV_PHYSICAL_PORT:
  		handler_fn = netdev_phys_port_fn;
  		(*logger)->info("netdev_phys_port_fn");
  		break;
  	default:
  		(*logger)->error("channel type not supported");
  		return SAI_STATUS_NOT_SUPPORTED;
  		break;
  }

  (*logger)->info("after parsing channel type");
  switch (hostif_table_entry->entry_type) {
  	case SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID:
  		(*logger)->info("add host if trap id. trap_id {}", hostif_table_entry->trap_id);
    	add_hostif_trap_id_table_entry(hostif_table_entry->trap_id, handler_fn);
    	(*logger)->info("after add hostif table entry");
    	break;
    default:
  		(*logger)->error("hostif table entry type not supported");
  		return SAI_STATUS_NOT_SUPPORTED;
  		break;
  }

  *hif_table_entry = hostif_table_entry->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t
sai_adapter::remove_hostif_table_entry(sai_object_id_t hif_table_entry) {
  (*logger)->info("remove_hostif_table_entry");
}

sai_status_t sai_adapter::create_hostif_trap_group(
    sai_object_id_t *hostif_trap_group_id, sai_object_id_t switch_id,
    uint32_t attr_count, const sai_attribute_t *attr_list) {
  (*logger)->info("create_hostif_trap_group");
  HostIF_Trap_Group_obj *hostif_trap_group =
      new HostIF_Trap_Group_obj(sai_id_map_ptr);
  switch_metadata_ptr->hostif_trap_groups[hostif_trap_group->sai_object_id] =
      hostif_trap_group;

  *hostif_trap_group_id = hostif_trap_group->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t
sai_adapter::remove_hostif_trap_group(sai_object_id_t hostif_trap_group_id) {
  (*logger)->info("remove_hostif_trap_group");
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::create_hostif_trap(sai_object_id_t *hostif_trap_id,
                                             sai_object_id_t switch_id,
                                             uint32_t attr_count,
                                             const sai_attribute_t *attr_list) {
  (*logger)->info("create_hostif_trap");
  HostIF_Trap_obj *hostif_trap = new HostIF_Trap_obj(sai_id_map_ptr);
  hostif_trap->trap_id = switch_metadata_ptr->GetNewTrapID();
  switch_metadata_ptr->hostif_traps[hostif_trap->sai_object_id] = hostif_trap;

  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_HOSTIF_TRAP_ATTR_TRAP_TYPE:
      hostif_trap->trap_type = (sai_hostif_trap_type_t)attribute.value.s32;
      break;
    case SAI_HOSTIF_TRAP_ATTR_PACKET_ACTION:
      hostif_trap->trap_action = (sai_packet_action_t)attribute.value.s32;
      break;
    default:
      (*logger)->warn(
          "in parsing hostif_trap, the following attr was dumped: {}",
          attribute.id);
    }
  }
  *hostif_trap_id = hostif_trap->sai_object_id;

  if (hostif_trap->trap_type == SAI_HOSTIF_TRAP_TYPE_LACP &&
      hostif_trap->trap_action == SAI_PACKET_ACTION_TRAP) {
    BmAddEntryOptions options;
    BmMatchParams match_params;
    BmActionData action_data;
    action_data.clear();
    match_params.clear();
    match_params.push_back(
        parse_exact_match_param(1652522221570, 6)); // dmac : 01-80-c2-00-00-02
    action_data.push_back(
        parse_param(hostif_trap->trap_id, 2));
    hostif_trap->handle_l2_trap = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_l2_trap", match_params, "action_set_trap_id",
        action_data, options);
    (*logger)->info("added l2 trap - lacp , trap_id: {}.",
                    hostif_trap->trap_id, hostif_trap->sai_object_id);

    action_data.clear();
    match_params.clear();
    match_params.push_back(
        parse_exact_match_param(hostif_trap->trap_id, 2)); 
    hostif_trap->handle_trap_id = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_trap_id", match_params, "action_trap_to_cpu",
        action_data, options);
    (*logger)->info("added LACP trap to cpu, trap_id: {}. sai_object_id: {}",
                   hostif_trap->trap_id, hostif_trap->sai_object_id);
  } else {
    (*logger)->warn(
        "unsupported trap requested, trap type is: {}, trap_action is: {} \n ",
        hostif_trap->trap_type, hostif_trap->trap_action);
  }
}

sai_status_t sai_adapter::remove_hostif_trap(sai_object_id_t hostif_trap_id) {
  (*logger)->info("remove_hostif_trap trap_id: {}", hostif_trap_id);
  HostIF_Trap_obj *hostif_trap =
      switch_metadata_ptr->hostif_traps[hostif_trap_id];
  if (hostif_trap->trap_type == SAI_HOSTIF_TRAP_TYPE_LACP) {
    try {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_l2_trap",
                                        hostif_trap->handle_l2_trap);
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_trap_id",
                                        hostif_trap->handle_trap_id);
    } catch (...) {
      (*logger)->warn("--> unable to remove hostif_trap tables entries");
    }
  } else {
    (*logger)->warn("unsupported remove trap requested, trap type is: {}",
                    hostif_trap->trap_type);
  }
  switch_metadata_ptr->hostif_traps.erase(hostif_trap->sai_object_id);
  sai_id_map_ptr->free_id(hostif_trap->sai_object_id);
  return SAI_STATUS_SUCCESS;
}

void sai_adapter::add_hostif_trap_id_table_entry(
    uint16_t trap_id, adapter_packet_handler_fn handler_fn) {
  hostif_trap_id_table[trap_id] = handler_fn;
}

void sai_adapter::lookup_hostif_trap_id_table(u_char* packet,
                                              cpu_hdr_t *cpu, int pkt_len) {
  hostif_trap_id_table_t::iterator it = hostif_trap_id_table.find(cpu->trap_id);
  if (it != hostif_trap_id_table.end()) {
    it->second(packet, cpu, pkt_len);
    return;
  } else {
    printf("hostif_table lookup failed\n"); // TODO logger / return value
  }
}

void sai_adapter::netdev_phys_port_fn(u_char* packet, cpu_hdr_t *cpu, int pkt_len) {
	(*logger)->info("trap arrived to physical netdev cahnnel @ ingress_port {}. len = {}", cpu->ingress_port, pkt_len);
	HostIF_obj *hostif = switch_metadata_ptr->GetHostIFFromPhysicalPort(cpu->ingress_port);
	write(hostif->netdev_fd, packet, pkt_len);
	return;
}