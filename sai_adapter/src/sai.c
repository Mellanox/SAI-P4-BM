#include "../inc/sai_adapter_interface.h"
#include <sai.h>
#include <stdio.h>
#include <stdlib.h>
// static sai_api_service_t sai_api_service;
static S_O_Handle sai_adapter;
static sai_api_t api_id = SAI_API_UNSPECIFIED;
// switch_device_t device = 0;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static const char *module[] = {
    "UNSPECIFIED",
    "SWITCH",
    "PORT",
    "FDB",
    "VLAN",
    "VIRTUAL_ROUTER",
    "ROUTE",
    "NEXT_HOP",
    "NEXT_HOP_GROUP",
    "ROUTER_INTERFACE",
    "NEIGHBOR",
    "ACL",
    "HOST_INTERFACE",
    "MIRROR",
    "SAMPLEPACKET",
    "STP",
    "LAG",
    "POLICER",
    "WRED",
    "QOS_MAP",
    "QUEUE",
    "SCHEDULER",
    "SCHEDULER_GROUP",
    "BUFFERS",
    "HASH",
    "UDF",
    "IPMC",
    "L2MC",
};

sai_status_t sai_api_query(sai_api_t sai_api_id, void **api_method_table) {
  sai_status_t status = SAI_STATUS_SUCCESS;
  if (!api_method_table) {
    status = SAI_STATUS_INVALID_PARAMETER;
    return status;
  }

  status = sai_adapter_api_query(sai_adapter, sai_api_id, api_method_table);
  return status;
}

sai_status_t sai_api_initialize(uint64_t flags,
                                const service_method_table_t *services) {
  sai_adapter = create_sai_adapter();
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_api_uninitialize(void) { 
    free_sai_adapter(sai_adapter);
    return SAI_STATUS_SUCCESS;
}

sai_object_type_t sai_object_type_query(sai_object_id_t sai_object_id) {
    return sai_adapter_object_type_query(sai_adapter, sai_object_id);
}

sai_status_t sai_log_set(sai_api_t sai_api_id, sai_log_level_t log_level) { 
    return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_object_id_t sai_switch_id_query(sai_object_id_t sai_object_id) {
        return 0; //TODO add switch id
}

sai_status_t sai_dbg_generate_dump(const char *dump_file_name) {
    return SAI_STATUS_NOT_IMPLEMENTED;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */