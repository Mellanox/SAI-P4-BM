#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_router_interface (sai_object_id_t *router_interface_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_router_interface");
  RouterInterface_obj * rif = new RouterInterface_obj(sai_id_map_ptr);
  switch_metadata_ptr->rifs[rif->sai_object_id] = rif;
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;

  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_ROUTER_INTERFACE_ATTR_TYPE:
      rif->type = (sai_router_interface_type_t) attribute.value.s32;
      break;
    case SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID:
      break;
    case SAI_ROUTER_INTERFACE_ATTR_VLAN_ID:
      rif->vid = attribute.value.u16;
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
          "while parsing lag member, attribute.id = was dumped in sai_obj",
          attribute.id);
      break;
    }  
  }

  // config tables
  uint32_t bridge_id;
  uint64_t mac_address_64;
  if (rif->mac_valid) {
    mac_address_64 = parse_mac_64(rif->mac);
  } else {
    mac_address_64 = parse_mac_64(switch_metadata_ptr->default_switch_mac);
  }

  if (rif->type == SAI_ROUTER_INTERFACE_TYPE_VLAN) {  //Vlan interface
    Vlan_obj *vlan = switch_metadata_ptr->vlans[switch_metadata_ptr->GetVlanObjIdFromVid(rif->vid)];
    bridge_id = vlan->bridge_id;
    match_params.push_back(parse_exact_match_param(mac_address_64, 6));
    match_params.push_back(parse_exact_match_param(bridge_id, 2));
    action_data.push_back(parse_param(switch_metadata_ptr->router_bridge_port->bridge_port, 1));
    rif->handle_l3_interface = bm_bridge_client_ptr->bm_mt_add_entry(
              cxt_id, "table_l3_interface", match_params, "action_set_egress_br_port",
              action_data, options);
  }

  *router_interface_id = rif->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_router_interface (sai_object_id_t router_interface_id) {
  (*logger)->info("remove_router_interface");
  RouterInterface_obj *rif = switch_metadata_ptr->rifs[router_interface_id];
  if (rif->handle_l3_interface != NULL_HANDLE) {
    bm_bridge_client_ptr->bm_mt_delete_entry(cxt_id, "table_l3_interface", rif->handle_l3_interface);
  }
  switch_metadata_ptr->rifs.erase(router_interface_id);
  sai_id_map_ptr->free_id(router_interface_id);
  return SAI_STATUS_SUCCESS;
}