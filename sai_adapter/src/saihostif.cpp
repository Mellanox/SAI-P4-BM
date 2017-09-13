#include "../inc/sai_adapter.h"
// extern "C" {
//   int tun_alloc(char*);
// }

sai_status_t sai_adapter::create_hostif(sai_object_id_t *hif_id,
                                        sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list) {
  // TODO - move to 1 thread listening to all hostif (like in cpu ports)?
  (*logger)->info("create_hostif");
  HostIF_obj *hostif = new HostIF_obj(sai_id_map_ptr);
  switch_metadata_ptr->hostifs[hostif->sai_object_id] = hostif;
  char *netdev_name;
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_HOSTIF_ATTR_TYPE:
      hostif->hostif_type = (sai_hostif_type_t)attribute.value.s32;
      break;
    case SAI_HOSTIF_ATTR_OBJ_ID:
      hostif->netdev_obj_type = _sai_object_type_query(attribute.value.oid);
      switch (hostif->netdev_obj_type) {
        case SAI_OBJECT_TYPE_PORT:
          (*logger)->info("port object");
          hostif->netdev_obj.port = switch_metadata_ptr->ports[attribute.value.oid];
          break;
        case SAI_OBJECT_TYPE_LAG:
          (*logger)->info("lag object");
          hostif->netdev_obj.lag = switch_metadata_ptr->lags[attribute.value.oid];
          break;
        case SAI_OBJECT_TYPE_VLAN:
          (*logger)->info("vlan object");
          hostif->netdev_obj.vlan = switch_metadata_ptr->vlans[attribute.value.oid];
          break;
        default:
          (*logger)->error("object id {} given to create hostif is invalid", attribute.value.oid);
          switch_metadata_ptr->hostifs.erase(hostif->sai_object_id);
          sai_id_map_ptr->free_id(hostif->sai_object_id);
          return SAI_STATUS_INVALID_OBJECT_TYPE;
          break;  
      }
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
    // hostif->netdev.type = hostif->netdev_obj_type;
    switch (hostif->netdev_obj_type) {
        case SAI_OBJECT_TYPE_PORT:
          phys_netdev_sniffer(hostif->netdev_fd, hostif->netdev_obj.port->hw_port);
          break;
        case SAI_OBJECT_TYPE_LAG:
          // hostif->netdev_thread = std::thread(, hostif->netdev_fd,);
          break;
        case SAI_OBJECT_TYPE_VLAN:
          vlan_netdev_sniffer(hostif->netdev_fd, hostif->netdev_obj.vlan->vid);
          break;
    }
  }
  *hif_id = hostif->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_hostif(sai_object_id_t hif_id) {
  (*logger)->info("remove_hostif");
  HostIF_obj *hostif = switch_metadata_ptr->hostifs[hif_id];
  active_netdevs.erase(std::remove_if(active_netdevs.begin(),
                                      active_netdevs.end(),
                                      [&](const netdev_fd_t x)-> bool { return (x.fd == hostif->netdev_fd);}),
                       active_netdevs.end());
  write(sniff_pipe_fd[1], "b", 1);
  tun_delete(hostif->netdev_fd);
  switch_metadata_ptr->hostifs.erase(hostif->sai_object_id);
  sai_id_map_ptr->free_id(hostif->sai_object_id);
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::set_hostif_attribute(sai_object_id_t hif_id, const sai_attribute_t *attr) {
  (*logger)->info("set_hostif_attribute ({})", attr->id);
  HostIF_obj *hostif = switch_metadata_ptr->hostifs[hif_id];
  switch (attr->id) {
    case SAI_HOSTIF_ATTR_OPER_STATUS:
      hostif->oper_status = attr->value.booldata;
      break;
    default:
      (*logger)->info("unsupported hostif attribute {}", attr->id);
      return SAI_STATUS_NOT_IMPLEMENTED;
  }
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::get_hostif_attribute(sai_object_id_t hif_id, uint32_t attr_count, sai_attribute_t *attr_list) {
  (*logger)->info("get_hostif_attribute: hostif_id {}",
                   hif_id);
  HostIF_obj *hostif = switch_metadata_ptr->hostifs[hif_id];
  for (int i = 0; i < attr_count; i++) {
    (*logger)->info("attr_id = {}", attr_list[i].id);
    switch (attr_list[i].id) {
      case SAI_HOSTIF_ATTR_OPER_STATUS:
        attr_list[i].value.booldata = hostif->oper_status;
        break;
      default:
        (*logger)->error("attribute not supported");
        return SAI_STATUS_NOT_IMPLEMENTED;
    }
  }
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
      hostif_table_entry->entry_type =
          (sai_hostif_table_entry_type_t)attribute.value.s32;
      break;
    case SAI_HOSTIF_TABLE_ENTRY_ATTR_TRAP_ID:
      (*logger)->info("trap oid {}", attribute.value.oid);
      hostif_table_entry->trap_id =
          switch_metadata_ptr->hostif_traps[attribute.value.oid]->trap_id;
      (*logger)->info("trap id {}", hostif_table_entry->trap_id);
      break;
    case SAI_HOSTIF_TABLE_ENTRY_ATTR_CHANNEL_TYPE:
      hostif_table_entry->channel_type =
          (sai_hostif_table_entry_channel_type_t)attribute.value.s32;
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
  case SAI_HOSTIF_TABLE_ENTRY_CHANNEL_TYPE_NETDEV_L3:
    handler_fn = netdev_vlan_fn;
    (*logger)->info("netdev_vlan_fn");
    break;
  default:
    (*logger)->error("channel type not supported");
    return SAI_STATUS_NOT_SUPPORTED;
    break;
  }

  (*logger)->info("after parsing channel type");
  switch (hostif_table_entry->entry_type) {
  case SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID:
    (*logger)->info("add host if trap id. trap_id {}",
                    hostif_table_entry->trap_id);
    add_hostif_trap_id_table_entry(hostif_table_entry->trap_id, handler_fn);
    (*logger)->info("after add hostif table entry");
    break;
  case SAI_HOSTIF_TABLE_ENTRY_TYPE_WILDCARD:
    wildcard_entry = handler_fn;
    break;
  default:
    (*logger)->error("hostif table entry type not supported");
    return SAI_STATUS_NOT_IMPLEMENTED;
    break;
  }

  *hif_table_entry = hostif_table_entry->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t
sai_adapter::remove_hostif_table_entry(sai_object_id_t hif_table_entry) {
  (*logger)->info("remove_hostif_table_entry");
  HostIF_Table_Entry_obj *hostif_table_entry = switch_metadata_ptr->hostif_table_entries[hif_table_entry];
  switch (hostif_table_entry->entry_type) {
    case SAI_HOSTIF_TABLE_ENTRY_TYPE_WILDCARD:
      wildcard_entry = NULL;
      break;
    case SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID:
      // TODO
      break;
  }
  switch_metadata_ptr->hostif_table_entries.erase(hostif_table_entry->sai_object_id);
  sai_id_map_ptr->free_id(hostif_table_entry->sai_object_id);
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
  HostIF_Trap_Group_obj *hostif_trap_group = switch_metadata_ptr->hostif_trap_groups[hostif_trap_group_id];
  switch_metadata_ptr->hostif_trap_groups.erase(hostif_trap_group->sai_object_id);
  sai_id_map_ptr->free_id(hostif_trap_group->sai_object_id);
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

  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  action_data.clear();
  match_params.clear();
  switch (hostif_trap->trap_type) {

    // TODO: SAI_HOSTIF_TRAP_TYPE_TTL_ERROR
    case SAI_HOSTIF_TRAP_TYPE_TTL_ERROR:
      return SAI_STATUS_SUCCESS;
      break;

    // l2 trap
    case SAI_HOSTIF_TRAP_TYPE_LACP:    
      match_params.push_back(
          parse_exact_match_param(1652522221570, 6)); // dmac : 01-80-c2-00-00-02
      action_data.push_back(parse_param(hostif_trap->trap_id, 2));
      hostif_trap->handle_trap = bm_bridge_client_ptr->bm_mt_add_entry(
          cxt_id, "table_l2_trap", match_params, "action_set_trap_id",
          action_data, options);

      action_data.clear();
      match_params.clear();
      match_params.push_back(parse_exact_match_param(hostif_trap->trap_id, 2));
      switch (hostif_trap->trap_action) {
        case SAI_PACKET_ACTION_TRAP:
          hostif_trap->handle_trap_id = bm_bridge_client_ptr->bm_mt_add_entry(
              cxt_id, "table_trap_id", match_params, "action_trap_to_cpu",
              action_data, options);
          break;
        case SAI_PACKET_ACTION_LOG:
        case SAI_PACKET_ACTION_COPY:
          hostif_trap->handle_trap_id = bm_bridge_client_ptr->bm_mt_add_entry(
              cxt_id, "table_trap_id", match_params, "action_copy_to_cpu",
              action_data, options);
          break;
      }

      (*logger)->info("added LACP trap to cpu, trap_id: {}. sai_object_id: {}",
                      hostif_trap->trap_id, hostif_trap->sai_object_id);
      break;

    // Router pre-l3 traps
    case SAI_HOSTIF_TRAP_TYPE_ARP_REQUEST:
      match_params.push_back(parse_ternary_param(0x806, 2, 0xffff));
      match_params.push_back(parse_lpm_param(0, 4, 0));
      match_params.push_back(parse_ternary_param(1, 2, 0xffff));
      action_data.push_back(parse_param(hostif_trap->trap_id, 2));
      hostif_trap->handle_trap = bm_router_client_ptr->bm_mt_add_entry(
          cxt_id, "table_pre_l3_trap", match_params, "action_set_trap_id",
          action_data, options);

      action_data.clear();
      match_params.clear();
      match_params.push_back(parse_exact_match_param(hostif_trap->trap_id, 2));
      switch (hostif_trap->trap_action) {
        case SAI_PACKET_ACTION_TRAP:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_trap_to_cpu",
              action_data, options);
          break;
        case SAI_PACKET_ACTION_LOG:
        case SAI_PACKET_ACTION_COPY:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_copy_to_cpu",
              action_data, options);
          break;
      }

      (*logger)->info("added ARP REQUEST trap to cpu, trap_id: {}. sai_object_id: {}",
                      hostif_trap->trap_id, hostif_trap->sai_object_id);
      break;

    case SAI_HOSTIF_TRAP_TYPE_ARP_RESPONSE:
      match_params.push_back(parse_ternary_param(0x806, 2, 0xffff));
      match_params.push_back(parse_lpm_param(0, 4, 0));
      match_params.push_back(parse_ternary_param(2, 2, 0xffff));
      action_data.push_back(parse_param(hostif_trap->trap_id, 2));
      hostif_trap->handle_trap = bm_router_client_ptr->bm_mt_add_entry(
          cxt_id, "table_pre_l3_trap", match_params, "action_set_trap_id",
          action_data, options);

      action_data.clear();
      match_params.clear();
      match_params.push_back(parse_exact_match_param(hostif_trap->trap_id, 2));
      switch (hostif_trap->trap_action) {
        case SAI_PACKET_ACTION_TRAP:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_trap_to_cpu",
              action_data, options);
          break;
        case SAI_PACKET_ACTION_LOG:
        case SAI_PACKET_ACTION_COPY:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_copy_to_cpu",
              action_data, options);
          break;
      }

      (*logger)->info("added ARP RESPONSE trap to cpu, trap_id: {}. sai_object_id: {}",
                      hostif_trap->trap_id, hostif_trap->sai_object_id);
      break;



    // Router traps
    case SAI_HOSTIF_TRAP_TYPE_IP2ME:
      action_data.push_back(parse_param(hostif_trap->trap_id, 2));
      bm_router_client_ptr->bm_mt_set_default_action(
          cxt_id, "table_ip2me_trap", "action_set_trap_id",
          action_data);

      action_data.clear();
      match_params.clear();
      match_params.push_back(parse_exact_match_param(hostif_trap->trap_id, 2));
      switch (hostif_trap->trap_action) {
        case SAI_PACKET_ACTION_TRAP:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_trap_to_cpu",
              action_data, options);
          break;
        case SAI_PACKET_ACTION_LOG:
        case SAI_PACKET_ACTION_COPY:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_copy_to_cpu",
              action_data, options);
          break;
      }

      (*logger)->info("added IP2ME trap to cpu, trap_id: {}. sai_object_id: {}",
                      hostif_trap->trap_id, hostif_trap->sai_object_id);
      break;

    // Router ip2me traps
    case SAI_HOSTIF_TRAP_TYPE_BGP:    
      match_params.push_back(parse_exact_match_param(179, 2));
      match_params.push_back(parse_exact_match_param(179, 2));
      match_params.push_back(parse_exact_match_param(6, 1));
      action_data.push_back(parse_param(hostif_trap->trap_id, 2));
      hostif_trap->handle_trap = bm_router_client_ptr->bm_mt_add_entry(
          cxt_id, "table_ip2me_trap", match_params, "action_set_trap_id",
          action_data, options);

      action_data.clear();
      match_params.clear();
      match_params.push_back(parse_exact_match_param(hostif_trap->trap_id, 2));
      switch (hostif_trap->trap_action) {
        case SAI_PACKET_ACTION_TRAP:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_trap_to_cpu",
              action_data, options);
          break;
        case SAI_PACKET_ACTION_LOG:
        case SAI_PACKET_ACTION_COPY:
          hostif_trap->handle_trap_id = bm_router_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_trap_id", match_params, "action_copy_to_cpu",
              action_data, options);
          break;
      }

      (*logger)->info("added BGP trap to cpu, trap_id: {}. sai_object_id: {}",
                      hostif_trap->trap_id, hostif_trap->sai_object_id);
      break;

    case SAI_HOSTIF_TRAP_TYPE_IPV6_NEIGHBOR_DISCOVERY:
      (*logger)->info("added unsupported negihbor dicovery trap");
      break;

    default:
      (*logger)->error(
        "unsupported trap requested, trap type is: {}, trap_action is: {}",
        hostif_trap->trap_type, hostif_trap->trap_action);
      return SAI_STATUS_NOT_IMPLEMENTED;
      break;
  } 
  return SAI_STATUS_SUCCESS;
}

