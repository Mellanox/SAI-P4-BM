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
    // UNREFERENCED_PARAMETER(profile_id);

    if (!strcmp(variable, "SAI_KEY_INIT_CONFIG_FILE")) {
        return "/usr/share/sai_2410.xml";
    }
    else if (!strcmp(variable, "KV_DEVICE_MAC_ADDRESS")) {
        return "20:03:04:05:06:00";
    }
    else if (!strcmp(variable, "SAI_KEY_L3_ROUTE_TABLE_SIZE")) {
        //return "1000";
    }
    else if (!strcmp(variable, "SAI_KEY_L3_NEIGHBOR_TABLE_SIZE")) {
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
    // UNREFERENCED_PARAMETER(profile_id);
    // UNREFERENCED_PARAMETER(variable);
    // UNREFERENCED_PARAMETER(value);

    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

/* Enumerate all the K/V pairs in a profile.
Pointer to NULL passed as variable restarts enumeration.
Function returns 0 if next value exists, -1 at the end of the list. */

// typedef void(*sai_packet_event_notification_fn)(
//         _In_ sai_object_id_t switch_id,
//         _In_ const void *buffer,
//         _In_ sai_size_t buffer_size,
//         _In_ uint32_t attr_count,
//         _In_ const sai_attribute_t *attr_list);
void* fdb_miss_event_notification(sai_object_id_t switch_id,
	            const void *buffer, sai_size_t buffer_size,
	            uint32_t attr_count, const sai_attribute_t *attr_list) {
	printf("FDB MISS.\n");

	// Learn new mac.
}

int main(int argc, char **argv)
{
	sai_object_id_t switch_id = 1;
	sai_api_initialize(0, &test_services);
    // sai_port_api_t* port_api;
    // sai_api_query(SAI_API_PORT, (void**)&port_api);

    // Set packet callback function
    sai_switch_api_t* switch_api;
    sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
    sai_attribute_t sai_attr;
    sai_attr.id = SAI_SWITCH_ATTR_PACKET_EVENT_NOTIFY;
    sai_attr.value.ptr = fdb_miss_event_notification;
    switch_api->set_switch_attribute(switch_id, &sai_attr);

    sai_hostif_api_t* hostif_api;
	sai_api_query(SAI_API_HOSTIF, (void**)&hostif_api); 

    // create trap group (currently only 1.)
    sai_object_id_t prio_group;
	sai_attribute_t sai_attr_list[2];
	sai_attr_list[1].id=SAI_HOSTIF_TRAP_GROUP_ATTR_QUEUE;
	sai_attr_list[1].value.u32 = 0; // high_queue_id; // high_queue_id is a queue element created via QoS SAI API 
	sai_attr_list[2].id= SAI_HOSTIF_TRAP_GROUP_ATTR_POLICER;
	sai_attr_list[2].value.oid = 0; // high_policer_id; //high_policer_id is a policer element created via policer SAI API
	hostif_api->create_hostif_trap_group(&prio_group, switch_id, 2, sai_attr_list);


	// Configuring Trap-IDs
    sai_attribute_t sai_trap_attr[3];
    sai_object_id_t host_trap_id[1];
	// configure STP trap_id
    // sai_trap_attr[0].id=SAI_HOSTIF_TRAP_ATTR_TRAP_GROUP;
    // sai_trap_attr[0].value=&high_prio_group;
    // sai_trap_attr[1].id= SAI_HOSTIF_TRAP_ATTR_TRAP_ACTION;
    // sai_trap_attr[1].value= SAI_PACKET_ACTION_TRAP;
    // sai_trap_attr[2].id= SAI_HOSTIF_TRAP_ATTR_TRAP_TYPE;
    // sai_trap_attr[2].value= SAI_HOSTIF_TRAP_TYPE_STP;
    // hostif_api->create_hostif_trap(&host_trap_id[1],2, sai_trap_attr);
    // configure FDB miss trap-id 
    sai_trap_attr[0].id = SAI_HOSTIF_USER_DEFINED_TRAP_ATTR_TRAP_GROUP;
    sai_trap_attr[0].value.oid = prio_group;
    sai_trap_attr[1].id = SAI_HOSTIF_USER_DEFINED_TRAP_ATTR_TYPE;
    sai_trap_attr[1].value.s32 = SAI_HOSTIF_USER_DEFINED_TRAP_TYPE_FDB;
    sai_trap_attr[2].id = SAI_HOSTIF_USER_DEFINED_TRAP_ATTR_TRAP_PRIORITY;
    sai_trap_attr[2].value.u32 = 0;
    hostif_api->create_hostif_user_defined_trap(&host_trap_id[0], switch_id, 3, sai_trap_attr);


    // Configuring Host tables
    sai_object_id_t host_table_entry[1];
	sai_attribute_t sai_if_channel_attr[3];
	// sai_if_channel_attr[0].id=SAI_HOSTIF_TABLE_ENTRY_ATTR_TYPE;
	// sai_if_channel_attr[0].value= SAI_HOST_INTERFACE_TABLE_ENTRY_TYPE_TRAP_ID;
	// sai_if_channel_attr[1].id= SAI_HOSTIF_TABLE_ENTRY_ATTR_TRAP_ID;
	// sai_if_channel_attr[1].value=host_trap_id[1]; // Object referencing STP trap
	// sai_if_channel_attr[2].id= SAI_HOSTIF_TABLE_ENTRY_ATTR_CHANNEL_TYPE;
	// sai_if_channel_attr[2].value=SAI_HOST_INTERFACE_TABLE_ENTRY_CHANNEL_TYPE_CB;
	// hostif_api->create_hostif_table_entry(&host_table_entry[0], 3, sai_if_channel_attr); 
	sai_if_channel_attr[0].id=SAI_HOSTIF_TABLE_ENTRY_ATTR_TYPE;
	sai_if_channel_attr[0].value.s32 = SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID;
	sai_if_channel_attr[1].id = SAI_HOSTIF_TABLE_ENTRY_ATTR_TRAP_ID;
	sai_if_channel_attr[1].value.oid = host_trap_id[0]; // Object referencing FDB trap
	sai_if_channel_attr[2].id = SAI_HOSTIF_TABLE_ENTRY_ATTR_CHANNEL_TYPE;
	sai_if_channel_attr[2].value.s32 = SAI_HOSTIF_TABLE_ENTRY_CHANNEL_TYPE_CB;
	hostif_api->create_hostif_table_entry(&host_table_entry[0], switch_id, 3, sai_if_channel_attr); 

    sai_api_uninitialize();
}
