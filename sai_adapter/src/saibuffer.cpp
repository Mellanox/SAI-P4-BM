#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_buffer_pool(
        _Out_ sai_object_id_t *pool_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_buffer_pool(
	_In_ sai_object_id_t pool_id) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_buffer_pool_attribute(
        _In_ sai_object_id_t pool_id,
        _In_ const sai_attribute_t *attr) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_buffer_pool_attribute(
        _In_ sai_object_id_t pool_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_buffer_pool_stats(
        _In_ sai_object_id_t pool_id,
        _In_ uint32_t number_of_counters,
        _In_ const sai_buffer_pool_stat_t *counter_ids,
        _Out_ uint64_t *counters) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::clear_buffer_pool_stats(
        _In_ sai_object_id_t pool_id,
        _In_ uint32_t number_of_counters,
        _In_ const sai_buffer_pool_stat_t *counter_ids) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}




sai_status_t sai_adapter::create_ingress_priority_group(
        _Out_ sai_object_id_t *ingress_pg_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_ingress_priority_group(
        _In_ sai_object_id_t ingress_pg_id) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_ingress_priority_group_attribute(
        _In_ sai_object_id_t ingress_pg_id,
        _In_ const sai_attribute_t *attr) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_ingress_priority_group_attribute(
        _In_ sai_object_id_t ingress_pg_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_ingress_priority_group_stats(
        _In_ sai_object_id_t ingress_pg_id,
        _In_ uint32_t number_of_counters,
        _In_ const sai_ingress_priority_group_stat_t *counter_ids,
        _Out_ uint64_t *counters) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::clear_ingress_priority_group_stats(
        _In_ sai_object_id_t ingress_pg_id,
        _In_ uint32_t number_of_counters,
        _In_ const sai_ingress_priority_group_stat_t *counter_ids) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}


sai_status_t sai_adapter::create_buffer_profile(
        _Out_ sai_object_id_t *buffer_profile_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_buffer_profile(
        _In_ sai_object_id_t buffer_profile_id) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_buffer_profile_attribute(
        _In_ sai_object_id_t buffer_profile_id,
        _In_ const sai_attribute_t *attr) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_buffer_profile_attribute(
        _In_ sai_object_id_t buffer_profile_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}