#include "sai_object.h"
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <standard_types.h>
#include <string>
StandardClient *sai_object::bm_client_ptr;
sai_id_map_t *sai_object::sai_id_map_ptr;
Switch_metadata *sai_object::switch_metadata_ptr;
std::vector<sai_object_id_t> *sai_object::switch_list_ptr;

std::string parse_param(uint64_t param, uint32_t num_of_bytes) {
  std::string my_string = std::string(
      static_cast<char *>(static_cast<void *>(&param)), num_of_bytes);
  std::reverse(my_string.begin(), my_string.end());
  return my_string;
}

BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes) {
  BmMatchParam match_param;
  match_param.type = BmMatchParamType::type::EXACT;
  BmMatchParamExact match_param_exact;
  match_param_exact.key = parse_param(param, num_of_bytes);
  match_param.__set_exact(match_param_exact);
  return match_param;
}

BmMatchParam parse_valid_match_param(bool param) {
  BmMatchParam match_param;
  match_param.type = BmMatchParamType::type::VALID;
  BmMatchParamValid match_param_valid;
  match_param_valid.key = param;
  match_param.__set_valid(match_param_valid);
  return match_param;
}

sai_status_t sai_object::create_switch(sai_object_id_t *switch_id,
                                       uint32_t attr_count,
                                       const sai_attribute_t *attr_list) {
  printf("create switch\n");
  if (switch_list_ptr->size() != 0) {
    printf("currently one switch is supportred, returning operating switch_id: "
           "%d\n",
           (*switch_list_ptr)[0]);
    return (*switch_list_ptr)[0];
  } else {
    BmAddEntryOptions options;
    BmMatchParams match_params;
    BmMtEntry entry;

    // Create Default 1Q Bridge, and switch_obj (not sure if switch is needed).
    Bridge_obj *bridge = new Bridge_obj(sai_id_map_ptr);
    printf("Default 1Q bridge. sai_object_id %d bridge_id %d\n",
           bridge->sai_object_id, bridge->bridge_id);
    Sai_obj *switch_obj = new Sai_obj(sai_id_map_ptr);
    switch_metadata_ptr->bridges[bridge->sai_object_id] = bridge;
    switch_metadata_ptr->default_bridge_id = bridge->sai_object_id;

    for (int i = 0; i < switch_metadata_ptr->hw_port_list.count; i++) {
      int hw_port = switch_metadata_ptr->hw_port_list.list[i];

      // Create Default Ports (one for each hw_port)
      Port_obj *port = new Port_obj(sai_id_map_ptr);
      switch_metadata_ptr->ports[port->sai_object_id] = port;
      port->hw_port = hw_port;
      port->l2_if = hw_port;
      printf("Default port_id %d. hw_port = %d\n", port->sai_object_id,
             port->hw_port);

      // Create Default Bridge ports and connect to 1Q bridge
      BridgePort_obj *bridge_port = new BridgePort_obj(sai_id_map_ptr);
      switch_metadata_ptr->bridge_ports[bridge_port->sai_object_id] =
          bridge_port;
      bridge_port->port_id = port->sai_object_id;
      bridge_port->bridge_port = hw_port;
      bridge_port->bridge_id = bridge->sai_object_id;
      bridge->bridge_port_list.push_back(bridge_port->sai_object_id);
      printf("Default bridge_port_id %d. bridge_port = %d\n",
             bridge_port->sai_object_id, bridge_port->bridge_port);

      // Store default table entries
      match_params.clear();
      match_params.push_back(parse_exact_match_param(port->l2_if, 1));
      bm_client_ptr->bm_mt_get_entry_from_key(
          entry, cxt_id, "table_port_ingress_interface_type", match_params,
          options);
      bridge_port->handle_port_ingress_interface_type = entry.entry_handle;
      match_params.clear();
      match_params.push_back(
          parse_exact_match_param(bridge_port->bridge_port, 1));
      bm_client_ptr->bm_mt_get_entry_from_key(
          entry, cxt_id, "table_egress_br_port_to_if", match_params, options);
      bridge_port->handle_egress_br_port_to_if = entry.entry_handle;
    }
    switch_list_ptr->push_back(switch_obj->sai_object_id);
    return switch_obj->sai_object_id;
  }
}

sai_status_t sai_object::get_switch_attribute(sai_object_id_t switch_id,
                                              sai_uint32_t attr_count,
                                              sai_attribute_t *attr_list) {
  printf("get_switch_attribute\n");
  int i;
  int index = 0;
  for (i = 0; i < attr_count; i++) {
    switch ((attr_list + i)->id) {
    case SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID:
      (attr_list + i)->value.oid = switch_metadata_ptr->default_bridge_id;
      // printf("--> attr_default_bridge_id -
      // %d\n",switch_metadata_ptr->default_bridge_id);
      break;
    case SAI_SWITCH_ATTR_PORT_LIST:
      for (port_id_map_t::iterator it = switch_metadata_ptr->ports.begin();
           it != switch_metadata_ptr->ports.end(); ++it) {
        (attr_list + i)->value.objlist.list[index] = it->first;
        // printf("--> attr_port_list %d: %d\n",index,it->first);
        index += 1;
      }
      break;
    case SAI_SWITCH_ATTR_PORT_NUMBER:
      (attr_list + i)->value.u32 = switch_metadata_ptr->hw_port_list.count;
      // printf("--> attr_port_num:
      // %d\n",switch_metadata_ptr->hw_port_list.count);
      break;
    }
  }
  return SAI_STATUS_SUCCESS;
}