sai_status_t  sai_adapter::set_hostif_trap_group_attribute(
        _In_ sai_object_id_t hostif_trap_group_id,
        _In_ const sai_attribute_t *attr) {
  (*logger)->info("set_hostif_trap_group_attribute");
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::get_hostif_trap_group_attribute(
        _In_ sai_object_id_t hostif_trap_group_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
  (*logger)->info("get_hostif_trap_group_attribute");
  return SAI_STATUS_NOT_IMPLEMENTED;
}


sai_status_t sai_adapter::remove_hostif_trap(sai_object_id_t hostif_trap_id) {
  (*logger)->info("remove_hostif_trap trap_id: {}", hostif_trap_id);
  HostIF_Trap_obj *hostif_trap =
      switch_metadata_ptr->hostif_traps[hostif_trap_id];
  BmActionData action_data;
  switch (hostif_trap->trap_type) {
    // l2 traps
    case SAI_HOSTIF_TRAP_TYPE_LACP:
      bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_trap_id",
                                           hostif_trap->handle_trap_id);
      bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_l2_trap",
                                           hostif_trap->handle_trap);
      break;


    // pre-l3 traps
    case SAI_HOSTIF_TRAP_TYPE_ARP_RESPONSE:
    case SAI_HOSTIF_TRAP_TYPE_ARP_REQUEST:
      bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_l3_trap_id",
                                           hostif_trap->handle_trap_id);
      bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_pre_l3_trap",
                                           hostif_trap->handle_trap);
      break;

    // IP2ME trap
    case SAI_HOSTIF_TRAP_TYPE_IP2ME:
      bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_l3_trap_id",
                                           hostif_trap->handle_trap_id);
      bm_router_client_ptr->bm_mt_set_default_action(
          cxt_id, "table_ip2me_trap", "_drop", action_data);
      break;

    // post-IP2Me traps
    case SAI_HOSTIF_TRAP_TYPE_BGP:
      bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_l3_trap_id",
                                           hostif_trap->handle_trap_id);
      bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_ip2me_trap",
                                           hostif_trap->handle_trap);
      break;
  }
  switch_metadata_ptr->hostif_traps.erase(hostif_trap->sai_object_id);
  sai_id_map_ptr->free_id(hostif_trap->sai_object_id);
  return SAI_STATUS_SUCCESS;
}

