#include "../inc/sai_adapter.h"

uint32_t get_prefix_length_from_mask(sai_ip4_t mask) {
  return 24; // TODO !!
}

sai_status_t sai_adapter::create_route_entry(const sai_route_entry_t *route_entry,
        uint32_t attr_count,
        const sai_attribute_t *attr_list) {
  (*logger)->info("create_route_entry");
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  NextHop_obj *nhop;

  // parsing attributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_ROUTE_ENTRY_ATTR_NEXT_HOP_ID:
      nhop = switch_metadata_ptr->nhops[attribute.value.oid];
      break;
    default:
      (*logger)->error(
          "while parsing lag member, attribute.id = was dumped in sai_obj",
          attribute.id);
      break;
    }
  }

  // config tables
  sai_ip_prefix_t dst_ip = router_entry->destination;
  uint32_t ipv4;
  uint32_t vrf = switch_metadata_ptr->vrs[router_entry->vr_id]->vrf;
  uint32_t prefix_length;
  uint64_t l3_key = (vrf << 32);
  if (dst_ip.addr_family == SAI_IP_ADDR_FAMILY_IPV4) {
    ipv4 = dst_ip.addr;
    prefix_length = get_prefix_length_from_mask(dst_ip.mask); //TODO!!
    l3_key += ipv4;
  }
  // uint32_t dst_ip = ;
  match_params.push_back(parse_lpm_param(l3_key, 5, prefix_length));
  action_data.push_back(parse_param(nhop->nhop_id, 1));
  bm_router_client_ptr->bm_mt_add_entry(cxt_id, "table_router",
        match_params, "action_set_nhop_id",
        action_data, options);

}

sai_status_t sai_adapter::remove_route_entry(const sai_route_entry_t *route_entry) {
	(*logger)->info("remove_route_entry");
}