void sai_object::set_parsed_port_attribute(Port_obj *port,
                                           sai_attribute_t attribute) {
  printf("set_parsed_port_attribute. attribute id = %d\n", attribute.id);
  printf("vlan = %d | bind_mode = %d | hw_lane_list = %d | drop_untagged = %d | drop_tagged = %d\n", 
    SAI_PORT_ATTR_PORT_VLAN_ID, SAI_PORT_ATTR_BIND_MODE, SAI_PORT_ATTR_HW_LANE_LIST,
    SAI_PORT_ATTR_DROP_UNTAGGED, SAI_PORT_ATTR_DROP_TAGGED);
  switch (attribute.id) {
  case SAI_PORT_ATTR_PORT_VLAN_ID:
    port->pvid = attribute.value.u16;
    break;
  case SAI_PORT_ATTR_BIND_MODE:
    port->bind_mode = attribute.value.s32;
    break;
  case SAI_PORT_ATTR_HW_LANE_LIST:
    port->hw_port = attribute.value.u32list.list[0];
    break;
  case SAI_PORT_ATTR_DROP_UNTAGGED:
    port->drop_untagged = attribute.value.booldata;
    printf("drop untagged debug = %d\n", attribute.value.booldata);
    break;
  case SAI_PORT_ATTR_DROP_TAGGED:
    port->drop_tagged = attribute.value.booldata;
    printf("drop tagged debug = %d\n", attribute.value.booldata);
    break;
  }
}

void sai_object::get_parsed_port_attribute(Port_obj *port,
                                           sai_attribute_t *attribute) {
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
  }
}

void sai_object::config_port(Port_obj *port) {
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
  printf("----> config port: l2_if = %d. drop_tagged = %d, drop_untagged = %d\n",port->l2_if, port->drop_tagged, port->drop_untagged);
  try { // need to change match - remove, than add entry!!!!!!!!!!!!
    BmMtEntry entry;
    bm_client_ptr->bm_mt_get_entry_from_key(
        entry, cxt_id, "table_port_configurations", match_params, options);  
    printf("----> deleting port_contifguration entry handle %d \n",entry.entry_handle); 
    bm_client_ptr->bm_mt_delete_entry(
        cxt_id, "table_port_configurations", entry.entry_handle);
  } 
  catch (...) {
    printf("----> InvalidTableOperation while removing table_port_configurations entry\n");
  }
  try{
    port->handle_port_cfg = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_port_configurations", match_params,
      "action_set_port_configurations", action_data, options);
    printf("----> added table_port_configurations entry handle: %d\n", port->handle_port_cfg);
  }
  catch (...) {
    printf("----> InvalidTableOperation while adding table_port_configurations entry\n");
  }
  // printf("port cfg modified, handle= %d\n",port->handle_port_cfg);
}