void sai_adapter::add_hostif_trap_id_table_entry(
    uint16_t trap_id, adapter_packet_handler_fn handler_fn) {
  hostif_trap_id_table[trap_id] = handler_fn;
}

void sai_adapter::lookup_hostif_trap_id_table(u_char *packet, cpu_hdr_t *cpu,
                                              int pkt_len) {
  hostif_trap_id_table_t::iterator it = hostif_trap_id_table.find(cpu->trap_id);
  if (it != hostif_trap_id_table.end()) {
    it->second(packet, cpu, pkt_len);
    return;
  };
  if (wildcard_entry != NULL) {
    (*wildcard_entry)(packet, cpu, pkt_len);
    return;
  };
  (*logger)->error("hostif_table lookup failed");
}

void sai_adapter::netdev_phys_port_fn(u_char *packet, cpu_hdr_t *cpu,
                                      int pkt_len) {
  (*logger)->info(
      "trap arrived to physical netdev cahnnel @ ingress_port {}. len = {}",
      cpu->dst, pkt_len);
  HostIF_obj *hostif =
      switch_metadata_ptr->GetHostIFFromPhysicalPort(cpu->dst);
  if (hostif != nullptr && cpu->type == PORT) {
    write(hostif->netdev_fd, packet, pkt_len);
  }
  else if(cpu->type != PORT){
    (*logger)->debug(
      "bad cpu header type, expecting PORT - 0 but type is {}",
      cpu->type);
  }
  return;
}

