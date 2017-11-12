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
      break;
    case SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID:
      (*logger)->info("bridge port_id {}", attribute.value.oid);
      bridge_port_obj = switch_metadata_ptr->bridge_ports[attribute.value.oid];
      break;
    case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
      packet_action = (sai_packet_action_t)attribute.value.s32;
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
  uint32_t bridge_id = get_bridge_id_from_fdb_entry(fdb_entry);
  if (!bridge_port_obj->does_fdb_exist(bridge_id)) {
    (*logger)->info("create fdb - bridge_id = {}", bridge_id);
    BmEntryHandle handle_fdb;
    BmEntryHandle handle_learn_fdb;
    if (packet_action == SAI_PACKET_ACTION_FORWARD) {
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
        handle_fdb = bm_client_ptr->bm_mt_add_entry(
            cxt_id, "table_fdb", match_params, "action_set_egress_br_port",
            action_data, options);
        action_data.clear();
        handle_learn_fdb = bm_client_ptr->bm_mt_add_entry(
            cxt_id, "table_learn_fdb", match_params, "nop", action_data,
            options);
        bridge_port_obj->set_fdb_handle(handle_fdb, handle_learn_fdb,
                                        bridge_id, entry_type);
      } catch (...) {
        (*logger)->warn("trying to add existing fdb_entry");
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
  bm_client_ptr->bm_mt_get_entry_from_key(bm_entry, cxt_id, "table_fdb",
                                          match_params, options);

  std::string bridge_port_str = bm_entry.action_entry.action_data.back();
  uint32_t *bridge_port = (uint32_t *)bridge_port_str.c_str();
  BridgePort_obj *bridge_port_obj =
      switch_metadata_ptr->GetBrPortObjFromBrPort(*bridge_port);
  (*logger)->info("removing fdb from bridge_port: {}. sai_id {}",
                  bridge_port_obj->bridge_port, bridge_port_obj->sai_object_id);
  bridge_port_obj->remove_fdb_handle(bridge_id);
  bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb", bm_entry.entry_handle);
  bm_client_ptr->bm_mt_get_entry_from_key(bm_entry, cxt_id, "table_learn_fdb",
                                          match_params, options);

  bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_learn_fdb",
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
  Vlan_obj *vlan;
  uint16_t vid;
  uint32_t bridge_id;
  sai_fdb_entry_type_t  entry_type;
  sai_attribute_t attribute;
  int mode = 0; 
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID:
      bridge_port_obj = switch_metadata_ptr->bridge_ports[attribute.value.oid];
      mode |= 1;
      break;
    case SAI_FDB_FLUSH_ATTR_VLAN_ID:
      vid = attribute.value.u16;
      vlan = switch_metadata_ptr->vlans[switch_metadata_ptr->GetVlanObjIdFromVid(vid)];
      if (vlan == SAI_NULL_OBJECT_ID) {
        (*logger)->error("trying to flush fdb with non existing vid {}", vid);
        return SAI_STATUS_INVALID_VLAN_ID;
      }
      bridge_id = vlan->bridge_id;
      mode |= 2;
      break;
    case SAI_FDB_FLUSH_ATTR_ENTRY_TYPE:
      entry_type = (sai_fdb_entry_type_t) attribute.value.s32;
      mode |= 4;
      break;
    default:
      (*logger)->error(
          "flush_fdb_entries attribute.id = {} was dumped in sai_obj",
          attribute.id);
      break;
    }
  }  

  switch(mode) {
    case 1: // flush by bridge_port
    case 5:
      (*logger)->info("flushing entries by bridge port (id: {})",
                      bridge_port_obj->sai_object_id);
      if (bridge_port_obj->bridge_port_type == SAI_BRIDGE_PORT_TYPE_SUB_PORT) {
        if ((mode == 1) or (bridge_port_obj->fdb_entry_type_sub_port == entry_type)) {
          bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb",
                                            bridge_port_obj->handle_fdb_sub_port);
          bm_client_ptr->bm_mt_delete_entry(
              cxt_id, "table_learn_fdb",
              bridge_port_obj->handle_fdb_learn_sub_port);
          bridge_port_obj->handle_fdb_sub_port = NULL_HANDLE;
          bridge_port_obj->handle_fdb_learn_sub_port = NULL_HANDLE;
        } 
      } else {
        for (std::map<uint32_t, BmEntryHandle>::iterator it =
                 bridge_port_obj->handle_fdb_port.begin();
             it != bridge_port_obj->handle_fdb_port.end(); ++it) {
          if ((mode == 1) or (bridge_port_obj->fdb_entry_type_port[it->first] == entry_type)) {
            bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb", it->second);
            bm_client_ptr->bm_mt_delete_entry(
              cxt_id, "table_learn_fdb",
              bridge_port_obj->handle_fdb_learn_port[it->first]);
            bridge_port_obj->handle_fdb_port.erase(it->first);
            bridge_port_obj->handle_fdb_learn_port.erase(it->first);
          }
        }
      }
      break;
    case 2: // flush by vlan_id
    case 6:
      (*logger)->info("flushing entries by vlan id {} (bridge_id {})", vid, bridge_id);
      for (bridge_port_id_map_t::iterator it = switch_metadata_ptr->bridge_ports.begin(); it!=switch_metadata_ptr->bridge_ports.end(); ++it) {
        if (it->second->does_fdb_exist(bridge_id)) {
          if ((mode == 2) or (entry_type == it->second->fdb_entry_type_port[bridge_id])) {
            (*logger)->info("removing bridge_port {}", it->first);
            bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb",
                                              it->second->handle_fdb_port[bridge_id]);
            bm_client_ptr->bm_mt_delete_entry(
                cxt_id, "table_learn_fdb",
                it->second->handle_fdb_learn_port[bridge_id]);
                it->second->handle_fdb_learn_port.erase(bridge_id);
                it->second->handle_fdb_port.erase(bridge_id);
          }
        }
      }
      break;
    case 3: // flush by vlan_id and bridge_port
    case 7:
      (*logger)->info("flushing entries by vlan id {} and bridge_port id {}", vid, bridge_port_obj->sai_object_id);
      if (bridge_port_obj->does_fdb_exist(bridge_id)) {
        if ((mode == 3) or (entry_type == bridge_port_obj->fdb_entry_type_port[bridge_id])) {
          bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb",
                                            bridge_port_obj->handle_fdb_port[bridge_id]);
          bm_client_ptr->bm_mt_delete_entry(
              cxt_id, "table_learn_fdb",
              bridge_port_obj->handle_fdb_learn_port[bridge_id]);

          bridge_port_obj->handle_fdb_learn_port.erase(bridge_id);
          bridge_port_obj->handle_fdb_port.erase(bridge_id);
        }
      }
      break;
    case 4:
      (*logger)->info("flushing all fdb entries of type {}", (entry_type == SAI_FDB_ENTRY_TYPE_STATIC) ? "STATIC" : "DYNAMIC");
      for (bridge_port_id_map_t::iterator it_bp = switch_metadata_ptr->bridge_ports.begin(); it_bp!=switch_metadata_ptr->bridge_ports.end(); ++it_bp) {
        if (it_bp->second->bridge_port_type == SAI_BRIDGE_PORT_TYPE_PORT) {
          for (std::map<uint32_t, BmEntryHandle>::iterator it =
                   it_bp->second->handle_fdb_port.begin();
               it != it_bp->second->handle_fdb_port.end(); ++it) {
            if ((it_bp->second->does_fdb_exist(it->first)) and (it_bp->second->fdb_entry_type_port[it->first] == entry_type)) {
              bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb", it->second);
              bm_client_ptr->bm_mt_delete_entry(
              cxt_id, "table_learn_fdb",
              it_bp->second->handle_fdb_learn_port[it->first]);
              it_bp->second->handle_fdb_port.erase(it->first);
              it_bp->second->handle_fdb_learn_port.erase(it->first);
            }
          }
        } else if (it_bp->second->bridge_port_type == SAI_BRIDGE_PORT_TYPE_SUB_PORT) {
          if (it_bp->second->does_fdb_exist(0)) {
            bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb",
                                            it_bp->second->handle_fdb_sub_port);
            bm_client_ptr->bm_mt_delete_entry(
                cxt_id, "table_learn_fdb",
                it_bp->second->handle_fdb_learn_sub_port);
            it_bp->second->handle_fdb_sub_port = NULL_HANDLE;
            it_bp->second->handle_fdb_learn_sub_port = NULL_HANDLE;
          }
        }
      }
    default:
      (*logger)->error("fdb_flush with unsupported mode");
      break;
  }
  return SAI_STATUS_SUCCESS;
}

uint32_t
sai_adapter::get_bridge_id_from_fdb_entry(const sai_fdb_entry_t *fdb_entry) {
  (*logger)->info("get_bridge_id_from_fdb_entry. fdb_entry->bridge_type {} (SAI_FDB_ENTRY_BRIDGE_TYPE_1Q={})", fdb_entry->bridge_type, SAI_FDB_ENTRY_BRIDGE_TYPE_1Q);
  if (fdb_entry->bridge_type == SAI_FDB_ENTRY_BRIDGE_TYPE_1Q) {
    sai_object_id_t vlan_obj_id =
        switch_metadata_ptr->GetVlanObjIdFromVid(fdb_entry->vlan_id);
    if (vlan_obj_id != SAI_NULL_OBJECT_ID) {
      return switch_metadata_ptr->vlans[vlan_obj_id]->bridge_id;
    } else {
      return fdb_entry->vlan_id;
    }
  } else {
    return switch_metadata_ptr->bridges[fdb_entry->bridge_id]->bridge_id;
  }
}
