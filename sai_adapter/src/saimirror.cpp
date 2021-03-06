#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_mirror_session(
        _Out_ sai_object_id_t *session_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::remove_mirror_session(
        _In_ sai_object_id_t session_id) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}
       
sai_status_t sai_adapter::set_mirror_session_attribute(
        _In_ sai_object_id_t session_id,
        _In_ const sai_attribute_t *attr) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t sai_adapter::get_mirror_session_attribute(
        _In_ sai_object_id_t session_id,
        _In_ uint32_t attr_count,
        _Inout_ sai_attribute_t *attr_list) {
        return SAI_STATUS_NOT_IMPLEMENTED;
}