#ifndef SAI_ADAPTER_H
#define SAI_ADAPTER_H

// SAI
#ifdef __cplusplus
extern "C" {
#endif
// TUN/TAP if
#include "tun_if.h"
#include <sai.h>
#ifdef __cplusplus
}
#endif

// INTERNAL
#include "switch_meta_data.h"
// #include "sai_hostif_table.h"

// thrift bm clinet
#include <Standard.h>
#include <standard_types.h>
#include <SimplePreLAG.h>
#include <simple_pre_lag_types.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TMultiplexedProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

// LOG
#include "../inc/spdlog/spdlog.h"

// General
#include <algorithm>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <pcap.h>
#include <sstream>
#include <string>
#include <sys/select.h>
// #include <team.h>
#include <thread>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace bm_runtime::standard;
using namespace bm_runtime::simple_pre_lag;

#define ETHER_ADDR_LEN 6
#define CPU_HDR_LEN 3
#define MAC_LEARN_TRAP_ID 512

typedef struct _ethernet_hdr_t {
  uint8_t dst_addr[ETHER_ADDR_LEN];
  uint8_t src_addr[ETHER_ADDR_LEN];
  uint16_t ether_type;
} ethernet_hdr_t;

typedef struct _cpu_hdr_t { // TODO: remove bridge_port and id
  unsigned int ingress_port : 8;
  unsigned int trap_id : 16;
} cpu_hdr_t;

typedef void (*adapter_packet_handler_fn)(u_char *, cpu_hdr_t *, int);
typedef std::map<uint16_t, adapter_packet_handler_fn> hostif_trap_id_table_t;

const int bm_port_bridge = 9090;
const int bm_port_router = 9091;
const int32_t cxt_id = 0;