void sai_adapter::phys_netdev_packet_handler(int hw_port, int length,
                                             const u_char *packet) {
  u_char *encaped_packet =
      (u_char *)malloc(sizeof(u_char) * (CPU_HDR_LEN + length));
  cpu_hdr_t *cpu_hdr = (cpu_hdr_t *)encaped_packet;
  cpu_hdr->type = PORT;
  cpu_hdr->dst = htons(hw_port);
  cpu_hdr->trap_id = htons(0xff);
  memcpy(encaped_packet + CPU_HDR_LEN, packet, length);
  // TODO: We should probably do this without copying the entire packet.
  //       currently not sure how to do it

  if (pcap_inject(cpu_port[0].pcap, encaped_packet, length + CPU_HDR_LEN) == -1) {
    (*logger)->error("error on injecting packet [%s]", pcap_geterr(cpu_port[0].pcap));
  }
  free(encaped_packet);
}

int sai_adapter::phys_netdev_sniffer(int in_dev_fd, int hw_port) {
  netdev_fd_t netdev;
  netdev.fd = in_dev_fd;
  netdev.type = SAI_OBJECT_TYPE_PORT;
  netdev.data.hw_port = hw_port;
  active_netdevs.push_back(netdev);
  write(sniff_pipe_fd[1], "b", 1);
}

