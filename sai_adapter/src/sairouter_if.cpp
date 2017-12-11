#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_router_interface (sai_object_id_t *router_interface_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_router_interface");
  uint32_t rif_id = switch_metadata_ptr->GetNewRifId();
  RouterInterface_obj * rif = new RouterInterface_obj(sai_id_map_ptr);
  switch_metadata_ptr->rifs[rif->sai_object_id] = rif;
  rif->rif_id = rif_id;
  Vlan_obj *vlan;
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_ROUTER_INTERFACE_ATTR_TYPE:
      rif->type = (sai_router_interface_type_t) attribute.value.s32;
      break;
    case SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID:
      rif->vr = switch_metadata_ptr->vrs[attribute.value.oid];
      break;
    case SAI_ROUTER_INTERFACE_ATTR_VLAN_ID:
      vlan = switch_metadata_ptr->vlans[attribute.value.oid];
      rif->vid = vlan->vid;
      break;
    case SAI_ROUTER_INTERFACE_ATTR_PORT_ID:
      rif->port_id = attribute.value.oid;
      break;
    case SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS:
      memcpy(rif->mac,attribute.value.mac, 6);
      rif->mac_valid = true;
      break;
    case SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE:
      break;
    case SAI_ROUTER_INTERFACE_ATTR_ADMIN_V6_STATE:
      break;
    default:
      (*logger)->error(
          "while parsing router interface, attribute.id = {} was dumped in sai_obj",
          attribute.id);
      break;
    }  
  }

  // config tables
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;

  match_params.push_back(parse_exact_match_param(rif->rif_id, 1));
  action_data.push_back(parse_param(rif->vr->vrf,1));
  rif->handle_ingress_vrf = bm_client_ptr->bm_mt_add_entry(
                cxt_id, "table_ingress_vrf", match_params, "action_set_vrf",
                action_data, options);
  action_data.clear();
  match_params.clear();
  uint32_t bridge_id;
  uint64_t mac_address_64;
  if (rif->mac_valid) {
    mac_address_64 = parse_mac_64(rif->mac);
  } else {
    mac_address_64 = parse_mac_64(switch_metadata_ptr->default_switch_mac);
  }

  switch(rif->type) {
    case SAI_ROUTER_INTERFACE_TYPE_VLAN:  //Vlan interface
      match_params.push_back(parse_exact_match_param(vlan->vid, 2)); 
      action_data.push_back(parse_param(rif->rif_id, 1));
      rif->handle_ingress_l3 = bm_client_ptr->bm_mt_add_entry(
                cxt_id, "table_ingress_l3_vlan_if", match_params, "action_set_irif",
                action_data, options);

      bridge_id = vlan->bridge_id;
      match_params.clear();
      action_data.clear();
      match_params.push_back(parse_exact_match_param(mac_address_64, 6));
      match_params.push_back(parse_exact_match_param(bridge_id, 2));

      rif->handle_learn_mac = bm_client_ptr->bm_mt_add_entry(
            cxt_id, "table_learn_fdb", match_params, "nop", action_data,
            options);

      action_data.push_back(parse_param(switch_metadata_ptr->router_bridge_port->bridge_port, 1));
      rif->handle_l3_interface = bm_client_ptr->bm_mt_add_entry(
                cxt_id, "table_l3_interface", match_params, "action_set_egress_br_port",
                action_data, options);


      match_params.clear();
      action_data.clear();
      match_params.push_back(parse_exact_match_param(rif->rif_id, 1));
      action_data.push_back(parse_param(mac_address_64, 6));
      action_data.push_back(parse_param(vlan->vid, 2));
      action_data.push_back(parse_param(switch_metadata_ptr->router_bridge_port->bridge_port, 1));
      rif->handle_egress_l3 = bm_client_ptr->bm_mt_add_entry(
                cxt_id, "table_egress_L3_if", match_params, "action_set_smac_vid",
                action_data, options);
      break;

  case SAI_ROUTER_INTERFACE_TYPE_PORT: { //Port interface
      int l2_if_type = 1; //Router port
      sai_object_type_t port_object_type = sai_object_type_query(rif->port_id);
      int l2_if;
      int out_if_type;
      if (port_object_type == SAI_OBJECT_TYPE_PORT) {
        l2_if = switch_metadata_ptr->ports[rif->port_id]->l2_if;
        out_if_type = 0;
        (*logger)->info("router interface of type port added with PORT oid {} , l2_if {}", rif->port_id, l2_if);
      } else {
        l2_if = switch_metadata_ptr->lags[rif->port_id]->l2_if;
        out_if_type =1 ;
        (*logger)->info("router interface of type port added with LAG oid {} , l2_if {}", rif->port_id, l2_if);
      }

      sai_object_id_t bridge_port = SAI_NULL_OBJECT_ID;
      for (bridge_port_id_map_t::iterator it = switch_metadata_ptr->bridge_ports.begin(); it != switch_metadata_ptr->bridge_ports.end(); ++it) {
        if (it->second->port_id == rif->port_id) {
          remove_bridge_port(it->first);
        }
      }

      match_params.push_back(parse_exact_match_param(l2_if, 1));
      action_data.push_back(parse_param(rif->rif_id, 1));
      rif->handle_port_ingress_if_type = bm_client_ptr->bm_mt_add_entry(
                cxt_id, "table_port_ingress_interface_type", match_params, "action_set_l2_if_type_rif",
                action_data, options);

      match_params.clear();
      action_data.clear();
      match_params.push_back(parse_exact_match_param(rif->rif_id, 1));
      action_data.push_back(parse_param(mac_address_64, 6));
      action_data.push_back(parse_param(l2_if, 1));
      action_data.push_back(parse_param(out_if_type, 1));
      rif->handle_egress_l3 = bm_client_ptr->bm_mt_add_entry(
                cxt_id, "table_egress_L3_if", match_params, "action_set_smac_out_if",
                action_data, options);
    }
    break;
  default:
      (*logger)->error("Unsupported rif type {}", rif->type);
      break;
  }

  *router_interface_id = rif->sai_object_id;
  (*logger)->info("object_id {}. rif_id {}", rif->sai_object_id, rif->rif_id);
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_router_interface (sai_object_id_t router_interface_id) {
  (*logger)->info("remove_router_interface");
  RouterInterface_obj *rif = switch_metadata_ptr->rifs[router_interface_id];
  if (rif->handle_port_ingress_if_type != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_port_ingress_interface_type", rif->handle_port_ingress_if_type);
  }
  if (rif->handle_l3_interface != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_l3_interface", rif->handle_l3_interface);
  }
  if (rif->handle_learn_mac != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_learn_fdb", rif->handle_learn_mac);
  }
  if (rif->handle_egress_l3 != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_egress_L3_if", rif->handle_egress_l3);
  }
  if (rif->handle_ingress_l3 != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_ingress_l3_vlan_if", rif->handle_ingress_l3);
  }
  if (rif->handle_ingress_vrf != NULL_HANDLE) {
    bm_client_ptr->bm_mt_delete_entry(cxt_id, "table_ingress_vrf", rif->handle_ingress_vrf);
  }
  if (rif->type == SAI_ROUTER_INTERFACE_TYPE_PORT) {
    // create_bridge_port(); TODO
  }
  switch_metadata_ptr->rifs.erase(router_interface_id);
  sai_id_map_ptr->free_id(router_interface_id);
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::set_router_interface_attribute(sai_object_id_t rif_id, const sai_attribute_t *attr) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
sai_status_t sai_adapter::get_router_interface_attribute(sai_object_id_t rif_id, uint32_t attr_count, sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