sai_status_t sai_object::set_port_attribute(sai_object_id_t port_id,
                                            const sai_attribute_t *attr) {
  Port_obj *port;
  port_id_map_t::iterator it = switch_metadata_ptr->ports.find(port_id);
  if (it != switch_metadata_ptr->ports.end()) {
    printf("set port %d attribute (port).\n", port_id);
    port = (Port_obj *)it->second;
  } else {
    printf("set port %d attribute (lag).\n", port_id);
    port = ((Lag_obj *)sai_id_map_ptr->get_object(port_id))->port_obj;
  }
  set_parsed_port_attribute(port, *attr);
  config_port(port);
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::get_port_attribute(sai_object_id_t port_id,
                                            uint32_t attr_count,
                                            sai_attribute_t *attr_list) {
  Port_obj *port = (Port_obj *)sai_id_map_ptr->get_object(port_id);
  for (int i = 0; i < attr_count; i++) {
    get_parsed_port_attribute(port, attr_list + i);
  }
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_port(sai_object_id_t *port_id,
                                     sai_object_id_t switch_id,
                                     uint32_t attr_count,
                                     const sai_attribute_t *attr_list) {
  // printf("sai_id_map addr in create port fn :%d\n",&sai_object::sai_id_map);
  Port_obj *port = new Port_obj(sai_id_map_ptr);
  // printf("switch_metadata_ptr->ports.size =
  // %d\n",switch_metadata_ptr->ports.size());
  switch_metadata_ptr->ports[port->sai_object_id] = port;
  // switch_metadata_ptr->ports.insert(std::make_pair(port->sai_object_id,&port));
  printf("--> new port sai_id = %d, tot port num: %d\n", port->sai_object_id,
         switch_metadata_ptr->ports.size());
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    set_parsed_port_attribute(port, attribute);
  }
  // printf("pvid %d, bind_mode %d, hw_port %d \n", port->pvid, port->bind_mode,
  // port->hw_port);
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  action_data.clear();
  match_params.clear();
  match_params.push_back(parse_exact_match_param(port->hw_port, 2));
  action_data.push_back(parse_param(0, 1));
  action_data.push_back(parse_param(port->sai_object_id, 1));
  port->handle_lag_if = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_ingress_lag", match_params, "action_set_lag_l2if",
      action_data, options);
  // printf("lag handle= %d, ",port->handle_lag_if);
  // config_port(port); Todo: Change to this
  action_data.clear();
  match_params.clear();
  match_params.push_back(parse_exact_match_param(port->sai_object_id, 1));
  action_data.push_back(parse_param(port->pvid, 2));
  action_data.push_back(parse_param(port->bind_mode, 1));
  action_data.push_back(parse_param(port->mtu, 4));
  action_data.push_back(parse_param(port->drop_tagged, 1));
  action_data.push_back(parse_param(port->drop_untagged, 1));
  port->handle_port_cfg = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_port_configurations", match_params,
      "action_set_port_configurations", action_data, options);
  // printf("port cfg handle= %d\n",port->handle_port_cfg);
  *port_id = port->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_port(sai_object_id_t port_id) {
  printf("sai_remove_port: %d \n", port_id);
  Port_obj *port = switch_metadata_ptr->ports[port_id];
  try {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_ingress_lag",
                                      port->handle_lag_if);
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_port_configurations",
                                      port->handle_port_cfg);
  } catch (int e) {
    printf("--> unable to remove port tables entries\n");
  }
  switch_metadata_ptr->ports.erase(port->sai_object_id);
  sai_id_map_ptr->free_id(port->sai_object_id);
  printf("--> ports.size after remove: %d\n", switch_metadata_ptr->ports.size());
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_bridge(sai_object_id_t *bridge_id,
                                       sai_object_id_t switch_id,
                                       uint32_t attr_count,
                                       const sai_attribute_t *attr_list) {
  uint32_t bridge_id_num = switch_metadata_ptr->GetNewBridgeID();
  Bridge_obj *bridge = new Bridge_obj(sai_id_map_ptr);
  switch_metadata_ptr->bridges[bridge->sai_object_id] = bridge;
  bridge->bridge_id = bridge_id_num;
  printf("--> Created new bridge %d (sai_object_id=%d)\n", bridge->bridge_id,
         bridge->sai_object_id);
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_BRIDGE_ATTR_TYPE:
      bridge->bridge_type = (sai_bridge_type_t)attribute.value.s32;
      break;
    }
  }
  *bridge_id = bridge->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_bridge(sai_object_id_t bridge_id) {
  Bridge_obj *bridge = switch_metadata_ptr->bridges[bridge_id];
  switch_metadata_ptr->bridges.erase(bridge->sai_object_id);
  sai_id_map_ptr->free_id(bridge->sai_object_id);
  // printf("bridges.size=%d\n",switch_metadata_ptr->bridges.size());
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_bridge_port(sai_object_id_t *bridge_port_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  uint32_t bridge_port_num = switch_metadata_ptr->GetNewBridgePort();
  BridgePort_obj *bridge_port = new BridgePort_obj(sai_id_map_ptr);
  switch_metadata_ptr->bridge_ports[bridge_port->sai_object_id] = bridge_port;
  bridge_port->bridge_port = bridge_port_num;
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_BRIDGE_PORT_ATTR_VLAN_ID:
      bridge_port->vlan_id = attribute.value.u16;
      break;
    case SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
      bridge_port->bridge_id = attribute.value.oid;
      break;
    case SAI_BRIDGE_PORT_ATTR_TYPE:
      bridge_port->bridge_port_type =
          (sai_bridge_port_type_t)attribute.value.s32;
      break;
    case SAI_BRIDGE_PORT_ATTR_PORT_ID:
      bridge_port->port_id = attribute.value.oid;
      break;
    }
  }
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  match_params.clear();
  action_data.clear();
  int32_t l2_if_type;
  uint32_t bridge_id =
      switch_metadata_ptr->bridges[bridge_port->bridge_id]->bridge_id;
  // 1D
  if (bridge_port->bridge_port_type == SAI_BRIDGE_PORT_TYPE_SUB_PORT) {
    match_params.push_back(
        parse_exact_match_param(bridge_port->bridge_port, 1));
    action_data.push_back(parse_param(bridge_id, 2));
    bridge_port->handle_id_1d = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_bridge_id_1d", match_params, "action_set_bridge_id",
        action_data, options);
    action_data.clear();
    action_data.push_back(parse_param(bridge_port->vlan_id, 2));
    bridge_port->handle_egress_set_vlan = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_set_vlan", match_params, "action_set_vlan",
        action_data, options);
    l2_if_type = 2;
  }
  // 1Q
  else if (bridge_port->bridge_port_type == SAI_BRIDGE_PORT_TYPE_PORT){
    match_params.clear();
    match_params.push_back(parse_exact_match_param(bridge_port->vlan_id, 2));
    action_data.clear();
    action_data.push_back(parse_param(bridge_id, 2));
    if(bridge_port->handle_id_1q == NULL_HANDLE){
      try {
        BmMtEntry entry;
        bm_client_ptr->bm_mt_get_entry_from_key(
            entry, cxt_id, "table_bridge_id_1q", match_params, options);  
        printf("--> deleting table_bridge_id_1q entry handle %d \n",entry.entry_handle); 
        bm_client_ptr->bm_mt_delete_entry(
            cxt_id, "table_bridge_id_1q", entry.entry_handle);
      } 
      catch (...) {
        printf("--> InvalidTableOperation while removing table_bridge_id_1q entry\n");
      }
      printf("--> adding table_bridge_id_1q entry, key:%d\n",bridge_port->vlan_id);
      bridge_port->handle_id_1q = bm_client_ptr->bm_mt_add_entry(
          cxt_id, "table_bridge_id_1q", match_params, "action_set_bridge_id",
          action_data, options);
    }
    l2_if_type = 3;
  }

  uint32_t bind_mode;
  uint32_t l2_if;
  uint32_t is_lag;
  port_id_map_t::iterator it =
      switch_metadata_ptr->ports.find(bridge_port->port_id);
  if (it != switch_metadata_ptr->ports.end()) { // port_id is port
    Port_obj *port = (Port_obj *)it->second;
    bind_mode = port->bind_mode;
    l2_if = port->l2_if;
    is_lag = 0;
  } else { // port_id is lag
    Lag_obj *lag = (Lag_obj *)sai_id_map_ptr->get_object(bridge_port->port_id);
    bind_mode = lag->port_obj->bind_mode;
    l2_if = lag->l2_if;
    is_lag = 1;
  }

  match_params.clear();
  match_params.push_back(parse_exact_match_param(bridge_port->bridge_port, 1));
  action_data.clear();
  action_data.push_back(parse_param(l2_if, 1));
  action_data.push_back(parse_param(is_lag, 1));
  std::cout << "--> adding table_egress_br_port_to_if : key:" << bridge_port->bridge_port<< endl;//match_params[0] << endl;
  bridge_port->handle_egress_br_port_to_if = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_egress_br_port_to_if", match_params,
      "action_forward_set_outIfType", action_data, options);
  if (bind_mode == SAI_PORT_BIND_MODE_SUB_PORT) {
    match_params.clear();
    match_params.push_back(
        parse_exact_match_param(l2_if, 1)); // TODO p4 table match is on l2_if
    match_params.push_back(parse_exact_match_param(bridge_port->vlan_id, 2));
    action_data.clear();
    action_data.push_back(parse_param(l2_if_type, 1));
    action_data.push_back(parse_param(bridge_port->bridge_port, 1));
    printf("--> Trying to create bridge_port. add "
           "table_subport_ingress_interface_type. with l2_if = %d, vid = %d\n",
           l2_if, bridge_port->vlan_id);
    bridge_port->handle_subport_ingress_interface_type =
        bm_client_ptr->bm_mt_add_entry(
            cxt_id, "table_subport_ingress_interface_type", match_params,
            "action_set_l2_if_type", action_data, options);
    printf("--> Created bridge port, entry handle %d\n",bridge_port->handle_subport_ingress_interface_type);
  } else {
    match_params.clear();
    match_params.push_back(parse_exact_match_param(l2_if, 1));
    action_data.clear();
    action_data.push_back(parse_param(l2_if_type, 1));
    action_data.push_back(parse_param(bridge_port->bridge_port, 1));
    printf("--> Trying to create bridge_port. add "
           "table_port_ingress_interface_type. with l2_if = %d\n",
           l2_if);
    bridge_port->handle_port_ingress_interface_type =
        bm_client_ptr->bm_mt_add_entry(
            cxt_id, "table_port_ingress_interface_type", match_params,
            "action_set_l2_if_type", action_data, options);
    printf("--> Created bridge port, entry handle %d\n",bridge_port->handle_port_ingress_interface_type);
  }
  *bridge_port_id = bridge_port->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_bridge_port(sai_object_id_t bridge_port_id) {
  sai_status_t status = SAI_STATUS_SUCCESS;
  BridgePort_obj *bridge_port =
      switch_metadata_ptr->bridge_ports[bridge_port_id];

  if (bridge_port->handle_id_1d != NULL_HANDLE) {
    try {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_bridge_id_1d",
                                      bridge_port->handle_id_1d);
    }catch(...){printf("--> DEBUG : Unable to remove table_bridge_id_1d entry, possible entry override\n");}
  }
  if (bridge_port->handle_id_1q != NULL_HANDLE) {
    try {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_bridge_id_1q",
                                      bridge_port->handle_id_1q);
    }catch(...){printf("--> DEBUG : Unable to remove table_bridge_id_1q entry, possible entry override\n");}
  }
  if (bridge_port->handle_egress_set_vlan != NULL_HANDLE) {
    try {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_set_vlan",
                                      bridge_port->handle_egress_set_vlan);
    }catch(...){printf("--> DEBUG : Unable to remove table_egress_set_vlan entry, possible entry override\n");}
  }
  if (bridge_port->handle_egress_br_port_to_if != NULL_HANDLE) {
    try{ 
      bm_client_ptr->bm_mt_delete_entry(
          cxt_id, "table_egress_br_port_to_if",
          bridge_port->handle_egress_br_port_to_if);
      }catch(...){printf("--> DEBUG : Unable to remove table_egress_br_port_to_if entry, possible entry override\n");}
  }
  if (bridge_port->handle_subport_ingress_interface_type != NULL_HANDLE) {
    try{
      bm_client_ptr->bm_mt_delete_entry(
          cxt_id, "table_subport_ingress_interface_type",
          bridge_port->handle_subport_ingress_interface_type);
    }catch(...){printf("--> DEBUG : Unable to remove table_subport_ingress_interface_type entry, possible entry override\n");}
  }
  if (bridge_port->handle_port_ingress_interface_type != NULL_HANDLE) {
    try{
      bm_client_ptr->bm_mt_delete_entry(
          cxt_id, "table_port_ingress_interface_type",
          bridge_port->handle_port_ingress_interface_type);
    }catch(...){printf("--> DEBUG : Unable to remove table_port_ingress_interface_type entry, possible entry override\n");}
  }
  printf("deleted bridge port %d bm_entries\n", bridge_port->sai_object_id);
  switch_metadata_ptr->bridge_ports.erase(bridge_port->sai_object_id);
  sai_id_map_ptr->free_id(bridge_port->sai_object_id);
  // printf("ports.size=%d\n",switch_metadata_ptr->ports.size());
  return status;
}

