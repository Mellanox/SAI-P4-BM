#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_neighbor_entry (const sai_neighbor_entry_t *neighbor_entry,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_neighbor_entry");
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;

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
          "while parsing lag member, attribute.id = was dumped in sai_obj",
          attribute.id);
      break;
    }  
  }

  // config tables
}

sai_status_t sai_adapter::remove_neighbor_entry(const sai_neighbor_entry_t *neighbor_entry) {
	(*logger)->info("remove_neighbor_entry");
  return SAI_STATUS_SUCCESS;
}
