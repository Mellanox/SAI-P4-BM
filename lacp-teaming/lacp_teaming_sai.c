#ifdef __cplusplus
#include <spdlog/spdlog.h>
extern "C" {
#endif
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <jansson.h>
#include <sai.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/select.h>
#include <team.h>

#define APPNAME "lacp_teaming_sai"

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

typedef struct _lacp_port_t {
  sai_object_id_t port_id;
  sai_object_id_t bridge_port_id;
  char netdev_name[32];
} lacp_port_t;

sai_object_id_t lag_id;
sai_object_id_t lag_bridge_port_id;
sai_lag_api_t *lag_api;
sai_bridge_api_t *bridge_api;
sai_fdb_api_t *fdb_api;
sai_object_id_t default_bridge_id; 
sai_object_id_t switch_id = 0;
int num_of_ports;
lacp_port_t *ports;

void create_lag(lacp_port_t **lag_ports, int cur_num_of_ports) {
  sai_attribute_t sai_attr[2];
  lag_api->create_lag(&lag_id, switch_id, 0, &sai_attr[0]);
  sai_attr[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
  sai_attr[0].value.oid = lag_id;
  sai_attr[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
  sai_object_id_t lag_member_id;

  sai_attribute_t fdb_flush_attr;
  fdb_flush_attr.id = SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID;
  for (int i = 0; i < cur_num_of_ports; ++i) {
    sai_attr[1].value.oid = lag_ports[i]->port_id;
    lag_api->create_lag_member(&lag_member_id, switch_id, 2, sai_attr);
    fdb_flush_attr.value.oid = lag_ports[i]->bridge_port_id;
    fdb_api->flush_fdb_entries(switch_id, 1, &fdb_flush_attr);
    bridge_api->remove_bridge_port(lag_ports[i]->bridge_port_id);
  }

  sai_attr[0].id = SAI_BRIDGE_PORT_ATTR_BRIDGE_ID;
  sai_attr[0].value.oid = default_bridge_id;
  sai_attr[1].id = SAI_BRIDGE_PORT_ATTR_PORT_ID;
  sai_attr[1].value.oid = lag_id;
  bridge_api->create_bridge_port(&lag_bridge_port_id, switch_id, 2, sai_attr);
}

void remove_lag(lacp_port_t **lag_ports, int cur_num_of_ports) {
  // remove lag
  lag_api->remove_lag(lag_id);
  // remove lag bridge port and re-create default bridge_ports (maybe this shouldn't be here, but a part of sai_adapter when removing from lag)
  bridge_api->remove_bridge_port(lag_bridge_port_id);
  sai_attribute_t sai_attr[2];
  sai_attr[0].id = SAI_BRIDGE_PORT_ATTR_BRIDGE_ID;
  sai_attr[0].value.oid = default_bridge_id;
  sai_attr[1].id = SAI_BRIDGE_PORT_ATTR_PORT_ID;
  for (int i=0; i<cur_num_of_ports; i++) {
    sai_attr[1].value.oid = lag_ports[i]->port_id;
    bridge_api->create_bridge_port(&lag_ports[i]->bridge_port_id, switch_id, 2 ,sai_attr);

  }
}

lacp_port_t* get_lacp_port_by_name(char *port_name) {
  for (int i = 0; i < num_of_ports; ++i) {
    if (strcmp(ports[i].netdev_name, port_name) == 0) {
      return &ports[i];
    }
  }
  printf("port name %s doesn't exist\n", port_name);
  return;
}

sai_object_id_t sai_get_bridge_port_id_by_port_id(sai_object_id_t port_id) {
  sai_bridge_api_t *bridge_api;
  sai_api_query(SAI_API_BRIDGE, (void **)&bridge_api);
  sai_attribute_t attr, bridge_attr;
  int max_ports = 10;
  attr.value.objlist.list = (sai_object_id_t*) malloc(sizeof(sai_object_id_t) * max_ports);
  sai_object_id_t bridge_port_id;
  attr.id = SAI_BRIDGE_ATTR_PORT_LIST;
  bridge_api->get_bridge_attribute(default_bridge_id, 1, &attr);

  bridge_attr.id = SAI_BRIDGE_PORT_ATTR_PORT_ID;
  for (int i=0; i<attr.value.objlist.count; ++i) {
    bridge_port_id = attr.value.objlist.list[i];
    bridge_api->get_bridge_port_attribute(bridge_port_id, 1, &bridge_attr);
    if (port_id == bridge_attr.value.oid) {
      free(attr.value.objlist.list);
      return bridge_port_id;
    }
  }
  printf("No bridge_port found for port id %d\n", port_id);
  free(attr.value.objlist.list);
  return -1;
}

sai_object_id_t sai_get_port_id_by_front_port(uint32_t hw_port) {
  sai_switch_api_t *switch_api;
  sai_port_api_t *port_api;
  sai_api_query(SAI_API_SWITCH, (void **)&switch_api);
  sai_api_query(SAI_API_PORT, (void **)&port_api);

  sai_attribute_t sai_attr;
  sai_attr.id = SAI_SWITCH_ATTR_PORT_NUMBER;
  sai_object_id_t switch_id = 1;
  switch_api->get_switch_attribute(switch_id, 1, &sai_attr);
  uint32_t max_ports = sai_attr.value.u32;

  sai_attr.id = SAI_SWITCH_ATTR_PORT_LIST;
  sai_attr.value.objlist.list =
      (sai_object_id_t *)malloc(sizeof(sai_object_id_t) * max_ports);
  sai_attr.value.objlist.count = max_ports;
  switch_api->get_switch_attribute(switch_id, 1, &sai_attr);

  sai_attribute_t hw_lane_list_attr;

  for (int i = 0; i < max_ports; i++) {
    hw_lane_list_attr.id = SAI_PORT_ATTR_HW_LANE_LIST;
    hw_lane_list_attr.value.u32list.list = (uint32_t *)malloc(
        sizeof(uint32_t));  // hw_lane list with only 1 lane (TODO??)
    port_api->get_port_attribute(sai_attr.value.objlist.list[i], 1,
                                 &hw_lane_list_attr);
    if (hw_lane_list_attr.value.u32list.list[0] == hw_port) {
      free(hw_lane_list_attr.value.u32list.list);
      free(sai_attr.value.objlist.list);
      return sai_attr.value.objlist.list[i];
    }
    free(hw_lane_list_attr.value.u32list.list);
  }
  free(sai_attr.value.objlist.list);
  printf("didn't find port");
  return -1;
}

// team monitor auxillary

static char *get_port_name(struct team_handle *th, uint32_t ifindex) {
  static char ifname[32];

  return team_ifindex2ifname(th, ifindex, ifname, sizeof(ifname));
}

static int die = 0;
static bool lag_up = false;

static void sigint_handler(int signum) { die = 1; }

static void do_main_loop(struct team_handle *th) {
  fd_set rfds;
  fd_set rfds_tmp;
  int fdmax;
  int ret;
  int tfd;

  FD_ZERO(&rfds);
  tfd = team_get_event_fd(th);
  FD_SET(tfd, &rfds);
  fdmax = tfd + 1;

  while (1) {
    rfds_tmp = rfds;
    ret = select(fdmax, &rfds_tmp, NULL, NULL, NULL);
    if (die) break;
    if (ret == -1) {
      perror("select()");
    }
    if (FD_ISSET(tfd, &rfds_tmp)) team_handle_events(th);
  }
}

static int option_change_handler_func(struct team_handle *th, void *arg,
                                      team_change_type_mask_t type_mask) {
  struct team_option *option;
  // printf("------------------\noption change\n\toption list:\n");
  bool carr_up;
  struct team_port *port;
  lacp_port_t *lag_ports[num_of_ports];

  team_for_each_option(option, th) {
    char *port_name;
    int cur_num_of_ports = 0;
    if (team_is_option_changed(option) &
        (strcmp("enabled", team_get_option_name(option)) == 0)) {

      team_for_each_port(port, th) {
        if (team_is_port_link_up(port)) {
          port_name = get_port_name(th, team_get_port_ifindex(port));
          lag_ports[cur_num_of_ports] = get_lacp_port_by_name(port_name);
          cur_num_of_ports++;
        }
      }

      team_carrier_get(th, &carr_up);
      if (carr_up) {
        if (!lag_up) {
          printf("CREATE LAG.\n");
          create_lag(lag_ports, cur_num_of_ports);
          lag_up = true;
        }
      } else {
        if (lag_up) {
          lag_up = false;
          remove_lag(lag_ports, cur_num_of_ports);
          printf("REMOVE LAG\n");
        }
      }
    }
  }
  return 0;
}

static struct team_change_handler option_change_handler = {
    .func = option_change_handler_func, .type_mask = TEAM_OPTION_CHANGE,
};

int teaming_init(const char *json_path, char *team_dev_name) {
  json_error_t jerror;
  size_t jflags = JSON_REJECT_DUPLICATES;
  json_t *json = json_load_file(json_path, jflags, &jerror);
  strcpy(team_dev_name, json_string_value(json_object_get(json, "device")));
  json_t *ports_json = json_object_get(json, "ports");
  void *it = json_object_iter(ports_json);
  for (int i = 0; i < num_of_ports; ++i) {
    if (it == NULL) {
      printf(
          "Number of ports given as argument is less then ports defined in "
          "config file\n");
      return -1;
    }
    strcpy(ports[i].netdev_name, json_object_iter_key(it));
    printf("port %d: '%s'\n", i, ports[i].netdev_name);
    it = json_object_iter_next(ports_json, it);
  }
  return 0;
}

void run_team_monitor(char *team_dev_name) {
  struct team_handle *th;
  int err;
  uint32_t ifindex;

  th = team_alloc();
  if (!th) {
    fprintf(stderr, "team alloc failed.\n");
    return 1;
  }

  ifindex = team_ifname2ifindex(th, team_dev_name);
  if (!ifindex) {
    fprintf(stderr, "Netdevice %s not found.\n", team_dev_name);
    return 1;
  }

  err = team_init(th, ifindex);
  if (err) {
    fprintf(stderr, "team init failed\n");
    return err;
  }

  err = team_change_handler_register(th, &option_change_handler, NULL);
  if (err) {
    fprintf(stderr, "option change handler register failed\n");
    return err;
  }

  signal(SIGINT, sigint_handler);

  do_main_loop(th);

  team_change_handler_unregister(th, &option_change_handler, NULL);
  // team_change_handler_unregister(th, &port_change_handler, NULL);
  team_free(th);
}

// main

void usage() {
  fprintf(stderr, "Usage: %s TEAM_CONF_FILE  port1 port2 [port3...]\n",
          APPNAME);
  return 1;
}

int main(int argc, char **argv) {
  if (argc < 4) {
    usage();
    return -1;
  }
  num_of_ports = argc - 2;
  char team_dev_name[32];
  char conf_file[32];
  strcpy(conf_file, argv[1]);
  ports = (lacp_port_t *)malloc(num_of_ports * sizeof(lacp_port_t));
  if (teaming_init(conf_file, team_dev_name) == -1) {
    return -1;
  }

  printf("%s initializing on team device %s with %d ports\n", APPNAME,
         team_dev_name, num_of_ports);

  // sai init ports
  sai_hostif_api_t *hostif_api;
  sai_switch_api_t *switch_api;
  sai_api_initialize(0, &test_services);

  sai_api_query(SAI_API_HOSTIF, (void **)&hostif_api);
  sai_api_query(SAI_API_LAG, (void **)&lag_api);
  sai_api_query(SAI_API_BRIDGE, (void **)&bridge_api);
  sai_api_query(SAI_API_SWITCH, (void **)&switch_api);
  sai_api_query(SAI_API_FDB, (void **)&fdb_api);
  
  // sai_object_id_t switch_id;
  sai_attribute_t switch_attr;
  switch_attr.id = SAI_SWITCH_ATTR_INIT_SWITCH;
  switch_attr.value.booldata = true;
  switch_api->create_switch(&switch_id, 1, &switch_attr);

  sai_attribute_t sai_attr;
  sai_attr.id = SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID;
  switch_api->get_switch_attribute(switch_id, 1, &sai_attr);
  default_bridge_id = sai_attr.value.oid;
  int i;

  for (i = 0; i < num_of_ports; ++i) {
    ports[i].port_id = sai_get_port_id_by_front_port(atoi(argv[i + 2]));
    ports[i].bridge_port_id = sai_get_bridge_port_id_by_port_id(ports[i].port_id);
  }
  // create trap group (currently only 1.)
  sai_object_id_t prio_group;

  sai_attribute_t sai_attr_list[2];
  // sai_attr_list[0].id = SAI_HOSTIF_TRAP_GROUP_ATTR_PRIO;
  // sai_attr_list[0].value = 7;
  sai_attr_list[0].id = SAI_HOSTIF_TRAP_GROUP_ATTR_QUEUE;
  sai_attr_list[0].value.u32 = 0;  // high_queue_id; // high_queue_id is a queue
                                   // element created via QoS SAI API
  sai_attr_list[1].id = SAI_HOSTIF_TRAP_GROUP_ATTR_POLICER;
  sai_attr_list[1].value.oid = 0;  // high_policer_id; //high_policer_id is a
                                   // policer element created via policer SAI
                                   // API
  hostif_api->create_hostif_trap_group(&prio_group, switch_id, 2,
                                       sai_attr_list);
  // Create host interface channels
  sai_attribute_t sai_if_channel_attr[num_of_ports];
  sai_object_id_t host_if_id[num_of_ports];
  for (int i = 0; i < num_of_ports; ++i) {
    sai_if_channel_attr[0].id = SAI_HOSTIF_ATTR_TYPE;
    sai_if_channel_attr[0].value.s32 = SAI_HOSTIF_TYPE_NETDEV;
    sai_if_channel_attr[1].id = SAI_HOSTIF_ATTR_OBJ_ID;
    sai_if_channel_attr[1].value.oid = ports[i].port_id;
    sai_if_channel_attr[2].id = SAI_HOSTIF_ATTR_NAME;
    strcpy(sai_if_channel_attr[2].value.chardata, ports[i].netdev_name);
    hostif_api->create_hostif(&host_if_id[i], switch_id, 3,
                                sai_if_channel_attr);
  }

  // Configuring Trap-IDs
  sai_attribute_t sai_trap_attr[3];
  sai_object_id_t host_trap_id[1];
  // configure LACP trap_id
  sai_trap_attr[0].id = SAI_HOSTIF_TRAP_ATTR_TRAP_GROUP;
  sai_trap_attr[0].value.oid = prio_group;
  sai_trap_attr[1].id = SAI_HOSTIF_TRAP_ATTR_PACKET_ACTION;
  sai_trap_attr[1].value.s32 = SAI_PACKET_ACTION_TRAP;
  sai_trap_attr[2].id = SAI_HOSTIF_TRAP_ATTR_TRAP_TYPE;
  sai_trap_attr[2].value.s32 = SAI_HOSTIF_TRAP_TYPE_LACP;
  hostif_api->create_hostif_trap(&host_trap_id[0], switch_id, 3, sai_trap_attr);

  // Configuring Host tables
  sai_object_id_t host_table_entry[1];
  sai_if_channel_attr[0].id = SAI_HOSTIF_TABLE_ENTRY_ATTR_TYPE;
  sai_if_channel_attr[0].value.s32 = SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID;
  sai_if_channel_attr[1].id = SAI_HOSTIF_TABLE_ENTRY_ATTR_TRAP_ID;
  sai_if_channel_attr[1].value.oid = host_trap_id[0];
  sai_if_channel_attr[2].id = SAI_HOSTIF_TABLE_ENTRY_ATTR_CHANNEL_TYPE;
  sai_if_channel_attr[2].value.s32 =
      SAI_HOSTIF_TABLE_ENTRY_CHANNEL_TYPE_NETDEV_PHYSICAL_PORT;
  hostif_api->create_hostif_table_entry(&host_table_entry[0], switch_id, 3,
                                        sai_if_channel_attr);
  char cmd[44] = "teamd -d -f ";
  strcat(cmd, conf_file);
  // printf("running system cmd:\t%s\n", cmd);
  int status = system(cmd);
  if (status != 0) {
    printf("error performing system opertaion: \t%s\n", cmd);
    return -1;
  }
  strcpy(cmd, "ip link set ");
  strcat(cmd, team_dev_name);
  strcat(cmd, " up");
  // printf("running system cmd:\t%s\n", cmd);
  status = system(cmd);
  if (status != 0) {
    printf("error performing system opertaion: \t%s\n", cmd);
    return -1;
  }
  printf("%s runing\n", APPNAME);
  run_team_monitor(team_dev_name);

  free(ports);

  // removing configurations
  printf("%s teardown tiated\n", APPNAME);
  hostif_api->remove_hostif_table_entry(host_table_entry[0]);
  hostif_api->remove_hostif_trap(host_trap_id[0]);
  hostif_api->remove_hostif(host_if_id[1]);
  hostif_api->remove_hostif(host_if_id[0]);
  hostif_api->remove_hostif_trap_group(prio_group);
  // spdlog::drop_all();
  sai_api_uninitialize();
  return 0;
}