sai_status_t
sai_object::get_bridge_port_attribute(sai_object_id_t bridge_port_id,
                                      uint32_t attr_count,
                                      sai_attribute_t *attr_list) {
  BridgePort_obj *bridge_port =
      (BridgePort_obj *)sai_id_map_ptr->get_object(bridge_port_id);
  for (int i = 0; i < attr_count; i++) {
    switch ((attr_list + i)->id) {
    case SAI_BRIDGE_PORT_ATTR_PORT_ID:
      (attr_list + i)->value.oid = bridge_port->port_id;
      break;
    case SAI_BRIDGE_PORT_ATTR_VLAN_ID:
      (attr_list + i)->value.u16 = bridge_port->vlan_id;
      break;
    case SAI_BRIDGE_PORT_ATTR_TYPE:
      (attr_list + i)->value.s32 = bridge_port->bridge_port_type;
      break;
    case SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
      (attr_list + i)->value.oid = bridge_port->bridge_port_type;
      break;
    }
  }
}

sai_status_t sai_object::get_bridge_attribute(sai_object_id_t bridge_id,
                                              uint32_t attr_count,
                                              sai_attribute_t *attr_list) {
  int i;
  Bridge_obj *bridge = (Bridge_obj *)sai_id_map_ptr->get_object(bridge_id);
  for (i = 0; i < attr_count; i++) {
    switch ((attr_list + i)->id) {
    case SAI_BRIDGE_ATTR_TYPE:
      (attr_list + i)->value.s32 = bridge->bridge_type;
      break;
    case SAI_BRIDGE_ATTR_PORT_LIST:
      (attr_list + i)->value.objlist.count = bridge->bridge_port_list.size();
      (attr_list + i)->value.objlist.list = &bridge->bridge_port_list[0];
      break;
    }
  }
}

