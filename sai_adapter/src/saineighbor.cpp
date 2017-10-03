#include "../inc/sai_adapter.h"

void get_match_params_from_neighbor_entry(const sai_neighbor_entry_t *neighbor_entry, BmMatchParams &match_params, Switch_metadata *switch_metadata_ptr) {
  spdlog::get("logger")->info("neighbor_entry get match_params. rif object id: {}", neighbor_entry->rif_id);
  RouterInterface_obj *rif = switch_metadata_ptr->rifs[neighbor_entry->rif_id];
  match_params.push_back(parse_exact_match_param(rif->rif_id, 1)); 
  if (neighbor_entry->ip_address.addr_family == SAI_IP_ADDR_FAMILY_IPV4) {
    match_params.push_back(parse_exact_match_param(htonl(neighbor_entry->ip_address.addr.ip4), 4));
  } else { // ipv6
    spdlog::get("logger")->error("unsupported ipv6 neighbor entry");
  }
}

sai_status_t sai_adapter::create_neighbor_entry (const sai_neighbor_entry_t *neighbor_entry,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_neighbor_entry");

  // parsing attributes
  sai_mac_t mac;
  for (uint32_t i = 0; i < attr_count; i++) {
    (*logger)->info("attribute id {}", attr_list[i].id);
    switch (attr_list[i].id) {
    case SAI_NEIGHBOR_ENTRY_ATTR_DST_MAC_ADDRESS:
      //memcpy(mac, attr_list[i].value.mac, 6);
      mac[0] = attr_list[i].value.mac[5];
      mac[1] = attr_list[i].value.mac[4];
      mac[2] = attr_list[i].value.mac[3];
      mac[3] = attr_list[i].value.mac[2];
      mac[4] = attr_list[i].value.mac[1];
      mac[5] = attr_list[i].value.mac[0];
      break;
    default:
      (*logger)->error(
          "while parsing neighbor_entry, attribute id = {}. was dumped in sai_obj",
          attr_list[i].id);
      break;
    }  
  }

  // config tables 
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;
  (*logger)->info("neighbor enrty: rif id {}, ip family {}, ip {}", neighbor_entry->rif_id, neighbor_entry->ip_address.addr_family, neighbor_entry->ip_address.addr.ip4);
  get_match_params_from_neighbor_entry(neighbor_entry, match_params, switch_metadata_ptr);
  action_data.push_back(parse_param(parse_mac_64(mac), 6)); 
  bm_router_client_ptr->bm_mt_add_entry(cxt_id, "table_neighbor",
        match_params, "action_set_packet_dmac",
        action_data, options);
  return SAI_STATUS_SUCCESS;

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
