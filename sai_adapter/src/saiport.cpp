#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_port(sai_object_id_t *port_id,
                                      sai_object_id_t switch_id,
                                      uint32_t attr_count,
                                      const sai_attribute_t *attr_list) {
  Port_obj *port = new Port_obj(sai_id_map_ptr);
  switch_metadata_ptr->ports[port->sai_object_id] = port;
  (*logger)->info("--> new port sai_id = {}, tot port num: {}",
                  port->sai_object_id, switch_metadata_ptr->ports.size());
  // parsing attributes
  for (uint32_t i = 0; i < attr_count; i++) {
    set_parsed_port_attribute(port, *(attr_list+i));
  }
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  action_data.clear();
  match_params.clear();
  match_params.push_back(parse_exact_match_param(port->hw_port, 2));
  action_data.push_back(parse_param(0, 1));
  action_data.push_back(parse_param(port->sai_object_id, 1));
  port->handle_lag_if = bm_bridge_client_ptr->bm_mt_add_entry(
      cxt_id, "table_ingress_lag", match_params, "action_set_lag_l2if",
      action_data, options);
  action_data.clear();
  match_params.clear();
  match_params.push_back(parse_exact_match_param(port->sai_object_id, 1));
  action_data.push_back(parse_param(port->pvid, 2));
  action_data.push_back(parse_param(port->bind_mode, 1));
  action_data.push_back(parse_param(port->mtu, 4));
  action_data.push_back(parse_param(port->drop_tagged, 1));
  action_data.push_back(parse_param(port->drop_untagged, 1));
  port->handle_port_cfg = bm_bridge_client_ptr->bm_mt_add_entry(
      cxt_id, "table_port_configurations", match_params,
      "action_set_port_configurations", action_data, options);
  *port_id = port->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_port(sai_object_id_t port_id) {
  (*logger)->info("sai_remove_port: {} ", port_id);
  Port_obj *port = switch_metadata_ptr->ports[port_id];
  try {
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_ingress_lag",
                                      port->handle_lag_if);
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_port_configurations",
                                      port->handle_port_cfg);
  } catch (...) {
    (*logger)->info("--> unable to remove port tables entries");
  }
  switch_metadata_ptr->ports.erase(port->sai_object_id);
  sai_id_map_ptr->free_id(port->sai_object_id);
  (*logger)->debug("--> ports.size after remove: {}",
                   switch_metadata_ptr->ports.size());
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::set_port_attribute(sai_object_id_t port_id,
                                             const sai_attribute_t *attr) {
  Port_obj *port;
  port_id_map_t::iterator it = switch_metadata_ptr->ports.find(port_id);
  if (it != switch_metadata_ptr->ports.end()) {
    (*logger)->info("set port {} attribute (port).", port_id);
    port = it->second;
  } else {
    (*logger)->info("set port {} attribute (lag).", port_id);
    port = switch_metadata_ptr->lags[port_id]->port_obj;
  }
  if (set_parsed_port_attribute(port, *attr)) {
    config_port(port);
  }
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::get_port_attribute(sai_object_id_t port_id,
                                             uint32_t attr_count,
                                             sai_attribute_t *attr_list) {
  (*logger)->info("get_port_attribute (id {})", port_id);
  sai_status_t status = SAI_STATUS_SUCCESS;
  sai_status_t curr_status;
  Port_obj *port = (Port_obj *)sai_id_map_ptr->get_object(port_id);
  for (int i = 0; i < attr_count; i++) {
    curr_status = get_parsed_port_attribute(port, attr_list + i);
    if (curr_status != SAI_STATUS_SUCCESS) {
      (*logger)->warn("failed getting attribute {} (status: {}).", attr_list[i].id, curr_status);
      status = curr_status;
    }
  }
  return status;
}

bool sai_adapter::set_parsed_port_attribute(Port_obj *port,
                                            sai_attribute_t attribute1) {
  sai_attribute_t *attribute = &attribute1;
  (*logger)->info("set_parsed_port_attribute. attribute id = {}", attribute->id);
  switch (attribute->id) {
    case SAI_PORT_ATTR_PORT_VLAN_ID:
      port->pvid = attribute->value.u16;
      return true;
      break;
    case SAI_PORT_ATTR_BIND_MODE:
      port->bind_mode = attribute->value.s32;
      return true;
      break;
    case SAI_PORT_ATTR_HW_LANE_LIST:
      port->hw_port = attribute->value.u32list.list[0];
      return true;
      break;
    case SAI_PORT_ATTR_DROP_UNTAGGED:
      port->drop_untagged = attribute->value.booldata;
      return true;
      break;
    case SAI_PORT_ATTR_DROP_TAGGED:
      port->drop_tagged = attribute->value.booldata;
      return true;
      break;
    case SAI_PORT_ATTR_ADMIN_STATE:
      port->admin_state = attribute->value.booldata;
      (*logger)->info("setting port admin state {}", port->admin_state);
      sai_port_oper_status_notification_t data[1];
      data[0].port_id = port->sai_object_id;
      data[0].port_state = (port->admin_state) ? SAI_PORT_OPER_STATUS_UP : SAI_PORT_OPER_STATUS_DOWN;
      if (switch_metadata_ptr->port_state_change_notification_fn != NULL) {
        (*logger)->info("port state change notification started");
        (*switch_metadata_ptr->port_state_change_notification_fn)(1, data);
        (*logger)->info("port state change notification ended");
      }
      return false;
      break;
    default:
      (*logger)->warn("port attribute not supported");
      return false;
      break;
  }
}

sai_status_t sai_adapter::get_parsed_port_attribute(Port_obj *port,
                                            sai_attribute_t *attribute) {
  (*logger)->info("attr_id {}", attribute->id);
  switch (attribute->id) {
    case SAI_PORT_ATTR_PORT_VLAN_ID:
      attribute->value.u16 = port->pvid;
      break;
    case SAI_PORT_ATTR_BIND_MODE:
      attribute->value.s32 = port->bind_mode;
      break;
    case SAI_PORT_ATTR_HW_LANE_LIST:
      attribute->value.u32list.count = 1;
      attribute->value.u32list.list[0] = port->hw_port;
      break;
    case SAI_PORT_ATTR_DROP_UNTAGGED:
      attribute->value.booldata = port->drop_untagged;
      break;
    case SAI_PORT_ATTR_DROP_TAGGED:
      attribute->value.booldata = port->drop_tagged;
      break;
    case SAI_PORT_ATTR_OPER_STATUS:
      (*logger)->info("getting port oper status. (admin state {})", port->admin_state);
      attribute->value.s32 = (port->admin_state) ? SAI_PORT_OPER_STATUS_UP : SAI_PORT_OPER_STATUS_DOWN; //TODO: add linux port status?
      break;
    case SAI_PORT_ATTR_NUMBER_OF_INGRESS_PRIORITY_GROUPS:
      attribute->value.u32 = 0;
      break;
    case SAI_PORT_ATTR_QOS_NUMBER_OF_QUEUES:
      attribute->value.u32 = 0;
      break;
    case SAI_PORT_ATTR_QOS_QUEUE_LIST:
    case SAI_PORT_ATTR_QOS_SCHEDULER_GROUP_LIST:
    case SAI_PORT_ATTR_INGRESS_PRIORITY_GROUP_LIST:
    case SAI_PORT_ATTR_INGRESS_MIRROR_SESSION:
    case SAI_PORT_ATTR_EGRESS_MIRROR_SESSION:
    case SAI_PORT_ATTR_QOS_INGRESS_BUFFER_PROFILE_LIST:
    case SAI_PORT_ATTR_QOS_EGRESS_BUFFER_PROFILE_LIST:
    case SAI_PORT_ATTR_EGRESS_BLOCK_PORT_LIST:
      attribute->value.objlist.count = 0;
    default:
      (*logger)->error("unsupported attr_id {}", attribute->id);
      return SAI_STATUS_NOT_IMPLEMENTED;
      break;
  }
  return SAI_STATUS_SUCCESS;
}

void sai_adapter::config_port(Port_obj *port) {
  BmAddEntryOptions options;
  BmMatchParams match_params;
  match_params.clear();
  match_params.push_back(parse_exact_match_param(port->l2_if, 1));
  BmActionData action_data;
  action_data.clear();
  action_data.push_back(parse_param(port->pvid, 2));
  action_data.push_back(parse_param(port->bind_mode, 1));
  action_data.push_back(parse_param(port->mtu, 4));
  action_data.push_back(parse_param(port->drop_tagged, 1));
  action_data.push_back(parse_param(port->drop_untagged, 1));
  try {
    BmMtEntry entry;
    bm_bridge_client_ptr->bm_mt_get_entry_from_key(
        entry, cxt_id, "table_port_configurations", match_params, options);
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_port_configurations",
                                      entry.entry_handle);
  } catch (...) {
    (*logger)->warn("--> InvalidTableOperation while removing "
                    "table_port_configurations entry");
  }
  try {
    port->handle_port_cfg = bm_bridge_client_ptr->bm_mt_add_entry(
        cxt_id, "table_port_configurations", match_params,
        "action_set_port_configurations", action_data, options);
  } catch (...) {
    (*logger)->warn("--> InvalidTableOperation while adding "
                    "table_port_configurations entry");
  }
}

sai_status_t sai_adapter::get_port_stats(sai_object_id_t port_id, const sai_port_stat_t *counter_ids, uint32_t number_of_counters, uint64_t *counters) {
  return SAI_STATUS_NOT_IMPLEMENTED;
}