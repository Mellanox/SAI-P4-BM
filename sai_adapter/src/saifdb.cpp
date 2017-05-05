#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_fdb_entry(const sai_fdb_entry_t *fdb_entry,
                                           uint32_t attr_count,
                                           const sai_attribute_t *attr_list) {
  (*logger)->info("create_fdb_entry");
  sai_status_t status = SAI_STATUS_SUCCESS;
  // parsing attributes
  sai_fdb_entry_type_t entry_type;

  BridgePort_obj *bridge_port_obj;
  sai_packet_action_t packet_action;
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_FDB_ENTRY_ATTR_TYPE:
      entry_type = (sai_fdb_entry_type_t)attribute.value.s32;
      // (*logger)->error("--> attr packet type="<<attribute.value.s32<<endl;
      // (*logger)->error("--> attr packet_static" <<
      // SAI_FDB_ENTRY_TYPE_STATIC
      // <<endl;
      break;
    case SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID:
      (*logger)->info("bridge port_id {}", attribute.value.oid);
      bridge_port_obj = switch_metadata_ptr->bridge_ports[attribute.value.oid];
      break;
    case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
      packet_action = (sai_packet_action_t)attribute.value.s32;
      // (*logger)->error("--> attr
      // packet_action="<<attribute.value.s32<<endl;
      // (*logger)->error("--> attr packet_action_fwd=" <<
      // SAI_PACKET_ACTION_FORWARD
      // <<endl;
      break;
    default:
      (*logger)->error(
          "create_fdb_entry attribute.id = {} was dumped in sai_obj",
          attribute.id);
      break;
    }
  }
  uint32_t bridge_port = bridge_port_obj->bridge_port;
  (*logger)->info("bridge port {}", bridge_port);
  // out_if_type = 0 # port_type (not lag or router). TODO: check how to do it
  // with SAI

  uint32_t bridge_id = get_bridge_id_from_fdb_entry(fdb_entry);
  if (!bridge_port_obj->does_fdb_exist(bridge_id)) {
    (*logger)->info("create fdb - bridge_id = {}", bridge_id);
    BmEntryHandle handle_fdb;
    BmEntryHandle handle_learn_fdb;
    if (packet_action == SAI_PACKET_ACTION_FORWARD) {
      if (entry_type == SAI_FDB_ENTRY_TYPE_STATIC) {
        BmAddEntryOptions options;
        BmMatchParams match_params;
        BmActionData action_data;
        uint64_t mac_address = parse_mac_64(fdb_entry->mac_address);
        match_params.push_back(parse_exact_match_param(mac_address, 6));
        match_params.push_back(parse_exact_match_param(bridge_id, 2));
        (*logger)->info("--> b_id: {}. mac: ", bridge_id);
        print_mac_to_log(fdb_entry->mac_address, *logger);
        action_data.push_back(parse_param(bridge_port, 1));
        try {
          handle_fdb = bm_bridge_client_ptr->bm_mt_add_entry(
              cxt_id, "table_fdb", match_params, "action_set_egress_br_port",
              action_data, options);
          action_data.clear();
          handle_learn_fdb = bm_bridge_client_ptr->bm_mt_add_entry(
              cxt_id, "table_learn_fdb", match_params, "_nop", action_data,
              options);
          bridge_port_obj->set_fdb_handle(handle_fdb, handle_learn_fdb,
                                          bridge_id);
        } catch (...) {
          (*logger)->warn("trying to add existing fdb_entry");
        }
      }
    }
  } else {
    (*logger)->warn("trying to add existing fdb_entry");
  }
  return status;
}

