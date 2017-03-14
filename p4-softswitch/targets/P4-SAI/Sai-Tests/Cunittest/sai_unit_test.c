#include <sai.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    UNREFERENCED_PARAMETER(profile_id);

    if (!strcmp(variable, SAI_KEY_INIT_CONFIG_FILE)) {
        return "/usr/share/sai_2410.xml";
    }
    else if (!strcmp(variable, KV_DEVICE_MAC_ADDRESS)) {
        return "20:03:04:05:06:00";
    }
    else if (!strcmp(variable, SAI_KEY_L3_ROUTE_TABLE_SIZE)) {
        //return "1000";
    }
    else if (!strcmp(variable, SAI_KEY_L3_NEIGHBOR_TABLE_SIZE)) {
        //return "2000";
    }

    return NULL;
}

/* Enumerate all the K/V pairs in a profile.
Pointer to NULL passed as variable restarts enumeration.
Function returns 0 if next value exists, -1 at the end of the list. */
int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    UNREFERENCED_PARAMETER(profile_id);
    UNREFERENCED_PARAMETER(variable);
    UNREFERENCED_PARAMETER(value);

    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

void test_on_switch_state_change(
    _In_ sai_switch_oper_status_t switch_oper_status)
{
    printf("^^^ Switch state change - got %d\n", switch_oper_status);
}

void test_on_fdb_event(
    _In_ uint32_t count,
    _In_ sai_fdb_event_notification_data_t *data)
{
    uint32_t ii;

    for (ii = 0; ii < count; ii++) {
        printf("^^^ FDB event - event %d attrs %u\n", data[ii].event_type, data[ii].attr_count);
    }
}

void test_on_port_state_change(
    _In_ uint32_t count,
    _In_ sai_port_oper_status_notification_t *data)
{
    uint32_t ii;

    for (ii = 0; ii < count; ii++) {
        printf("^^^ Port state change - got port %" PRIx64 " %d\n", data[ii].port_id, data[ii].port_state);
    }
}

void test_on_switch_shutdown_request(void)
{
    printf("^^^ Switch shutdown request\n");
}

void test_on_port_event(
    _In_ uint32_t count,
    _In_ sai_port_event_notification_t *data)
{
    uint32_t ii;

    for (ii = 0; ii < count; ii++) {
        printf("^^^ Port event - got port %" PRIx64 " %s\n", data[ii].port_id,
            (SAI_PORT_EVENT_ADD == data[ii].port_event) ? "add" : "delete");
    }
}

void test_on_packet_event(
    _In_ const void *buffer,
    _In_ sai_size_t buffer_size,
    _In_ uint32_t attr_count,
    _In_ const sai_attribute_t *attr_list)
{
    char list_str[MAX_LIST_VALUE_STR_LEN];
    uint32_t ii;

    sai_attr_list_to_str(attr_count, attr_list, host_interface_packet_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("^^^ packet event size %zu attrs %s\n", buffer_size, list_str);

    for (ii = 0; ii < buffer_size; ii++) {
        if ((ii % 16) == 0) {
            // Output the offset.
            printf("\n  %04x ", ii);
        }

        printf(" %02x", ((char *)buffer)[ii]);
    }

    printf("\n");
}

const sai_switch_notification_t test_notification_callbacks = {
    test_on_switch_state_change,
    test_on_fdb_event,
    test_on_port_state_change,
    test_on_port_event,
    test_on_switch_shutdown_request,
    test_on_packet_event
};

int main(int argc, char **argv)
{
    sai_switch_api_t* switch_api;

    sai_api_initialize(0, &test_services);
    sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
    switch_api->initialize_switch(1, "SX", "/",
        (sai_switch_notification_t *)&test_notification_callbacks);
    sai_api_uninitialize();
}
