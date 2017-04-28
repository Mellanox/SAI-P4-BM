#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_virtual_router (sai_object_id_t *vr_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list) {
  (*logger)->info("create_virtual_router");
  VirtualRouter_obj * vr = new VirtualRouter_obj(sai_id_map_ptr);
  switch_metadata_ptr->vrs[vr->sai_object_id] = vr;
  BmAddEntryOptions options;
  BmMatchParams match_params;
  BmActionData action_data;

  // Parse atrributes
  sai_attribute_t attribute;
  for (uint32_t i = 0; i < attr_count; i++) {
    attribute = attr_list[i];
    switch (attribute.id) {
      case SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V6_STATE:
        vr->v6_state = attribute.value.booldata;
        break;
      case SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V4_STATE:
        vr->v4_state = attribute.value.booldata;
        break;
      default:
        (*logger)->error(
            "while parsing lag member, attribute.id = was dumped in sai_obj",
            attribute.id);
        break;
      }

    // Config Tables
  // match_params.push_back(parse_exact_match_param(1,1));
  // action_data.push_back(parse_param(2,1));
  // bm_router_client_ptr->bm_mt_add_entry(
  //     cxt_id, "table_ingress_vrf", match_params,
  //     "action_set_vrf", action_data, options);


    *vr_id = vr->sai_object_id;
    return SAI_STATUS_SUCCESS;
  }
}


sai_status_t sai_adapter::remove_virtual_router(sai_object_id_t vr_id) {
	(*logger)->info("remove_virtual_router");
  switch_metadata_ptr->vrs.erase(vr_id);
  sai_id_map_ptr->free_id(vr_id);
  return SAI_STATUS_SUCCESS;
}