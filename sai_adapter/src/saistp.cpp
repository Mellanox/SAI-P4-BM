#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_stp(
        _Out_ sai_object_id_t *stp_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_stp(
	_In_ sai_object_id_t stp_id) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_stp_attribute(
        _In_ sai_object_id_t stp_id,
        _In_ const sai_attribute_t *attr) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_stp_attribute(
        _In_ sai_object_id_t stp_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::create_stp_port(
        _Out_ sai_object_id_t *stp_port_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_stp_port(
        _In_ sai_object_id_t stp_port_id) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_stp_port_attribute(
        _In_ sai_object_id_t stp_port_id,
        _In_ const sai_attribute_t *attr) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_stp_port_attribute(
        _In_ sai_object_id_t stp_port_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::create_stp_ports(
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t object_count,
        _In_ const uint32_t *attr_count,
        _In_ const sai_attribute_t **attrs,
        _In_ sai_bulk_op_type_t type,
        _Out_ sai_object_id_t *object_id,
        _Out_ sai_status_t *object_statuses) {

        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::remove_stp_ports(
        _In_ uint32_t object_count,
        _In_ const sai_object_id_t *object_id,
        _In_ sai_bulk_op_type_t type,
        _Out_ sai_status_t *object_statuses) {

        return SAI_STATUS_NOT_IMPLEMENTED;
}