string parse_param(uint64_t param, uint32_t num_of_bytes);
BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes);
BmMatchParam parse_valid_match_param(bool param);
BmMatchParam parse_lpm_param(uint64_t param, uint32_t num_of_bytes, uint32_t prefix_length);
uint64_t parse_mac_64(uint8_t const mac_8[6]);
void print_mac_to_log(const uint8_t *, std::shared_ptr<spdlog::logger>);
class sai_adapter {
public:
  // thrift
  boost::shared_ptr<TTransport> socket;
  boost::shared_ptr<TTransport> transport;
  boost::shared_ptr<TProtocol> bprotocol;
  boost::shared_ptr<TProtocol> protocol;
  boost::shared_ptr<TProtocol> mc_protocol;
  boost::shared_ptr<TTransport> router_socket;
  boost::shared_ptr<TTransport> router_transport;
  boost::shared_ptr<TProtocol> router_bprotocol;
  boost::shared_ptr<TProtocol> router_protocol;
  StandardClient bm_bridge_client;
  StandardClient bm_router_client;
  SimplePreLAGClient bm_bridge_client_mc;
  // generals
  sai_id_map_t sai_id_map;
  Switch_metadata switch_metadata;
  vector<sai_object_id_t> switch_list;
  uint32_t list[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  // logger
  std::shared_ptr<spdlog::logger> logger_o;
  static vector<sai_object_id_t> *switch_list_ptr;
  static sai_id_map_t *sai_id_map_ptr;
  static StandardClient *bm_bridge_client_ptr;
  static StandardClient *bm_router_client_ptr;
  static SimplePreLAGClient *bm_bridge_client_mc_ptr;
  static Switch_metadata *switch_metadata_ptr;
  static std::shared_ptr<spdlog::logger> *logger;
  // switch
  static sai_status_t create_switch(sai_object_id_t *switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list);
  static sai_status_t get_switch_attribute(sai_object_id_t switch_id,
                                           sai_uint32_t attr_count,
                                           sai_attribute_t *attr_list);
  static sai_status_t set_switch_attribute(sai_object_id_t switch_id, const sai_attribute_t *attr);
  // port functions
  static sai_status_t create_port(sai_object_id_t *port_id,
                                  sai_object_id_t switch_id,
                                  uint32_t attr_count,
                                  const sai_attribute_t *attr_list);
  static sai_status_t remove_port(sai_object_id_t port_id);
  static sai_status_t set_port_attribute(sai_object_id_t port_id,
                                         const sai_attribute_t *attr);
  static sai_status_t get_port_attribute(sai_object_id_t port_id,
                                         uint32_t attr_count,
                                         sai_attribute_t *attr_list);
  static void config_port(Port_obj *port);
  static void set_parsed_port_attribute(Port_obj *port,
                                        sai_attribute_t attribute);
  static void get_parsed_port_attribute(Port_obj *port,
                                        sai_attribute_t *attribute);
  // bridge functions
  static sai_status_t create_bridge(sai_object_id_t *bridge_id,
                                    sai_object_id_t switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list);
  static sai_status_t remove_bridge(sai_object_id_t bridge_id);
  static sai_status_t get_bridge_attribute(sai_object_id_t bridge_id,
                                           uint32_t attr_count,
                                           sai_attribute_t *attr_list);

  // bridge_port functions
  static sai_status_t create_bridge_port(sai_object_id_t *bridge_port_id,
                                         sai_object_id_t switch_id,
                                         uint32_t attr_count,
                                         const sai_attribute_t *attr_list);
  static sai_status_t remove_bridge_port(sai_object_id_t bridge_port_id);
  static sai_status_t get_bridge_port_attribute(sai_object_id_t bridge_port_id,
                                                uint32_t attr_count,
                                                sai_attribute_t *attr_list);
  // fdb
  static sai_status_t create_fdb_entry(const sai_fdb_entry_t *fdb_entry,
                                       uint32_t attr_count,
                                       const sai_attribute_t *attr_list);
  static sai_status_t remove_fdb_entry(const sai_fdb_entry_t *fdb_entry);
  static sai_status_t flush_fdb_entries(sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list);

  // vlan
  static sai_status_t create_vlan(sai_object_id_t *vlan_id,
                                  sai_object_id_t switch_id,
                                  uint32_t attr_count,
                                  const sai_attribute_t *attr_list);
  static sai_status_t remove_vlan(sai_object_id_t vlan_id);
  static sai_status_t set_vlan_attribute(sai_object_id_t vlan_id,
                                         const sai_attribute_t *attr);
  static sai_status_t get_vlan_attribute(sai_object_id_t vlan_id,
                                         const uint32_t attr_count,
                                         sai_attribute_t *attr_list);
  static sai_status_t create_vlan_member(sai_object_id_t *vlan_member_id,
                                         sai_object_id_t switch_id,
                                         uint32_t attr_count,
                                         const sai_attribute_t *attr_list);
  static sai_status_t remove_vlan_member(sai_object_id_t vlan_member_id);
  static sai_status_t set_vlan_member_attribute(sai_object_id_t vlan_member_id,
                                                const sai_attribute_t *attr);
  static sai_status_t get_vlan_member_attribute(sai_object_id_t vlan_member_id,
                                                const uint32_t attr_count,
                                                sai_attribute_t *attr_list);
  static sai_status_t get_vlan_stats(sai_object_id_t vlan_id,
                                     const sai_vlan_stat_t *counter_ids,
                                     uint32_t number_of_counters,
                                     uint64_t *counters);
  static sai_status_t clear_vlan_stats(sai_object_id_t vlan_id,
                                       const sai_vlan_stat_t *counter_ids,
                                       uint32_t number_of_counters);
  // lag
  static sai_status_t create_lag(sai_object_id_t *lag_id,
                                 sai_object_id_t switch_id, uint32_t attr_count,
                                 const sai_attribute_t *attr_list);
  static sai_status_t remove_lag(sai_object_id_t lag_id);
  static sai_status_t create_lag_member(sai_object_id_t *lag_member_id,
                                        sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list);
  static sai_status_t remove_lag_member(sai_object_id_t lag_member_id);
  static sai_status_t get_lag_member_attribute(sai_object_id_t lag_member_id, uint32_t attr_count, sai_attribute_t *attr_list);
  static void get_parsed_lag_attribute(Lag_member_obj *lag_member, sai_attribute_t *attribute);

  // hostif
  static sai_status_t create_hostif(sai_object_id_t *hif_id,
                                    sai_object_id_t switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list);
  static sai_status_t remove_hostif(sai_object_id_t hif_id);
  static sai_status_t
  create_hostif_table_entry(sai_object_id_t *hif_table_entry,
                            sai_object_id_t switch_id, uint32_t attr_count,
                            const sai_attribute_t *attr_list);
  static sai_status_t
  remove_hostif_table_entry(sai_object_id_t hif_table_entry);
  static sai_status_t
  create_hostif_trap_group(sai_object_id_t *hostif_trap_group_id,
                           sai_object_id_t switch_id, uint32_t attr_count,
                           const sai_attribute_t *attr_list);
  static sai_status_t
  remove_hostif_trap_group(sai_object_id_t hostif_trap_group_id);
  static sai_status_t create_hostif_trap(sai_object_id_t *hostif_trap_id,
                                         sai_object_id_t switch_id,
                                         uint32_t attr_count,
                                         const sai_attribute_t *attr_list);
  static sai_status_t remove_hostif_trap(sai_object_id_t hostif_trap_id);

  // ROUTER
  static sai_status_t create_virtual_router (sai_object_id_t *vr_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list);
  static sai_status_t remove_virtual_router(sai_object_id_t vr_id);

  static sai_status_t create_router_interface (sai_object_id_t *lag_member_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list);
  static sai_status_t remove_router_interface (sai_object_id_t router_interface_id);

  static sai_status_t create_neighbor_entry (const sai_neighbor_entry_t *neighbor_entry,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list);
  static sai_status_t remove_neighbor_entry(const sai_neighbor_entry_t *neighbor_entry);

  static sai_status_t create_next_hop (sai_object_id_t *next_hop_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list);
  static sai_status_t remove_next_hop(sai_object_id_t next_hop_id);

  static sai_status_t create_route_entry(const sai_route_entry_t *route_entry,
                                         uint32_t attr_count,
                                         const sai_attribute_t *attr_list);
  static sai_status_t remove_route_entry(const sai_route_entry_t *route_entry);

  // api s
  sai_port_api_t port_api;
  sai_bridge_api_t bridge_api;
  sai_fdb_api_t fdb_api;
  sai_switch_api_t switch_api;
  sai_vlan_api_t vlan_api;
  sai_lag_api_t lag_api;
  sai_hostif_api_t hostif_api;
  sai_virtual_router_api_t virtual_router_api;
  sai_route_api_t route_api;
  sai_next_hop_api_t next_hop_api;
  sai_next_hop_group_api_t next_hop_group_api;
  sai_router_interface_api_t router_interface_api;
  sai_neighbor_api_t neighbor_api;
  sai_adapter();
  ~sai_adapter();
  sai_status_t sai_api_query(sai_api_t sai_api_id, void **api_method_table);

private:
  // sai_object_id_t switch_id;
  static pcap_t *adapter_pcap;
  // sai adapter threading handlers
  std::thread SaiAdapterThread;
  static bool pcap_loop_started;
  static std::mutex m;
  std::condition_variable cv;
  static hostif_trap_id_table_t hostif_trap_id_table;
  void startSaiAdapterMain();
  void endSaiAdapterMain();
  void SaiAdapterMain();
  void release_pcap_lock();
  //
  void PacketSniffer();
  void internal_init_switch();
  static uint32_t
  get_bridge_id_from_fdb_entry(const sai_fdb_entry_t *fdb_entry);
  static void packetHandler(u_char *, const struct pcap_pkthdr *,
                            const u_char *);
  static void adapter_create_fdb_entry(sai_object_id_t, sai_mac_t,
                                       sai_fdb_entry_bridge_type_t,
                                       sai_vlan_id_t, sai_object_id_t);
  static void learn_mac(u_char *, cpu_hdr_t *, int);
  static void netdev_phys_port_fn(u_char *, cpu_hdr_t *, int);
  static void lookup_hostif_trap_id_table(u_char *packet, cpu_hdr_t *, int);
  static void add_hostif_trap_id_table_entry(uint16_t,
                                             adapter_packet_handler_fn);
  static void phys_netdev_packet_handler(int, int, const u_char *);
  static int phys_netdev_sniffer(int, int);
  static void update_mc_node_vlan(Vlan_obj *vlan);
  // static void update_mc_node_bridge(Bridge_obj *bridge);
  // hostif_table_t hostif_table;
  // static hostif_table_t* hostif_table_p;
};

#endif
