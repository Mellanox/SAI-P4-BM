#include "../inc/sai_adapter.h"

uint32_t get_prefix_length_from_mask(sai_ip4_t mask) {
  uint8_t *byte = (uint8_t*) &mask;
  uint32_t prefix_length = 0;
  for (int i=0; i<4; ++i) {
    switch (byte[i]) {
      case 255:
        prefix_length+=8;
        break;
      case 254:
        prefix_length+=7;
        i=4;
        break;
      case 252:
        prefix_length+=6;
        i=4;
        break;
      case 248:
        prefix_length+=5;
        i=4;
        break;
      case 240:
        prefix_length+=4;
        i=4;
        break;
      case 224:
        prefix_length+=3;
        i=4;
        break;
      case 192:
        prefix_length+=2;
        i=4;
        break;
      case 128:
        prefix_length+=1;
        i=4;
        break;
    }
  }
  return prefix_length;
}

BmMatchParams get_match_param_from_route_entry(const sai_route_entry_t *route_entry, Switch_metadata *switch_metadata_ptr) {
  BmMatchParams match_params;
  sai_ip_prefix_t dst_ip = route_entry->destination;
  uint32_t ipv4;
  uint32_t vrf = switch_metadata_ptr->vrs[route_entry->vr_id]->vrf;
  uint32_t prefix_length;
  match_params.push_back(parse_exact_match_param(vrf, 1));
  if (dst_ip.addr_family == SAI_IP_ADDR_FAMILY_IPV4) {
    ipv4 = dst_ip.addr.ip4;
    prefix_length = get_prefix_length_from_mask(dst_ip.mask.ip4);
    match_params.push_back(parse_lpm_param(htonl(ipv4), 4, prefix_length));
  }
  return match_params;
}

sai_status_t sai_adapter::create_route_entry(const sai_route_entry_t *route_entry,
        uint32_t attr_count,
        const sai_attribute_t *attr_list) {
  (*logger)->info("create_route_entry");
  if (route_entry->destination.addr_family == SAI_IP_ADDR_FAMILY_IPV6) {
    (*logger)->error("IPv6 is not yet supported");
    return SAI_STATUS_SUCCESS;
  }
  NextHop_obj *nhop;
  RouterInterface_obj *rif;
  sai_object_type_t nhop_obj_type;
  sai_packet_action_t action = SAI_PACKET_ACTION_FORWARD;
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
      case SAI_ROUTE_ENTRY_ATTR_NEXT_HOP_ID:
        nhop_obj_type = _sai_object_type_query(attribute.value.oid);
        switch (nhop_obj_type) {
          case SAI_OBJECT_TYPE_NEXT_HOP:
            nhop = switch_metadata_ptr->nhops[attribute.value.oid];
            break;
          // case SAI_OBJECT_TYPE_NEXT_HOP_GROUP
          case SAI_OBJECT_TYPE_ROUTER_INTERFACE:
            rif = switch_metadata_ptr->rifs[attribute.value.oid];
            break;
          case SAI_OBJECT_TYPE_PORT:
            if (attribute.value.oid != switch_metadata_ptr->cpu_port_id) {
              (*logger)->error("adding route with port object id ({}) which is not SAI_SWITCH_ATTR_CPU_PORT ({})", attribute.value.oid, switch_metadata_ptr->cpu_port_id);
              return SAI_STATUS_INVALID_OBJECT_ID;
            } else {
              (*logger)->info("added CPU_PORT route (IP2ME)");
            }
            break;
        }
        break;
      case SAI_ROUTE_ENTRY_ATTR_PACKET_ACTION:
        action = (sai_packet_action_t) attribute.value.s32;
        break;
      default:
        (*logger)->error(
            "while parsing route entry, attribute.id = {} was dumped in sai_obj",
            attribute.id);
        break;
    }
  }

  // config tables
  BmAddEntryOptions options;
  BmActionData action_data;
  BmMatchParams match_params = get_match_param_from_route_entry(route_entry, switch_metadata_ptr);
  if (action == SAI_PACKET_ACTION_FORWARD) {
    switch (nhop_obj_type) {
      case SAI_OBJECT_TYPE_NEXT_HOP:
        action_data.push_back(parse_param(nhop->nhop_id, 1));
        bm_router_client_ptr->bm_mt_add_entry(
            cxt_id, "table_router", match_params, "action_set_nhop_id",
            action_data, options);
        break;
      // case SAI_OBJECT_TYPE_NEXT_HOP_GROUP
      case SAI_OBJECT_TYPE_ROUTER_INTERFACE:
        action_data.push_back(parse_param(rif->rif_id, 1));
        bm_router_client_ptr->bm_mt_add_entry(
            cxt_id, "table_router", match_params, "action_set_erif_set_nh_dstip_from_pkt",
            action_data, options);
        break;
      case SAI_OBJECT_TYPE_PORT:
        bm_router_client_ptr->bm_mt_add_entry(
            cxt_id, "table_router", match_params, "action_set_ip2me",
            action_data, options);
        break;
    }
    return SAI_STATUS_SUCCESS;
  }
  if (action == SAI_PACKET_ACTION_DROP) {
    bm_router_client_ptr->bm_mt_add_entry(
            cxt_id, "table_router", match_params, "drop",
            action_data, options);
    return SAI_STATUS_SUCCESS;
  }
  (*logger)->error("requested action type for route entry is not supported");
  return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::remove_route_entry(const sai_route_entry_t *route_entry) {
  (*logger)->info("remove_route_entry");
  BmMtEntry bm_entry;
  BmAddEntryOptions options;
  BmMatchParams match_params = get_match_param_from_route_entry(route_entry, switch_metadata_ptr);
  bm_router_client_ptr->bm_mt_get_entry_from_key(bm_entry, cxt_id, "table_router",
                                          match_params, options);
  (*logger)->info("trying to remove table_router entry handle {}", bm_entry.entry_handle);
  bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_router",
                                    bm_entry.entry_handle);
  return SAI_STATUS_SUCCESS;
}