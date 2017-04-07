#include "../inc/sai_adapter.h"
// extern "C" {
//   int tun_alloc(char*);
// }

sai_status_t sai_adapter::create_hostif(sai_object_id_t *hif_id,
                                        sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list) {
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
    tun_alloc(netdev_name, 1);
  }
  *hif_id = hostif->sai_object_id;
}

sai_status_t sai_adapter::remove_hostif(sai_object_id_t hif_id) {
  (*logger)->info("remove_hostif");
}

sai_status_t sai_adapter::create_hostif_table_entry(
    sai_object_id_t *hif_table_entry, sai_object_id_t switch_id,
    uint32_t attr_count, const sai_attribute_t *attr_list) {
  (*logger)->info("create_hostif_table_entry");
  HostIF_Table_Entry_obj *hostif_table_entry =
      new HostIF_Table_Entry_obj(sai_id_map_ptr);
  switch_metadata_ptr->hostif_table_entries[hostif_table_entry->sai_object_id] =
      hostif_table_entry;

  *hif_table_entry = hostif_table_entry->sai_object_id;
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
}

sai_status_t
sai_adapter::remove_hostif_trap_group(sai_object_id_t hostif_trap_group_id) {
  (*logger)->info("remove_hostif_trap_group");
}

sai_status_t sai_adapter::create_hostif_trap(sai_object_id_t *hostif_trap_id,
                                             sai_object_id_t switch_id,
                                             uint32_t attr_count,
                                             const sai_attribute_t *attr_list) {
  (*logger)->info("create_hostif_trap");
  // parsing attributes
  HostIF_Trap_obj *hostif_trap = new HostIF_Trap_obj(sai_id_map_ptr);
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
  switch_metadata_ptr->hostif_traps[hostif_trap->sai_object_id] = hostif_trap;
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
        parse_param(hostif_trap->sai_object_id, 2)); // trap_id
    hostif_trap->handle_l2_trap = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_l2_trap", match_params, "action_set_trap_id",
        action_data, options);
    (*logger)->info("added l2 trap - lacp , trap_id: {}",
                    hostif_trap->sai_object_id);

    action_data.clear();
    match_params.clear();
    match_params.push_back(
        parse_exact_match_param(hostif_trap->sai_object_id, 2)); // trap_id
    hostif_trap->handle_trap_id = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_trap_id", match_params, "action_trap_to_cpu",
        action_data, options);
    (*logger)->info("added LACP trap to cpu, trap_id: {}",
                    hostif_trap->sai_object_id);
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

void sai_adapter::lookup_hostif_trap_id_table(ethernet_hdr_t *ether,
                                              cpu_hdr_t *cpu) {
  hostif_trap_id_table_t::iterator it = hostif_trap_id_table.find(cpu->trap_id);
  if (it != hostif_trap_id_table.end()) {
    it->second(ether, cpu);
    return;
  } else {
    printf("hostif_table lookup failed\n"); // TODO logger / return value
  }
}