uint64_t parse_mac_64(uint8_t const mac_8[6]) {
  uint64_t mac_64 = 0;
  memcpy(&mac_64, mac_8, 6);
  return mac_64;
}

sai_status_t sai_object::create_fdb_entry(const sai_fdb_entry_t *fdb_entry,
                                          uint32_t attr_count,
                                          const sai_attribute_t *attr_list) {
  sai_status_t status = SAI_STATUS_SUCCESS;
  // parsing attributes

  uint32_t entry_type;
  uint32_t bridge_port;
  uint32_t packet_action;
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_FDB_ENTRY_ATTR_TYPE:
      entry_type = attribute.value.s32;
      // std::cout << "--> attr packet type="<<attribute.value.s32<<endl;
      // std::cout << "--> attr packet_static" << SAI_FDB_ENTRY_TYPE_STATIC
      // <<endl;
      break;
    case SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID:
      bridge_port =
          switch_metadata_ptr->bridge_ports[attribute.value.oid]->bridge_port;
      break;
    case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
      packet_action = attribute.value.s32;
      // std::cout << "--> attr packet_action="<<attribute.value.s32<<endl;
      // std::cout << "--> attr packet_action_fwd=" << SAI_PACKET_ACTION_FORWARD
      // <<endl;
      break;
    default:
      std::cout << "--> create_fdb_entry attribute.id = " << attribute.id << "was dumped in sai_obj"
                << endl;
      break;
    }
  }
  // out_if_type = 0 # port_type (not lag or router). TODO: check how to do it
  // with SAI
  uint32_t bridge_id =
      switch_metadata_ptr->bridges[fdb_entry->bridge_id]->bridge_id;
  if (packet_action == SAI_PACKET_ACTION_FORWARD) {
    std::cout << "--> SAI_PACKET_ACTION_FORWARD" << endl;
    if (entry_type == SAI_FDB_ENTRY_TYPE_STATIC) {
      std::cout << "--> SAI_FDB_ENTRY_TYPE_STATIC" << endl;
      BmAddEntryOptions options;
      BmMatchParams match_params;
      BmActionData action_data;
      uint64_t mac_address = parse_mac_64(fdb_entry->mac_address);
      match_params.push_back(parse_exact_match_param(mac_address, 6));
      match_params.push_back(parse_exact_match_param(bridge_id, 2));
      printf("--> mac: %d, b_id: %d\n", mac_address, bridge_id);
      action_data.push_back(parse_param(bridge_port, 1));
      bm_client_ptr->bm_mt_add_entry(cxt_id, "table_fdb", match_params,
                                     "action_set_egress_br_port", action_data,
                                     options);
    }
  }
  return status;
}

sai_status_t sai_object::remove_fdb_entry(const sai_fdb_entry_t *fdb_entry) {
  printf("remove_fdb_entry\n");
  sai_status_t status = SAI_STATUS_SUCCESS;
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  uint64_t mac_address = parse_mac_64(fdb_entry->mac_address);
  match_params.push_back(parse_exact_match_param(mac_address, 6));
  uint32_t bridge_id =
      switch_metadata_ptr->bridges[fdb_entry->bridge_id]->bridge_id;
  match_params.push_back(parse_exact_match_param(bridge_id, 2));
  printf("--> mac: %d, b_id: %d\n", mac_address, bridge_id);

  BmMtEntry bm_entry;
  // try{
  bm_client_ptr->bm_mt_get_entry_from_key(bm_entry, cxt_id, "table_fdb",
                                          match_params, options);
  printf("--> fdb entry handle %d\n", bm_entry.entry_handle);

  //} catch (int e) {
  //  printf("Unable to delete table. possible key missmatch:%d\n",mac_address);
  //  return SAI_STATUS_FAILURE;
  //};
  printf("--> deleting fdb handle\n");
  bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_fdb", bm_entry.entry_handle);
  printf("--> deleted: fdb handle\n");
  return status;
}

