#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_queue(
        _Out_ sai_object_id_t *queue_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_queue(
	_In_ sai_object_id_t queue_id) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_queue_attribute(
        _In_ sai_object_id_t queue_id,
        _In_ const sai_attribute_t *attr) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_queue_attribute(
        _In_ sai_object_id_t queue_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_queue_stats(
        _In_ sai_object_id_t queue_id,
        _In_ const sai_queue_stat_t *counter_ids,
        _In_ uint32_t number_of_counters,
        _Out_ uint64_t *counters) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::clear_queue_stats(
        _In_ sai_object_id_t queue_id,
        _In_ const sai_queue_stat_t *counter_ids,
        _In_ uint32_t number_of_counters) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
