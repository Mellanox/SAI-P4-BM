#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_neighbor_entry (const sai_neighbor_entry_t *neighbor_entry,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_neighbor_entry");
  // Lag_member_obj *lag_member = new Lag_member_obj(sai_id_map_ptr);
  // switch_metadata_ptr->lag_members[lag_member->sai_object_id] = lag_member;
  // Port_obj *port;
  // Lag_obj *lag;
  // // parsing attributes
  // sai_attribute_t attribute;
  // for (uint32_t i = 0; i < attr_count; i++) {
  //   attribute = attr_list[i];
  //   switch (attribute.id) {
  //   case SAI_LAG_MEMBER_ATTR_PORT_ID:
  //     port = switch_metadata_ptr->ports[attribute.value.oid];
  //     lag_member->port = port;
  //     break;
  //   case SAI_LAG_MEMBER_ATTR_LAG_ID:
  //     lag = switch_metadata_ptr->lags[attribute.value.oid];
  //     lag_member->lag = lag;
  //     lag->lag_members.push_back(lag_member->sai_object_id);
  //     break;
  //   default:
  //     (*logger)->error(
  //         "while parsing lag member, attribute.id = was dumped in sai_obj",
  //         attribute.id);
  //     break;
  //   }
  // }
}

sai_status_t sai_adapter::remove_neighbor_entry(const sai_neighbor_entry_t *neighbor_entry) {
	(*logger)->info("remove_neighbor_entry");
}