sai_status_t sai_object::create_vlan(sai_object_id_t *vlan_id,
                                     sai_object_id_t switch_id,
                                     uint32_t attr_count,
                                     const sai_attribute_t *attr_list) {
  printf("create_vlan \n");
  Vlan_obj *vlan = new Vlan_obj(sai_id_map_ptr);
  switch_metadata_ptr->vlans[vlan->sai_object_id] = vlan;
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_VLAN_ATTR_VLAN_ID:
      vlan->vid = (uint32_t)attribute.value.u16; // TODO correct casting type
      break;
    }
  }
  *vlan_id = vlan->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_vlan(sai_object_id_t vlan_id) {
  printf("remove_vlan: %d\n", vlan_id);
  Vlan_obj *vlan = switch_metadata_ptr->vlans[vlan_id];
  switch_metadata_ptr->vlans.erase(vlan->sai_object_id);
  sai_id_map_ptr->free_id(vlan->sai_object_id);
  // printf("vlans.size=%d\n",switch_metadata_ptr->vlans.size());
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_vlan_member(sai_object_id_t *vlan_member_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  printf("create_vlan_member\n");
  Vlan_member_obj *vlan_member = new Vlan_member_obj(sai_id_map_ptr);
  switch_metadata_ptr->vlan_members[vlan_member->sai_object_id] = vlan_member;
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_VLAN_MEMBER_ATTR_VLAN_ID:
      vlan_member->vlan_oid = (sai_object_id_t)attribute.value.oid;
      break;
    case SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID:
      vlan_member->bridge_port_id = (sai_object_id_t)attribute.value.oid;
      break;
    case SAI_VLAN_MEMBER_ATTR_VLAN_TAGGING_MODE:
      vlan_member->tagging_mode = (uint32_t)attribute.value.s32;
      break;
    default:
      std::cout << "while parsing vlan member, attribute.id = " << attribute.id
                << "was dumped in sai_obj" << endl;
      break;
    }
  }
  Vlan_obj *vlan = switch_metadata_ptr->vlans[vlan_member->vlan_oid];
  vlan_member->vid = vlan->vid;
  vlan->vlan_members.push_back(vlan_member->sai_object_id);
  uint32_t port_id =
      switch_metadata_ptr->bridge_ports[vlan_member->bridge_port_id]->port_id;
  uint32_t bridge_port =
      switch_metadata_ptr->bridge_ports[vlan_member->bridge_port_id]
          ->bridge_port;
  uint32_t out_if;
  if (switch_metadata_ptr->lags.find(port_id) !=
      switch_metadata_ptr->lags.end()) {
    out_if = switch_metadata_ptr->lags[port_id]->l2_if;
  } else {
    out_if = switch_metadata_ptr->ports[port_id]->hw_port;
  }
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  if (vlan_member->tagging_mode == SAI_VLAN_TAGGING_MODE_TAGGED) {
    uint32_t vlan_pcp = 0;
    uint32_t vlan_cfi = 0;
    match_params.push_back(parse_exact_match_param(out_if, 1));
    match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
    match_params.push_back(parse_valid_match_param(false));
    action_data.push_back(parse_param(vlan_pcp, 1));
    action_data.push_back(parse_param(vlan_cfi, 1));
    action_data.push_back(parse_param(vlan_member->vid, 2));
    vlan_member->handle_egress_vlan_tag = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_vlan_tag", match_params,
        "action_forward_vlan_tag", action_data, options);
  } else if (vlan_member->tagging_mode ==
             SAI_VLAN_TAGGING_MODE_PRIORITY_TAGGED) {
    uint32_t vlan_pcp = 0;
    uint32_t vlan_cfi = 0;
    match_params.push_back(parse_exact_match_param(out_if, 1));
    match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
    match_params.push_back(parse_valid_match_param(false));
    action_data.push_back(parse_param(vlan_pcp, 1));
    action_data.push_back(parse_param(vlan_cfi, 1));
    action_data.push_back(parse_param(0, 2));
    vlan_member->handle_egress_vlan_tag = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_vlan_tag", match_params,
        "action_forward_vlan_tag", action_data, options);
  } else {
    match_params.push_back(parse_exact_match_param(out_if, 1));
    match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
    match_params.push_back(parse_valid_match_param(true));
    action_data.clear();
    vlan_member->handle_egress_vlan_tag = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_vlan_tag", match_params,
        "action_forward_vlan_untag", action_data, options);
  }
  match_params.clear();
  match_params.push_back(parse_exact_match_param(bridge_port, 1));
  match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
  action_data.clear();
  vlan_member->handle_egress_vlan_filtering = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_egress_vlan_filtering", match_params, "_nop", action_data,
      options);
  vlan_member->handle_ingress_vlan_filtering = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_ingress_vlan_filtering", match_params, "_nop", action_data,
      options);
  *vlan_member_id = vlan_member->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_vlan_member(sai_object_id_t vlan_member_id) {
  printf("remove vlan_member: vlan_member_id = %d\n", vlan_member_id);
  sai_status_t status = SAI_STATUS_SUCCESS;
  Vlan_member_obj *vlan_member =
      switch_metadata_ptr->vlan_members[vlan_member_id];
  try {
    bm_client_ptr->bm_mt_delete_entry(
        cxt_id, "table_egress_vlan_filtering",
        vlan_member->handle_egress_vlan_filtering);
    bm_client_ptr->bm_mt_delete_entry(
        cxt_id, "table_ingress_vlan_filtering",
        vlan_member->handle_ingress_vlan_filtering);
    if (vlan_member->tagging_mode == SAI_VLAN_TAGGING_MODE_TAGGED) {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_vlan_tag",
                                        vlan_member->handle_egress_vlan_tag);
    } else if (vlan_member->tagging_mode ==
               SAI_VLAN_TAGGING_MODE_PRIORITY_TAGGED) {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_vlan_tag",
                                        vlan_member->handle_egress_vlan_tag);
    } else {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_vlan_tag",
                                        vlan_member->handle_egress_vlan_tag);
    }
  } catch (int e) {
    printf("ERROR : Unable to delete tables.\n");
    return SAI_STATUS_FAILURE;
  };
  // get parent vlan member list, and remove the memeber by value - TODO
  // consider map instead of vector.
  std::vector<sai_object_id_t> *vlan_members =
      &switch_metadata_ptr->vlans[vlan_member->vlan_oid]->vlan_members;
  vlan_members->erase(std::remove(vlan_members->begin(), vlan_members->end(),
                                  vlan_member->sai_object_id),
                      vlan_members->end());
  switch_metadata_ptr->vlan_members.erase(vlan_member->sai_object_id);
  sai_id_map_ptr->free_id(vlan_member->sai_object_id);
  return status;
}