sai_status_t sai_adapter::remove_fdb_entry(const sai_fdb_entry_t *fdb_entry) {
  (*logger)->info("remove_fdb_entry");
  sai_status_t status = SAI_STATUS_SUCCESS;
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  uint64_t mac_address = parse_mac_64(fdb_entry->mac_address);
  match_params.push_back(parse_exact_match_param(mac_address, 6));
  uint32_t bridge_id = get_bridge_id_from_fdb_entry(fdb_entry);
  match_params.push_back(parse_exact_match_param(bridge_id, 2));
  BmMtEntry bm_entry;
  bm_bridge_client_ptr->bm_mt_get_entry_from_key(bm_entry, cxt_id, "table_fdb",
                                          match_params, options);

  std::string bridge_port_str = bm_entry.action_entry.action_data.back();
  uint32_t *bridge_port = (uint32_t *)bridge_port_str.c_str();
  BridgePort_obj *bridge_port_obj =
      switch_metadata_ptr->GetBrPortObjFromBrPort(*bridge_port);
  (*logger)->info("removing fdb from bridge_port: {}. sai_id {}",
                  bridge_port_obj->bridge_port, bridge_port_obj->sai_object_id);
  bridge_port_obj->remove_fdb_handle(bridge_id);
  bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb", bm_entry.entry_handle);
  bm_bridge_client_ptr->bm_mt_get_entry_from_key(bm_entry, cxt_id, "table_learn_fdb",
                                          match_params, options);

  bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_learn_fdb",
                                    bm_entry.entry_handle);

  return status;
}

sai_status_t sai_adapter::flush_fdb_entries(sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("flush_fdb_entries");
  sai_status_t status = SAI_STATUS_SUCCESS;

  // parsing attributes
  // sai_fdb_entry_type_t entry_type;
  BridgePort_obj *bridge_port_obj;

  sai_attribute_t attribute;
  int mode = 0; // 1 - bridge_port, 2 - vlan, 3 - bridge_port and vlan, etc..
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID:
      bridge_port_obj = switch_metadata_ptr->bridge_ports[attribute.value.oid];
      mode = mode | 1;
      break;
    default:
      (*logger)->error(
          "flush_fdb_entries attribute.id = {} was dumped in sai_obj",
          attribute.id);
      break;
    }
  }

  if (mode == 1) {
    (*logger)->info("flushing entries by bridge port (id: {})",
                    bridge_port_obj->sai_object_id);
    if (bridge_port_obj->bridge_port_type == SAI_BRIDGE_PORT_TYPE_SUB_PORT) {

      bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb",
                                        bridge_port_obj->handle_fdb_sub_port);
      bm_bridge_client_ptr->bm_mt_delete_entry(
          cxt_id, "table_learn_fdb",
          bridge_port_obj->handle_fdb_learn_sub_port);
      bridge_port_obj->handle_fdb_sub_port = NULL_HANDLE;
      bridge_port_obj->handle_fdb_learn_sub_port = NULL_HANDLE;
    } else {
      for (std::map<uint32_t, BmEntryHandle>::iterator it =
               bridge_port_obj->handle_fdb_port.begin();
           it != bridge_port_obj->handle_fdb_port.end(); ++it) {
        bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb", it->second);
        bridge_port_obj->handle_fdb_port.erase(it->first);
      }
      for (std::map<uint32_t, BmEntryHandle>::iterator it =
               bridge_port_obj->handle_fdb_learn_port.begin();
           it != bridge_port_obj->handle_fdb_learn_port.end(); ++it) {
        bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_learn_fdb",
                                          it->second);
        bridge_port_obj->handle_fdb_learn_port.erase(it->first);
      }
    }
  }
}

uint32_t
sai_adapter::get_bridge_id_from_fdb_entry(const sai_fdb_entry_t *fdb_entry) {
  if (fdb_entry->bridge_type == SAI_FDB_ENTRY_BRIDGE_TYPE_1Q) {
    sai_object_id_t vlan_obj_id =
        switch_metadata_ptr->GetVlanObjIdFromVid(fdb_entry->vlan_id);
    if (vlan_obj_id != 0) {
      return switch_metadata_ptr->vlans[vlan_obj_id]->bridge_id;
    } else {
      return fdb_entry->vlan_id;
    }
  } else {
    return switch_metadata_ptr->bridges[fdb_entry->bridge_id]->bridge_id;
  }
}
