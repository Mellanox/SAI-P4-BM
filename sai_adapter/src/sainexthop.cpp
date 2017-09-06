#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_next_hop (sai_object_id_t *next_hop_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_next_hop");
  uint32_t nhop_id = switch_metadata_ptr->GetNewNextHopID();
  NextHop_obj *nhop = new NextHop_obj(sai_id_map_ptr);
  switch_metadata_ptr->nhops[nhop->sai_object_id] = nhop;
  nhop->nhop_id = nhop_id;
  
  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_NEXT_HOP_ATTR_IP:
      nhop->ip = attribute.value.ipaddr;
      break;
    case SAI_NEXT_HOP_ATTR_ROUTER_INTERFACE_ID:
      nhop->rif = switch_metadata_ptr->rifs[attribute.value.oid];
      break;
    case SAI_NEXT_HOP_ATTR_TYPE:
      nhop->type = (sai_next_hop_type_t) attribute.value.s32;
      break;
    default:
      (*logger)->error(
          "while parsing next_hop, attribute.id = {} was dumped in sai_obj",
          attribute.id);
      break;
    }
  }

  // config tables
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  match_params.push_back(parse_exact_match_param(nhop->nhop_id, 1));
  if (nhop->ip.addr_family == SAI_IP_ADDR_FAMILY_IPV4) {
    action_data.push_back(parse_param(htonl(nhop->ip.addr.ip4), 4));
  }
  action_data.push_back(parse_param(nhop->rif->rif_id, 1));
  nhop->hanlde_table_nhop = bm_router_client_ptr->bm_mt_add_entry(cxt_id, "table_next_hop",
        match_params, "action_set_erif_set_nh_dstip",
        action_data, options);

  *next_hop_id = nhop->sai_object_id;
  (*logger)->info("object_id {}", nhop->sai_object_id);
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_next_hop(sai_object_id_t next_hop_id) {
	(*logger)->info("remove_next_hop");
  NextHop_obj *nhop = switch_metadata_ptr->nhops[next_hop_id];
  if (nhop->hanlde_table_nhop != NULL_HANDLE) {
    bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_next_hop", nhop->hanlde_table_nhop);
  }
  switch_metadata_ptr->nhops.erase(next_hop_id);
  sai_id_map_ptr->free_id(next_hop_id);
  return SAI_STATUS_SUCCESS;
}