#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_policer(
        _Out_ sai_object_id_t *policer_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_policer(
	_In_ sai_object_id_t policer_id) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_policer_attribute(
        _In_ sai_object_id_t policer_id,
        _In_ const sai_attribute_t *attr) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_policer_attribute(
        _In_ sai_object_id_t policer_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_policer_stats(
        _In_ sai_object_id_t policer_id,
        _In_ uint32_t number_of_counters,
        _In_ const sai_policer_stat_t *counter_ids,
        _Out_ uint64_t *counters) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::clear_policer_stats(
        _In_ sai_object_id_t policer_id,
        _In_ uint32_t number_of_counters,
        _In_ const sai_policer_stat_t *counter_ids) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