sai_status_t sai_object::set_vlan_attribute(sai_object_id_t vlan_id,
                                            const sai_attribute_t *attr) {
  printf("TODO : set_vlan_attribute not implemened\n");
  return SAI_STATUS_NOT_IMPLEMENTED;
  // implementation
}
sai_status_t sai_object::get_vlan_attribute(sai_object_id_t vlan_id,
                                            const uint32_t attr_count,
                                            sai_attribute_t *attr_list) {
  printf("TODO : get_vlan_attribute not implemened\n");
  return SAI_STATUS_NOT_IMPLEMENTED;
  // implementation
}
sai_status_t
sai_object::set_vlan_member_attribute(sai_object_id_t vlan_member_id,
                                      const sai_attribute_t *attr) {
  printf("TODO : set_vlan_member_attribute not implemened\n");
  return SAI_STATUS_NOT_IMPLEMENTED;

  // implementation
}
sai_status_t
sai_object::get_vlan_member_attribute(sai_object_id_t vlan_member_id,
                                      const uint32_t attr_count,
                                      sai_attribute_t *attr_list) {
  printf("TODO : get_vlan_member_attribute not implemened\n");
  return SAI_STATUS_NOT_IMPLEMENTED;
  // implementation
}
sai_status_t sai_object::get_vlan_stats(sai_object_id_t vlan_id,
                                        const sai_vlan_stat_t *counter_ids,
                                        uint32_t number_of_counters,
                                        uint64_t *counters) {
  printf("TODO : get_vlan_stats not implemened\n");
  return SAI_STATUS_NOT_IMPLEMENTED;

  // implementation
}
sai_status_t sai_object::clear_vlan_stats(sai_object_id_t vlan_id,
                                          const sai_vlan_stat_t *counter_ids,
                                          uint32_t number_of_counters) {
  printf("TODO : clear_vlan_stats not implemened\n");
  // implementation
}

