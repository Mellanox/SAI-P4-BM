#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_samplepacket(
        _Out_ sai_object_id_t *samplepacket_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_samplepacket(
	_In_ sai_object_id_t samplepacket_id) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_samplepacket_attribute(
        _In_ sai_object_id_t samplepacket_id,
        _In_ const sai_attribute_t *attr) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_samplepacket_attribute(
        _In_ sai_object_id_t samplepacket_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
	return SAI_STATUS_NOT_IMPLEMENTED;
}