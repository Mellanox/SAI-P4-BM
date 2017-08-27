#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_l2mc_entry(
        _In_ const sai_l2mc_entry_t *l2mc_entry,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_l2mc_entry(
        _In_ const sai_l2mc_entry_t *l2mc_entry) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_l2mc_entry_attribute(
        _In_ const sai_l2mc_entry_t *l2mc_entry,
        _In_ const sai_attribute_t *attr) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_l2mc_entry_attribute(
        _In_ const sai_l2mc_entry_t *l2mc_entry,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}