sai_status_t sai_object::create_lag(sai_object_id_t *lag_id,
                                    sai_object_id_t switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list) {
  printf("create_lag\n");
  Lag_obj *lag = new Lag_obj(sai_id_map_ptr);
  lag->l2_if = switch_metadata_ptr->GetNewL2IF();
  switch_metadata_ptr->lags[lag->sai_object_id] = lag;
  *lag_id = lag->sai_object_id;
  return SAI_STATUS_SUCCESS;
}
sai_status_t sai_object::remove_lag(sai_object_id_t lag_id) {
  printf("remove_lag: %d\n", lag_id);
  sai_status_t status = SAI_STATUS_SUCCESS;
  Lag_obj *lag = switch_metadata_ptr->lags[lag_id];
  try {
    if (lag->handle_port_configurations != NULL_HANDLE) {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_port_configurations",
                                        lag->handle_port_configurations);
    }
    if (lag->handle_lag_hash != NULL_HANDLE) {
      bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_lag_hash",
                                        lag->handle_lag_hash);
    }
  } catch (int e) {
    status = SAI_STATUS_FAILURE;
    printf("ERROR : unable to remove lag tables entries\n");
  }
  switch_metadata_ptr->lags.erase(lag->sai_object_id);
  sai_id_map_ptr->free_id(lag->sai_object_id);
  return status;
}
sai_status_t sai_object::create_lag_member(sai_object_id_t *lag_member_id,
                                           sai_object_id_t switch_id,
                                           uint32_t attr_count,
                                           const sai_attribute_t *attr_list) {
  printf("create_lag_member \n");
  Lag_member_obj *lag_member = new Lag_member_obj(sai_id_map_ptr);
  switch_metadata_ptr->lag_members[lag_member->sai_object_id] = lag_member;
  Port_obj *port;
  Lag_obj *lag;
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_LAG_MEMBER_ATTR_PORT_ID:
      port = switch_metadata_ptr->ports[attribute.value.oid];
      lag_member->port = port;
      break;
    case SAI_LAG_MEMBER_ATTR_LAG_ID:
      lag = switch_metadata_ptr->lags[attribute.value.oid];
      lag_member->lag = lag;
      lag->lag_members.push_back(lag_member->sai_object_id);
      break;
    default:
      std::cout << "while parsing lag member, attribute.id = " << attribute.id
                << "was dumped in sai_obj" << endl;
      break;
    }
  }
  lag->port_obj = port;
  uint32_t l2_if = lag->l2_if;
  if (port->handle_ingress_lag != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_ingress_lag",
                                      port->handle_ingress_lag);
    port->handle_ingress_lag = NULL_HANDLE;
  }
  if (lag->handle_lag_hash != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_lag_hash",
                                      lag->handle_lag_hash);
    lag->handle_lag_hash = NULL_HANDLE;
  }

  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  match_params.push_back(parse_exact_match_param(port->hw_port, 2));
  action_data.push_back(parse_param(1, 1));
  action_data.push_back(parse_param(lag->sai_object_id, 1));
  port->handle_ingress_lag = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_ingress_lag", match_params, "action_set_lag_l2if",
      action_data, options);
  printf("lag l2_if %d. port %d (hw %d). ingress_lag_handle = %d\n",
         lag->l2_if ,port->sai_object_id, port->hw_port, port->handle_ingress_lag);
  match_params.clear();
  match_params.push_back(parse_exact_match_param(l2_if, 1));
  match_params.push_back(
      parse_exact_match_param(lag->lag_members.size() - 1, 1));
  action_data.clear();
  action_data.push_back(parse_param(port->hw_port, 1));
  lag_member->handle_egress_lag = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_egress_lag", match_params, "action_set_out_port",
      action_data, options);
  action_data.clear();
  match_params.clear();
  match_params.push_back(parse_exact_match_param(l2_if, 1));
  action_data.push_back(parse_param(lag->lag_members.size(), 1));
  lag->handle_lag_hash = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_lag_hash", match_params, "action_set_lag_hash_size",
      action_data, options);
  uint32_t port_l2if = port->l2_if;
  port->l2_if = l2_if;
  config_port(port);
  port->l2_if = port_l2if;
  *lag_member_id = lag_member->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

void print_lag_members(std::vector<sai_object_id_t> v) {
  printf("lag memebers:\n");
  for (std::vector<sai_object_id_t>::iterator it = v.begin(); it != v.end();
       ++it) {
    printf("%d ", *it);
  }
  printf("\n");
}

sai_status_t sai_object::remove_lag_member(sai_object_id_t lag_member_id) {
  printf("sai_remove_lag_member: %d\n", lag_member_id);
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  Lag_member_obj *lag_member = switch_metadata_ptr->lag_members[lag_member_id];
  Lag_obj *lag = lag_member->lag;
  sai_status_t status = SAI_STATUS_SUCCESS;
  std::vector<sai_object_id_t>::iterator iter = std::find(
      lag->lag_members.begin(), lag->lag_members.end(), lag_member_id);
  size_t hash_index = std::distance(lag->lag_members.begin(), iter);
  printf("hash_index = %d\n", hash_index);
  print_lag_members(lag->lag_members);
  lag->lag_members.erase(lag->lag_members.begin() + hash_index);
  print_lag_members(lag->lag_members);
  bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_lag_hash",
                                    lag->handle_lag_hash);
  action_data.clear();
  match_params.clear();
  match_params.push_back(parse_exact_match_param(lag->l2_if, 1));
  action_data.push_back(parse_param(lag->lag_members.size(), 1));
  lag->handle_lag_hash = bm_client_ptr->bm_mt_add_entry(
      cxt_id, "table_lag_hash", match_params, "action_set_lag_hash_size",
      action_data, options);
  // printf("lag %d. port %d . ingress_lag_handle = %d\n",lag->sai_object_id,
  // lag->port->sai_object_id, lag->port->handle_ingress_lag);
  bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_ingress_lag",
                                    lag_member->port->handle_ingress_lag);
  lag_member->port->handle_ingress_lag = NULL_HANDLE;
  bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_lag",
                                    lag_member->handle_egress_lag);
  lag_member->handle_egress_lag = NULL_HANDLE;

  if (hash_index != lag->lag_members.size()) {
    sai_object_id_t last_lag_member_id = lag->lag_members.back();
    Lag_member_obj *last_lag_member = switch_metadata_ptr->lag_members[last_lag_member_id];
    lag->lag_members.pop_back();
    lag->lag_members.insert(lag->lag_members.begin() + hash_index,
                            last_lag_member_id);
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_lag",
                                      last_lag_member->handle_egress_lag);
    match_params.clear();
    match_params.push_back(parse_exact_match_param(lag->l2_if, 1));
    match_params.push_back(parse_exact_match_param(hash_index, 1));
    action_data.clear();
    action_data.push_back(parse_param(last_lag_member->port->hw_port, 1));
    last_lag_member->handle_egress_lag = bm_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_lag", match_params, "action_set_out_port",
        action_data, options);
  }
  print_lag_members(lag->lag_members);
  switch_metadata_ptr->lag_members.erase(lag_member->sai_object_id);
  sai_id_map_ptr->free_id(lag_member->sai_object_id);
  return status;
}
