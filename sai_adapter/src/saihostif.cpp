#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_hostif(sai_object_id_t *hif_id,
                                    sai_object_id_t switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list) {
	(*logger)->info("create_hostif");
}
sai_status_t sai_adapter::remove_hostif(sai_object_id_t hif_id) {
	(*logger)->info("remove_hostif");
}
sai_status_t sai_adapter::create_hostif_table_entry(
      sai_object_id_t *hif_table_entry, sai_object_id_t switch_id,
      uint32_t attr_count, const sai_attribute_t *attr_list) {
	(*logger)->info("create_hostif_table_entry");
}
sai_status_t sai_adapter::remove_hostif_table_entry(
      sai_object_id_t hif_table_entry) {
	(*logger)->info("remove_hostif_table_entry");
}
sai_status_t sai_adapter::create_hostif_trap_group(
      sai_object_id_t *hostif_trap_group_id, sai_object_id_t switch_id,
      uint32_t attr_count, const sai_attribute_t *attr_list) {
	(*logger)->info("create_hostif_trap_group");
}
sai_status_t sai_adapter::remove_hostif_trap_group(
      sai_object_id_t hostif_trap_group_id) {
	(*logger)->info("remove_hostif_trap_group");
}
sai_status_t sai_adapter::create_hostif_trap(sai_object_id_t *hostif_trap_id,
                                         sai_object_id_t switch_id,
                                         uint32_t attr_count,
                                         const sai_attribute_t *attr_list) {
	(*logger)->info("create_hostif_trap");
}
sai_status_t sai_adapter::remove_hostif_trap(sai_object_id_t hostif_trap_id) {
	(*logger)->info("remove_hostif_trap");
}