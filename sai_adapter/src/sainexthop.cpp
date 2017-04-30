#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_next_hop (sai_object_id_t *next_hop_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_next_hop");
  NextHop_obj *nhop = new NextHop_obj(sai_id_map_ptr);
  switch_metadata_ptr->nhops[nhop->sai_object_id] = nhop;
  nhop->nhop_id = switch_metadata_ptr->GetNewNextHopID();
  
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
          "while parsing lag member, attribute.id = was dumped in sai_obj",
          attribute.id);
      break;
    }
  }

  // config tables
  match_params.push_back(parse_exact_match_param(nhop->nhop_id, 1));
  action_data.push_back(parse_param(nhop->rif->rif_id, 1));
  bm_router_client_ptr->bm_mt_add_entry(cxt_id, "table_next_hop",
        match_params, "action_set_erif_set_nh_dstip_from_pkt",
        action_data, options);
  x

  *next_hop_id = nhop->sai_object_id;
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::remove_next_hop(sai_object_id_t next_hop_id) {
	(*logger)->info("remove_next_hop");
  switch_metadata_ptr->nhops.erase(next_hop_id);
  sai_id_map_ptr->free_id(next_hop_id);
  return SAI_STATUS_SUCCESS;
}