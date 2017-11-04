#ifdef __cplusplus
#include <spdlog/spdlog.h>
extern "C" {
#endif
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <sai.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/select.h>

// sai functions
const char *test_profile_get_value(_In_ sai_switch_profile_id_t profile_id,
                                   _In_ const char *variable) {
  return NULL;
}
int test_profile_get_next_value(_In_ sai_switch_profile_id_t profile_id,
                                _Out_ const char **variable,
                                _Out_ const char **value) {
  return -1;
}
const service_method_table_t test_services = {test_profile_get_value,
                                              test_profile_get_next_value};
sai_status_t sai_api_initialize(_In_ uint64_t flags,
                                _In_ const service_method_table_t *services);
sai_status_t sai_api_query(_In_ sai_api_t sai_api_id,
                           _Out_ void **api_method_table);
sai_status_t sai_api_uninitialize(void);
#ifdef __cplusplus
}
#endif
/* Enumerate all the K/V pairs in a profile.
Pointer to NULL passed as variable restarts enumeration.
Function returns 0 if next value exists, -1 at the end of the list. */

// sai lag creation functions
void port_state_change(uint32_t count, sai_port_oper_status_notification_t *data) {
  for (int i=0; i<count; i++) {
    printf("port %d has changed status to %s\n", data[i].port_id, (data[i].port_state == SAI_PORT_OPER_STATUS_UP) ? "UP" : "DOWN");
  }
}

int main(int argc, char **argv) {
  

  // sai init ports
  sai_switch_api_t *switch_api;
  sai_port_api_t *port_api;
  sai_bridge_api_t *bridge_api;
  sai_api_initialize(0, &test_services);
  sai_api_query(SAI_API_SWITCH, (void **)&switch_api);
  sai_api_query(SAI_API_PORT, (void **)&port_api);
  sai_api_query(SAI_API_BRIDGE, (void **)&bridge_api);
  
  sai_object_id_t switch_id;
  sai_attribute_t switch_attr;
  switch_attr.id = SAI_SWITCH_ATTR_INIT_SWITCH;
  switch_attr.value.booldata = true;
  switch_api->create_switch(&switch_id, 1, &switch_attr);

  sai_attribute_t sai_attr;
  sai_attr.id = SAI_SWITCH_ATTR_DEFAULT_VIRTUAL_ROUTER_ID;
  switch_api->get_switch_attribute(switch_id, 1, &sai_attr);
  printf("Default VR id: %d\n", sai_attr.value.oid);

  sai_attr.id = SAI_BRIDGE_PORT_ATTR_PORT_ID;
  bridge_api->get_bridge_port_attribute(8, 1, &sai_attr);
  printf("bridge port 8 port id: %d\n", sai_attr.value.oid);

  // sai_attr.id = SAI_SWITCH_ATTR_PORT_STATE_CHANGE_NOTIFY;
  // sai_attr.value.ptr = port_state_change;
  // switch_api->set_switch_attribute(switch_id, &sai_attr);

  // sai_object_id_t ports[32];
  // sai_attr.id = SAI_SWITCH_ATTR_PORT_LIST;
  // sai_attr.value.objlist.list = ports;
  // sai_attr.value.objlist.count = 32;
  // switch_api->get_switch_attribute(switch_id, 1, &sai_attr);
  
  // sai_attr.id = SAI_PORT_ATTR_ADMIN_STATE;
  // sai_attr.value.booldata = false;
  // port_api->set_port_attribute(ports[5], &sai_attr);
  // sleep(10);
  sai_api_uninitialize();
  return 0;
}
