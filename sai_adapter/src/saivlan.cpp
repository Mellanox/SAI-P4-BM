#include "../inc/sai_adapter.h"
#include <bitset>

sai_status_t sai_adapter::create_vlan(sai_object_id_t *vlan_id,
                                      sai_object_id_t switch_id,
                                      uint32_t attr_count,
                                      const sai_attribute_t *attr_list) {
  (*logger)->info("create_vlan ");
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

  // config tables
  BmMatchParams match_params;
  BmActionData action_data;
  BmAddEntryOptions options;
  uint32_t bridge_id = switch_metadata_ptr->GetNewBridgeID(vlan->vid);
  vlan->bridge_id = bridge_id;
  vlan->handle_mc_mgrp = bm_bridge_client_mc_ptr->bm_mc_mgrp_create(cxt_id, bridge_id);
  vlan->handle_mc_l1 = bm_bridge_client_mc_ptr->bm_mc_node_create(cxt_id, bridge_id, "", "");
  bm_bridge_client_mc_ptr->bm_mc_node_associate(cxt_id, vlan->handle_mc_mgrp, vlan->handle_mc_l1);
  match_params.push_back(parse_exact_match_param(bridge_id, 2));
  action_data.push_back(parse_param(bridge_id, 4));
  vlan->handle_broadcast = bm_bridge_client_ptr->bm_mt_add_entry(
        cxt_id, "table_broadcast", match_params, "action_forward_mc_set_if_list",
        action_data, options);
  vlan->handle_flood = bm_bridge_client_ptr->bm_mt_add_entry(
        cxt_id, "table_flood", match_params, "action_forward_mc_set_if_list",
        action_data, options);
  match_params.clear();
  action_data.clear();

  // vlan->handle_mc_mgrp = 0;
  // vlan->handle_mc_l1 = 0;
  if (vlan->vid != bridge_id) {
    match_params.push_back(parse_exact_match_param(vlan->vid, 2));
    action_data.push_back(parse_param(bridge_id, 2));
    vlan->handle_id_1q = bm_bridge_client_ptr->bm_mt_add_entry(
        cxt_id, "table_bridge_id_1q", match_params, "action_set_bridge_id",
        action_data, options);
  }

  match_params.clear();
  match_params.push_back(parse_exact_match_param(switch_metadata_ptr->router_bridge_port->bridge_port, 1));
  match_params.push_back(parse_exact_match_param(vlan->vid, 2));
  action_data.clear();
  vlan->handle_router_ingress_vlan_filtering = bm_bridge_client_ptr->bm_mt_add_entry(
      cxt_id, "table_ingress_vlan_filtering", match_params, "nop", action_data,
      options);
  vlan->handle_router_egress_vlan_filtering = bm_bridge_client_ptr->bm_mt_add_entry(
      cxt_id, "table_egress_vlan_filtering", match_params, "nop", action_data,
      options);
  *vlan_id = vlan->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_vlan(sai_object_id_t vlan_id) {
  (*logger)->info("remove_vlan: {}", vlan_id);
  Vlan_obj *vlan = switch_metadata_ptr->vlans[vlan_id];
  if (vlan->handle_id_1q != NULL_HANDLE) {
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_bridge_id_1q",
                                      vlan->handle_id_1q);
  }
  if (vlan->handle_router_ingress_vlan_filtering != NULL_HANDLE) { 
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_ingress_vlan_filtering",
                                      vlan->handle_router_ingress_vlan_filtering);
  }
  if (vlan->handle_router_egress_vlan_filtering != NULL_HANDLE) { 
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_vlan_filtering",
                                      vlan->handle_router_egress_vlan_filtering);
  }
  if (vlan->handle_broadcast != NULL_HANDLE) { 
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_broadcast",
                                      vlan->handle_broadcast);
  }
  if (vlan->handle_flood != NULL_HANDLE) { 
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_flood",
                                      vlan->handle_flood);
  }
  if (vlan->handle_mc_mgrp != NULL_HANDLE) { 
    bm_bridge_client_mc_ptr->bm_mc_mgrp_destroy(cxt_id, vlan->handle_mc_mgrp);
  }
  if (vlan->handle_mc_l1 != NULL_HANDLE) { 
    bm_bridge_client_mc_ptr->bm_mc_node_destroy(cxt_id, vlan->handle_mc_l1);
  }
  switch_metadata_ptr->vlans.erase(vlan->sai_object_id);
  sai_id_map_ptr->free_id(vlan->sai_object_id);
  // (*logger)->info("vlans.size={}",switch_metadata_ptr->vlans.size());
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::create_vlan_member(sai_object_id_t *vlan_member_id,
                                             sai_object_id_t switch_id,
                                             uint32_t attr_count,
                                             const sai_attribute_t *attr_list) {
  (*logger)->info("create_vlan_member");
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
      (*logger)->error(
          "while parsing vlan member, attribute.id = {} was dumped in "
          "sai_obj",
          attribute.id);
      break;
    }
  }
  Vlan_obj *vlan = switch_metadata_ptr->vlans[vlan_member->vlan_oid];
  vlan_member->vid = vlan->vid;
  vlan->vlan_members.push_back(vlan_member->sai_object_id);
  update_mc_node_vlan(vlan);
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
    match_params.push_back(parse_exact_match_param(0, 1));
    action_data.push_back(parse_param(vlan_pcp, 1));
    action_data.push_back(parse_param(vlan_cfi, 1));
    action_data.push_back(parse_param(vlan_member->vid, 2));
    vlan_member->handle_egress_vlan_tag = bm_bridge_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_vlan_tag", match_params,
        "action_forward_vlan_tag", action_data, options);
  } else if (vlan_member->tagging_mode ==
             SAI_VLAN_TAGGING_MODE_PRIORITY_TAGGED) {
    uint32_t vlan_pcp = 0;
    uint32_t vlan_cfi = 0;
    match_params.push_back(parse_exact_match_param(out_if, 1));
    match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
    match_params.push_back(parse_exact_match_param(0, 1));
    action_data.push_back(parse_param(vlan_pcp, 1));
    action_data.push_back(parse_param(vlan_cfi, 1));
    action_data.push_back(parse_param(0, 2));
    vlan_member->handle_egress_vlan_tag = bm_bridge_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_vlan_tag", match_params,
        "action_forward_vlan_tag", action_data, options);
  } else {
    (*logger)->info("table_egress_vlan_tag debug.  out_if = {}, vid = {}",
                    out_if, vlan_member->vid);
    match_params.push_back(parse_exact_match_param(out_if, 1));
    match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
    match_params.push_back(parse_exact_match_param(1, 1));
    action_data.clear();
    vlan_member->handle_egress_vlan_tag = bm_bridge_client_ptr->bm_mt_add_entry(
        cxt_id, "table_egress_vlan_tag", match_params,
        "action_forward_vlan_untag", action_data, options);
  }
  match_params.clear();
  match_params.push_back(parse_exact_match_param(bridge_port, 1));
  match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
  action_data.clear();
  vlan_member->handle_egress_vlan_filtering = bm_bridge_client_ptr->bm_mt_add_entry(
      cxt_id, "table_egress_vlan_filtering", match_params, "nop", action_data,
      options);
  vlan_member->handle_ingress_vlan_filtering = bm_bridge_client_ptr->bm_mt_add_entry(
      cxt_id, "table_ingress_vlan_filtering", match_params, "nop", action_data,
      options);
  *vlan_member_id = vlan_member->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_vlan_member(sai_object_id_t vlan_member_id) {
  (*logger)->info("remove vlan_member: vlan_member_id = {}", vlan_member_id);
  sai_status_t status = SAI_STATUS_SUCCESS;
  Vlan_member_obj *vlan_member =
      switch_metadata_ptr->vlan_members[vlan_member_id];
  try {
    bm_bridge_client_ptr->bm_mt_delete_entry(
        cxt_id, "table_egress_vlan_filtering",
        vlan_member->handle_egress_vlan_filtering);
    bm_bridge_client_ptr->bm_mt_delete_entry(
        cxt_id, "table_ingress_vlan_filtering",
        vlan_member->handle_ingress_vlan_filtering);
    if (vlan_member->tagging_mode == SAI_VLAN_TAGGING_MODE_TAGGED) {
      bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_vlan_tag",
                                        vlan_member->handle_egress_vlan_tag);
    } else if (vlan_member->tagging_mode ==
               SAI_VLAN_TAGGING_MODE_PRIORITY_TAGGED) {
      bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_vlan_tag",
                                        vlan_member->handle_egress_vlan_tag);
    } else {
      bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_vlan_tag",
                                        vlan_member->handle_egress_vlan_tag);
    }
  } catch (...) {
    (*logger)->warn(" Unable to delete vlan member tables.");
    return SAI_STATUS_FAILURE;
  };
  // get parent vlan member list, and remove the memeber by value - TODO
  // consider map instead of vector.
  std::vector<sai_object_id_t> *vlan_members =
      &switch_metadata_ptr->vlans[vlan_member->vlan_oid]->vlan_members;
  vlan_members->erase(std::remove(vlan_members->begin(), vlan_members->end(),
                                  vlan_member->sai_object_id),
                      vlan_members->end());
  update_mc_node_vlan(switch_metadata_ptr->vlans[vlan_member->vlan_oid]);
  switch_metadata_ptr->vlan_members.erase(vlan_member->sai_object_id);
  sai_id_map_ptr->free_id(vlan_member->sai_object_id);
  return status;
}

sai_status_t sai_adapter::set_vlan_attribute(sai_object_id_t vlan_id,
                                             const sai_attribute_t *attr) {
  (*logger)->info("TODO : set_vlan_attribute not implemened");
  return SAI_STATUS_NOT_IMPLEMENTED;
  // implementation
}
sai_status_t sai_adapter::get_vlan_attribute(sai_object_id_t vlan_id,
                                             const uint32_t attr_count,
                                             sai_attribute_t *attr_list) {
  (*logger)->info("get_vlan_attribute");
  Vlan_obj *vlan = switch_metadata_ptr->vlans[vlan_id];
  for (int i=0; i<attr_count; ++i) {
    switch(attr_list[i].id) {
      case SAI_VLAN_ATTR_VLAN_ID:
        attr_list[i].value.u16  = vlan->vid;
        break;
      case SAI_VLAN_ATTR_MEMBER_LIST:
        std::copy(vlan->vlan_members.begin(), vlan->vlan_members.end(), attr_list[i].value.objlist.list);
        attr_list[i].value.objlist.count = vlan->vlan_members.size();
        break;
    }
  }
  return SAI_STATUS_SUCCESS;
  // implementation
}
sai_status_t
sai_adapter::set_vlan_member_attribute(sai_object_id_t vlan_member_id,
                                       const sai_attribute_t *attr) {
  (*logger)->info("TODO : set_vlan_member_attribute not implemened");
  return SAI_STATUS_NOT_IMPLEMENTED;

  // implementation
}
sai_status_t
sai_adapter::get_vlan_member_attribute(sai_object_id_t vlan_member_id,
                                       const uint32_t attr_count,
                                       sai_attribute_t *attr_list) {
  (*logger)->info("get_vlan_member_attribute");
  int i;
  Vlan_member_obj *vlan_member = switch_metadata_ptr->vlan_members[vlan_member_id];
  for (i = 0; i < attr_count; i++) {
    switch (attr_list[i].id) {
      case SAI_VLAN_MEMBER_ATTR_VLAN_ID:
        attr_list[i].value.oid = vlan_member->vlan_oid;
        break;
      case SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID:
        attr_list[i].value.oid = vlan_member->bridge_port_id;
        break;
      default:
        (*logger)->error("vlan_member attribute {} not supported", attr_list[i].id);
        return SAI_STATUS_NOT_IMPLEMENTED;
    }
  }
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::get_vlan_stats(sai_object_id_t vlan_id,
                                         uint32_t number_of_counters,
                                         const sai_vlan_stat_t *counter_ids,
                                         uint64_t *counters) {
  (*logger)->info("TODO : get_vlan_stats not implemened");
  return SAI_STATUS_NOT_IMPLEMENTED;

  // implementation
}
sai_status_t sai_adapter::clear_vlan_stats(sai_object_id_t vlan_id,
                                           uint32_t number_of_counters,
                                           const sai_vlan_stat_t *counter_ids) {
  (*logger)->info("TODO : clear_vlan_stats not implemened");
  return SAI_STATUS_NOT_IMPLEMENTED;
  // implementation
}

sai_status_t sai_adapter::create_vlan_members(
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t object_count,
        _In_ const uint32_t *attr_count,
        _In_ const sai_attribute_t **attrs,
        _In_ sai_bulk_op_type_t type,
        _Out_ sai_object_id_t *object_id,
        _Out_ sai_status_t *object_statuses) {
  (*logger)->info("create_vlan_members");
  return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::remove_vlan_members(
        _In_ uint32_t object_count,
        _In_ const sai_object_id_t *object_id,
        _In_ sai_bulk_op_type_t type,
        _Out_ sai_status_t *object_statuses) {
  (*logger)->info("remove_vlan_members");
  return SAI_STATUS_NOT_IMPLEMENTED;
}



void sai_adapter::update_mc_node_vlan(Vlan_obj *vlan) {
  std::string port_map;
  uint32_t bridge_port;
  uint32_t ports_int = 0;
  for (std::vector<sai_object_id_t>::iterator it = vlan->vlan_members.begin(); it != vlan->vlan_members.end(); ++it) {
    bridge_port = switch_metadata_ptr->bridge_ports[switch_metadata_ptr->vlan_members[*it]->bridge_port_id]->bridge_port;
    ports_int += 1 << bridge_port;
  }
  // ports_int += 1 << switch_metadata_ptr->router_bridge_port->bridge_port;
  std::bitset<256> port_bitset(ports_int);
  port_bitset[switch_metadata_ptr->router_bridge_port->bridge_port] = true;
  port_map = port_bitset.to_string();
  port_map.erase(0, std::min(port_map.find_first_not_of('0'), port_map.size()-1));
  (*logger)->info("updating mc_node {} with port_map {}", vlan->handle_mc_l1, port_map);
  bm_bridge_client_mc_ptr->bm_mc_node_update(cxt_id, vlan->handle_mc_l1, port_map, "");
}