void sai_adapter::netdev_vlan_fn(u_char *packet, cpu_hdr_t *cpu,
                                      int pkt_len) {
  vlan_hdr_t *vlan_hdr = (vlan_hdr_t *) (packet + ETHER_HDR_LEN);
  uint16_t vid = ntohs(vlan_hdr->tci) & 0x0fff;
  (*logger)->info(
      "trap arrived to vlan netdev cahnnel @ vlan_id {}. len = {}", vid, pkt_len);
  HostIF_obj *hostif =
      switch_metadata_ptr->GetHostIFFromVlanId(vid);
  if (hostif != nullptr) {
    write(hostif->netdev_fd, packet, pkt_len);
  }
  return;
}

void sai_adapter::vlan_netdev_packet_handler(uint16_t vlan_id, int length,
                                             const u_char *packet) {
  ethernet_hdr_t *ether_hdr = (ethernet_hdr_t *) packet;
  uint16_t ethertype = ntohs(ether_hdr->ether_type);
  (*logger)->info("recieved packet on vlan {0}, ethertype 0x{1:02X}.", vlan_id, ethertype);
  u_char *encaped_packet =
      (u_char *)malloc(sizeof(u_char) * (CPU_HDR_LEN + length));
  cpu_hdr_t *cpu_hdr = (cpu_hdr_t *)encaped_packet;
  cpu_hdr->type = VLAN;
  cpu_hdr->dst = htons(vlan_id);
  cpu_hdr->trap_id = htons(0xff);
  memcpy(encaped_packet + CPU_HDR_LEN, packet, length);
  // TODO: We should probably do this without copying the entire packet.
  //       currently not sure how to do it

  // sai_adapter *adapter = (sai_adapter*) arg_array[1];
  if (pcap_inject(cpu_port[1].pcap, encaped_packet, length + CPU_HDR_LEN) == -1) {
    (*logger)->debug("error on injecting packet [%s]\n", pcap_geterr(cpu_port[1].pcap));
  }
  free(encaped_packet);
}

int sai_adapter::vlan_netdev_sniffer(int in_dev_fd, uint16_t vlan_id) {
  netdev_fd_t netdev;
  netdev.fd = in_dev_fd;
  netdev.type = SAI_OBJECT_TYPE_VLAN;
  netdev.data.vid = vlan_id;
  active_netdevs.push_back(netdev);
  write(sniff_pipe_fd[1], "b", 1);
}