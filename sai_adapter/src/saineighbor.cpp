#include "../inc/sai_adapter.h"

void get_match_params_from_neighbor_entry(const sai_neighbor_entry_t *neighbor_entry, BmMatchParams &match_params, Switch_metadata *switch_metadata_ptr) {
  RouterInterface_obj *rif = switch_metadata_ptr->rifs[neighbor_entry->rif_id];
  match_params.push_back(parse_exact_match_param(rif->rif_id, 1)); 
  if (neighbor_entry->ip_address.addr_family == SAI_IP_ADDR_FAMILY_IPV4) {
    match_params.push_back(parse_exact_match_param(htonl(neighbor_entry->ip_address.addr.ip4), 4));
  } else { // ipv6
    // match_params.push_back(parse_exact_match_param(neighbor_entry->ip_address.addr.ip6)); 
  }
}

sai_status_t sai_adapter::create_neighbor_entry (const sai_neighbor_entry_t *neighbor_entry,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_neighbor_entry");

  // parsing attributes
  sai_attribute_t attribute;
  sai_mac_t mac;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
    case SAI_NEIGHBOR_ENTRY_ATTR_DST_MAC_ADDRESS:
      memcpy(mac, attribute.value.mac, 6);
      break;
    default:
      (*logger)->error(
          "while parsing neighbor_entry, attribute.id = {}. was dumped in sai_obj",
          attribute.id);
      break;
    }  
  }

  // config tables 
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  get_match_params_from_neighbor_entry(neighbor_entry, match_params, switch_metadata_ptr);
  action_data.push_back(parse_param(parse_mac_64(mac), 6)); 
  bm_router_client_ptr->bm_mt_add_entry(cxt_id, "table_neighbor",
        match_params, "action_set_packet_dmac",
        action_data, options);

}

sai_status_t sai_adapter::remove_neighbor_entry(const sai_neighbor_entry_t *neighbor_entry) {
	(*logger)->info("remove_neighbor_entry");
  BmMtEntry bm_entry;
  BmAddEntryOptions options;
  BmMatchParams match_params;
  get_match_params_from_neighbor_entry(neighbor_entry, match_params, switch_metadata_ptr);
  bm_router_client_ptr->bm_mt_get_entry_from_key(bm_entry, cxt_id, "table_neighbor",
                                          match_params, options);
  bm_router_client_ptr->bm_mt_delete_entry(cxt_id, "table_neighbor",
                                    bm_entry.entry_handle);
  return SAI_STATUS_SUCCESS;
}
