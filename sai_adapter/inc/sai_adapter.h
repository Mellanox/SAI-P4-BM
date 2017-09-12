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
#include <bm/Standard.h>
#include <bm/standard_types.h>
#include <bm/SimplePreLAG.h>
#include <bm/simple_pre_lag_types.h>
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
#define CPU_HDR_LEN 5
#define ETHER_HDR_LEN ETHER_ADDR_LEN+ETHER_ADDR_LEN+2
#define MAC_LEARN_TRAP_ID 512

typedef struct _ethernet_hdr_t {
  uint8_t dst_addr[ETHER_ADDR_LEN];
  uint8_t src_addr[ETHER_ADDR_LEN];
  uint16_t ether_type;
} ethernet_hdr_t;


typedef enum _cpu_hdr_type_t {
PORT=0,
LAG=1,
VLAN=2,
} cpu_hdr_type_t;

typedef struct _cpu_hdr_t { // TODO: remove bridge_port and id
  uint16_t dst;
  uint16_t trap_id;
  uint8_t  type; // reserved[6] , cpu_hdr_type_t[2]
} cpu_hdr_t;

typedef struct _vlan_hdr_t {
  uint16_t tci;
  uint16_t etherType;
} vlan_hdr_t;

typedef struct pcap_fd {
    int fd;
    pcap_t *pcap;
    const char *dev;
} pcap_fd_t;

typedef struct netdev_fd {
    int fd;
    sai_object_type_t type;
    union {
      uint16_t vid;
      uint16_t hw_port;
      uint16_t lag_id; 
    } data;
} netdev_fd_t;

typedef void (*adapter_packet_handler_fn)(u_char *, cpu_hdr_t *, int);
typedef std::map<uint16_t, adapter_packet_handler_fn> hostif_trap_id_table_t;

const int bm_port_bridge = 9090;
const int bm_port_router = 9091;
const int32_t cxt_id = 0;

string parse_param(uint64_t param, uint32_t num_of_bytes);
BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes);
BmMatchParam parse_valid_match_param(bool param);
BmMatchParam parse_lpm_param(uint64_t param, uint32_t num_of_bytes, uint32_t prefix_length);
BmMatchParam parse_ternary_param(uint64_t param, uint32_t num_of_bytes, uint64_t mask);
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
  // logger
  std::shared_ptr<spdlog::logger> logger_o;
  static vector<sai_object_id_t> *switch_list_ptr;
  static sai_id_map_t *sai_id_map_ptr;
  static StandardClient *bm_bridge_client_ptr;
  static StandardClient *bm_router_client_ptr;
  static SimplePreLAGClient *bm_bridge_client_mc_ptr;
  static Switch_metadata *switch_metadata_ptr;
  static std::shared_ptr<spdlog::logger> *logger;

  // sai api functions
  static sai_status_t create_switch(sai_object_id_t *switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list);
  static sai_status_t remove_switch(sai_object_id_t switch_id);
  static sai_status_t get_switch_attribute(sai_object_id_t switch_id,
                                           sai_uint32_t attr_count,
                                           sai_attribute_t *attr_list);
  static sai_status_t set_switch_attribute(sai_object_id_t switch_id, const sai_attribute_t *attr);
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
  static bool set_parsed_port_attribute(Port_obj *port,
                                        sai_attribute_t attribute);
  static sai_status_t get_parsed_port_attribute(Port_obj *port,
                                        sai_attribute_t *attribute);
  static sai_status_t get_port_stats(sai_object_id_t port_id, const sai_port_stat_t *counter_ids, uint32_t number_of_counters, uint64_t *counters);
  static sai_status_t create_bridge(sai_object_id_t *bridge_id,
                                    sai_object_id_t switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list);
  static sai_status_t remove_bridge(sai_object_id_t bridge_id);
  static sai_status_t get_bridge_attribute(sai_object_id_t bridge_id,
                                           uint32_t attr_count,
                                           sai_attribute_t *attr_list);
  static sai_status_t set_bridge_attribute(sai_object_id_t bridge_id,const sai_attribute_t *attr);
  static sai_status_t create_bridge_port(sai_object_id_t *bridge_port_id,
                                         sai_object_id_t switch_id,
                                         uint32_t attr_count,
                                         const sai_attribute_t *attr_list);
  static sai_status_t remove_bridge_port(sai_object_id_t bridge_port_id);
  static sai_status_t get_bridge_port_attribute(sai_object_id_t bridge_port_id,
                                                uint32_t attr_count,
                                                sai_attribute_t *attr_list);
  static sai_status_t set_bridge_port_attribute(sai_object_id_t bridge_port_id, const sai_attribute_t *attr);
  static sai_status_t create_fdb_entry(const sai_fdb_entry_t *fdb_entry,
                                       uint32_t attr_count,
                                       const sai_attribute_t *attr_list);
  static sai_status_t remove_fdb_entry(const sai_fdb_entry_t *fdb_entry);
  static sai_status_t flush_fdb_entries(sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list);
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
static sai_status_t create_vlan_members(sai_object_id_t switch_id,uint32_t object_count,const uint32_t *attr_count,const sai_attribute_t **attrs,sai_bulk_op_type_t type,sai_object_id_t *object_id,sai_status_t *object_statuses);

static sai_status_t remove_vlan_members(uint32_t object_count,const sai_object_id_t *object_id,sai_bulk_op_type_t type,sai_status_t *object_statuses);

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
  static sai_status_t create_hostif(sai_object_id_t *hif_id,
                                    sai_object_id_t switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list);
  static sai_status_t remove_hostif(sai_object_id_t hif_id);
  static sai_status_t set_hostif_attribute(sai_object_id_t hif_id, const sai_attribute_t *attr);
  static sai_status_t get_hostif_attribute(sai_object_id_t hif_id, uint32_t attr_count, sai_attribute_t *attr_list);
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
  static sai_status_t set_hostif_trap_group_attribute(sai_object_id_t hostif_trap_group_id,const sai_attribute_t *attr);
  static sai_status_t get_hostif_trap_group_attribute(sai_object_id_t hostif_trap_group_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_hostif_trap(sai_object_id_t *hostif_trap_id,
                                         sai_object_id_t switch_id,
                                         uint32_t attr_count,
                                         const sai_attribute_t *attr_list);
  static sai_status_t remove_hostif_trap(sai_object_id_t hostif_trap_id);
  static sai_status_t create_virtual_router (sai_object_id_t *vr_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list);
  static sai_status_t remove_virtual_router(sai_object_id_t vr_id);
  static sai_status_t set_virtual_router_attribute(sai_object_id_t vr_id, const sai_attribute_t *attr);
  static sai_status_t get_virtual_router_attribute(sai_object_id_t vr_id, uint32_t attr_count, sai_attribute_t *attr_list);
  static sai_status_t create_router_interface (sai_object_id_t *lag_member_id,
                                            sai_object_id_t switch_id,
                                            uint32_t attr_count,
                                            const sai_attribute_t *attr_list);
  static sai_status_t remove_router_interface (sai_object_id_t router_interface_id);
  static sai_status_t set_router_interface_attribute(sai_object_id_t rif_id, const sai_attribute_t *attr);
  static sai_status_t get_router_interface_attribute(sai_object_id_t rif_id, uint32_t attr_count, sai_attribute_t *attr_list);

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
  static sai_status_t create_policer(sai_object_id_t *policer_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_policer(sai_object_id_t policer_id);
  static sai_status_t set_policer_attribute(sai_object_id_t policer_id,const sai_attribute_t *attr);
  static sai_status_t get_policer_attribute(sai_object_id_t policer_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t get_policer_stats(sai_object_id_t policer_id,const sai_policer_stat_t *counter_ids,uint32_t number_of_counters,uint64_t *counters);
  static sai_status_t clear_policer_stats(sai_object_id_t policer_id,uint32_t number_of_counters,const sai_policer_stat_t *counter_ids);
  static sai_status_t create_mirror_session(sai_object_id_t *session_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_mirror_session(sai_object_id_t session_id);
  static sai_status_t set_mirror_session_attribute(sai_object_id_t session_id,const sai_attribute_t *attr);
  static sai_status_t get_mirror_session_attribute(sai_object_id_t session_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_stp(sai_object_id_t *stp_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_stp(sai_object_id_t stp_id);
  static sai_status_t set_stp_attribute(sai_object_id_t stp_id,const sai_attribute_t *attr);
  static sai_status_t get_stp_attribute(sai_object_id_t stp_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_stp_port(sai_object_id_t *stp_port_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_stp_port(sai_object_id_t stp_port_id);
  static sai_status_t set_stp_port_attribute(sai_object_id_t stp_port_id,const sai_attribute_t *attr);
  static sai_status_t get_stp_port_attribute(sai_object_id_t stp_port_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_stp_ports(sai_object_id_t switch_id,uint32_t object_count,const uint32_t *attr_count,const sai_attribute_t **attrs,sai_bulk_op_type_t type,sai_object_id_t *object_id,sai_status_t *object_statuses);
  static sai_status_t remove_stp_ports(uint32_t object_count,const sai_object_id_t *object_id,sai_bulk_op_type_t type,sai_status_t *object_statuses);
  static sai_status_t create_samplepacket(sai_object_id_t *samplepacket_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_samplepacket(sai_object_id_t samplepacket_id);
  static sai_status_t set_samplepacket_attribute(sai_object_id_t samplepacket_id,const sai_attribute_t *attr);
  static sai_status_t get_samplepacket_attribute(sai_object_id_t samplepacket_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_wred(sai_object_id_t *wred_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_wred(sai_object_id_t wred_id);
  static sai_status_t set_wred_attribute(sai_object_id_t wred_id,const sai_attribute_t *attr);
  static sai_status_t get_wred_attribute(sai_object_id_t wred_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_qos_map(sai_object_id_t *qos_map_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_qos_map(sai_object_id_t qos_map_id);
  static sai_status_t set_qos_map_attribute(sai_object_id_t qos_map_id,const sai_attribute_t *attr);
  static sai_status_t get_qos_map_attribute(sai_object_id_t qos_map_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_queue(sai_object_id_t *queue_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_queue(sai_object_id_t queue_id);
  static sai_status_t set_queue_attribute(sai_object_id_t queue_id,const sai_attribute_t *attr);
  static sai_status_t get_queue_attribute(sai_object_id_t queue_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t get_queue_stats(sai_object_id_t queue_id,const sai_queue_stat_t *counter_ids,uint32_t number_of_counters,uint64_t *counters);
  static sai_status_t clear_queue_stats(sai_object_id_t queue_id,const sai_queue_stat_t *counter_ids,uint32_t number_of_counters);
  static sai_status_t create_scheduler_group(sai_object_id_t *scheduler_group_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_scheduler_group(sai_object_id_t scheduler_group_id);
  static sai_status_t set_scheduler_group_attribute(sai_object_id_t scheduler_group_id,const sai_attribute_t *attr);
  static sai_status_t get_scheduler_group_attribute(sai_object_id_t scheduler_group_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_scheduler(sai_object_id_t *scheduler_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_scheduler(sai_object_id_t scheduler_id);
  static sai_status_t set_scheduler_attribute(sai_object_id_t scheduler_id,const sai_attribute_t *attr);
  static sai_status_t get_scheduler_attribute(sai_object_id_t scheduler_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_hash(sai_object_id_t *hash_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_hash(sai_object_id_t hash_id);
  static sai_status_t set_hash_attribute(sai_object_id_t hash_id,const sai_attribute_t *attr);
  static sai_status_t get_hash_attribute(sai_object_id_t hash_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_udf(sai_object_id_t *udf_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_udf(sai_object_id_t udf_id);
  static sai_status_t set_udf_attribute(sai_object_id_t udf_id,const sai_attribute_t *attr);
  static sai_status_t get_udf_attribute(sai_object_id_t udf_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_udf_match(sai_object_id_t *udf_match_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_udf_match(sai_object_id_t udf_match_id);
  static sai_status_t set_udf_match_attribute(sai_object_id_t udf_match_id,const sai_attribute_t *attr);
  static sai_status_t get_udf_match_attribute(sai_object_id_t udf_match_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_udf_group(sai_object_id_t *udf_group_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_udf_group(sai_object_id_t udf_group_id);
  static sai_status_t set_udf_group_attribute(sai_object_id_t udf_group_id,const sai_attribute_t *attr);
  static sai_status_t get_udf_group_attribute(sai_object_id_t udf_group_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_l2mc_group(sai_object_id_t *l2mc_group_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_l2mc_group(sai_object_id_t l2mc_group_id);
  static sai_status_t set_l2mc_group_attribute(sai_object_id_t l2mc_group_id,const sai_attribute_t *attr);
  static sai_status_t get_l2mc_group_attribute(sai_object_id_t l2mc_group_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_l2mc_group_member(sai_object_id_t *l2mc_group_member_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_l2mc_group_member(sai_object_id_t l2mc_group_member_id);
  static sai_status_t set_l2mc_group_member_attribute(sai_object_id_t l2mc_group_member_id,const sai_attribute_t *attr);
  static sai_status_t get_l2mc_group_member_attribute(sai_object_id_t l2mc_group_member_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_ipmc_group(sai_object_id_t *ipmc_group_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_ipmc_group(sai_object_id_t ipmc_group_id);
  static sai_status_t set_ipmc_group_attribute(sai_object_id_t ipmc_group_id,const sai_attribute_t *attr);
  static sai_status_t get_ipmc_group_attribute(sai_object_id_t ipmc_group_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_ipmc_group_member(sai_object_id_t *ipmc_group_member_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_ipmc_group_member(sai_object_id_t ipmc_group_member_id);
  static sai_status_t set_ipmc_group_member_attribute(sai_object_id_t ipmc_group_member_id,const sai_attribute_t *attr);
  static sai_status_t get_ipmc_group_member_attribute(sai_object_id_t ipmc_group_member_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_tunnel_map(sai_object_id_t *tunnel_map_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_tunnel_map(sai_object_id_t tunnel_map_id);
  static sai_status_t set_tunnel_map_attribute(sai_object_id_t tunnel_map_id,const sai_attribute_t *attr);
  static sai_status_t get_tunnel_map_attribute(sai_object_id_t tunnel_map_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_tunnel(sai_object_id_t *tunnel_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_tunnel(sai_object_id_t tunnel_id);
  static sai_status_t set_tunnel_attribute(sai_object_id_t tunnel_id,const sai_attribute_t *attr);
  static sai_status_t get_tunnel_attribute(sai_object_id_t tunnel_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_tunnel_term_table_entry(sai_object_id_t *tunnel_term_table_entry_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_tunnel_term_table_entry(sai_object_id_t tunnel_term_table_entry_id);
  static sai_status_t set_tunnel_term_table_entry_attribute(sai_object_id_t tunnel_term_table_entry_id,const sai_attribute_t *attr);
  static sai_status_t get_tunnel_term_table_entry_attribute(sai_object_id_t tunnel_term_table_entry_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_tunnel_map_entry(sai_object_id_t *tunnel_map_entry_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_tunnel_map_entry(sai_object_id_t tunnel_map_entry_id);
  static sai_status_t set_tunnel_map_entry_attribute(sai_object_id_t tunnel_map_entry_id,const sai_attribute_t *attr);
  static sai_status_t get_tunnel_map_entry_attribute(sai_object_id_t tunnel_map_entry_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_l2mc_entry(const sai_l2mc_entry_t *l2mc_entry,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_l2mc_entry(const sai_l2mc_entry_t *l2mc_entry);
  static sai_status_t set_l2mc_entry_attribute(const sai_l2mc_entry_t *l2mc_entry,const sai_attribute_t *attr);
  static sai_status_t get_l2mc_entry_attribute(const sai_l2mc_entry_t *l2mc_entry,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_ipmc_entry(const sai_ipmc_entry_t *ipmc_entry,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_ipmc_entry(const sai_ipmc_entry_t *ipmc_entry);
  static sai_status_t set_ipmc_entry_attribute(const sai_ipmc_entry_t *ipmc_entry,const sai_attribute_t *attr);
  static sai_status_t get_ipmc_entry_attribute(const sai_ipmc_entry_t *ipmc_entry,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_mcast_fdb_entry(const sai_mcast_fdb_entry_t *fdb_entry,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_mcast_fdb_entry(const sai_mcast_fdb_entry_t *fdb_entry);
  static sai_status_t set_mcast_fdb_entry_attribute(const sai_mcast_fdb_entry_t *fdb_entry,const sai_attribute_t *attr);
  static sai_status_t get_mcast_fdb_entry_attribute(const sai_mcast_fdb_entry_t *fdb_entry,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_buffer_pool(sai_object_id_t *pool_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_buffer_pool(sai_object_id_t pool_id);
  static sai_status_t set_buffer_pool_attribute(sai_object_id_t pool_id,const sai_attribute_t *attr);
  static sai_status_t get_buffer_pool_attribute(sai_object_id_t pool_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t get_buffer_pool_stats(sai_object_id_t pool_id,const sai_buffer_pool_stat_t *counter_ids,uint32_t number_of_counters,uint64_t *counters);
  static sai_status_t clear_buffer_pool_stats(sai_object_id_t pool_id,uint32_t number_of_counters,const sai_buffer_pool_stat_t *counter_ids);
  static sai_status_t create_ingress_priority_group(sai_object_id_t *ingress_pg_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_ingress_priority_group(sai_object_id_t ingress_pg_id);
  static sai_status_t set_ingress_priority_group_attribute(sai_object_id_t ingress_pg_id,const sai_attribute_t *attr);
  static sai_status_t get_ingress_priority_group_attribute(sai_object_id_t ingress_pg_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t get_ingress_priority_group_stats(sai_object_id_t ingress_pg_id,const sai_ingress_priority_group_stat_t *counter_ids,uint32_t number_of_counters,uint64_t *counters);
  static sai_status_t clear_ingress_priority_group_stats(sai_object_id_t ingress_pg_id,uint32_t number_of_counters,const sai_ingress_priority_group_stat_t *counter_ids);
  static sai_status_t create_buffer_profile(sai_object_id_t *buffer_profile_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_buffer_profile(sai_object_id_t buffer_profile_id);
  static sai_status_t set_buffer_profile_attribute(sai_object_id_t buffer_profile_id,const sai_attribute_t *attr);
  static sai_status_t get_buffer_profile_attribute(sai_object_id_t buffer_profile_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_acl_table(sai_object_id_t *acl_table_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_acl_table(sai_object_id_t acl_table_id);
  static sai_status_t set_acl_table_attribute(sai_object_id_t acl_table_id,const sai_attribute_t *attr);
  static sai_status_t get_acl_table_attribute(sai_object_id_t acl_table_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_acl_entry(sai_object_id_t *acl_entry_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_acl_entry(sai_object_id_t acl_entry_id);
  static sai_status_t set_acl_entry_attribute(sai_object_id_t acl_entry_id,const sai_attribute_t *attr);
  static sai_status_t get_acl_entry_attribute(sai_object_id_t acl_entry_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_acl_counter(sai_object_id_t *acl_counter_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_acl_counter(sai_object_id_t acl_counter_id);
  static sai_status_t set_acl_counter_attribute(sai_object_id_t acl_counter_id,const sai_attribute_t *attr);
  static sai_status_t get_acl_counter_attribute(sai_object_id_t acl_counter_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_acl_range(sai_object_id_t *acl_range_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_acl_range(sai_object_id_t acl_range_id);
  static sai_status_t set_acl_range_attribute(sai_object_id_t acl_range_id,const sai_attribute_t *attr);
  static sai_status_t get_acl_range_attribute(sai_object_id_t acl_range_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_acl_table_group(sai_object_id_t *acl_table_group_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_acl_table_group(sai_object_id_t acl_table_group_id);
  static sai_status_t set_acl_table_group_attribute(sai_object_id_t acl_table_group_id,const sai_attribute_t *attr);
  static sai_status_t get_acl_table_group_attribute(sai_object_id_t acl_table_group_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_acl_table_group_member(sai_object_id_t *acl_table_group_member_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
  static sai_status_t remove_acl_table_group_member(sai_object_id_t acl_table_group_member_id);
  static sai_status_t set_acl_table_group_member_attribute(sai_object_id_t acl_table_group_member_id,const sai_attribute_t *attr);
  static sai_status_t get_acl_table_group_member_attribute(sai_object_id_t acl_table_group_member_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_rpf_group(sai_object_id_t *rpf_group_id,sai_object_id_t switch_id);
  static sai_status_t remove_rpf_group(sai_object_id_t rpf_group_id);
  static sai_status_t set_rpf_group_attribute(sai_object_id_t rpf_group_id,const sai_attribute_t *attr);
  static sai_status_t get_rpf_group_attribute(sai_object_id_t rpf_group_id,uint32_t attr_count,sai_attribute_t *attr_list);
  static sai_status_t create_rpf_group_member(sai_object_id_t *rpf_group_member_id,sai_object_id_t switch_id);
  static sai_status_t remove_rpf_group_member(sai_object_id_t rpf_group_member_id);
  static sai_status_t set_rpf_group_member_attribute(sai_object_id_t rpf_group_member_id,const sai_attribute_t *attr);
  static sai_status_t get_rpf_group_member_attribute(sai_object_id_t rpf_group_member_id,uint32_t attr_count,sai_attribute_t *attr_list);

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
  sai_acl_api_t acl_api;
  sai_mirror_api_t mirror_api;
  sai_samplepacket_api_t samplepacket_api;
  sai_stp_api_t stp_api;
  sai_policer_api_t policer_api;
  sai_wred_api_t wred_api;
  sai_qos_map_api_t qos_map_api;
  sai_queue_api_t queue_api;
  sai_scheduler_api_t scheduler_api;
  sai_scheduler_group_api_t scheduler_group_api;
  sai_buffer_api_t buffer_api;
  sai_hash_api_t hash_api;
  sai_udf_api_t udf_api;
  sai_tunnel_api_t tunnel_api;
  sai_l2mc_api_t l2mc_api;
  sai_ipmc_api_t ipmc_api;
  sai_rpf_group_api_t rpf_group_api;
  sai_l2mc_group_api_t l2mc_group_api;
  sai_ipmc_group_api_t ipmc_group_api;
  sai_mcast_fdb_api_t mcast_fdb_api;
  sai_adapter();
  ~sai_adapter();
  sai_status_t sai_api_query(sai_api_t sai_api_id, void **api_method_table);
  static sai_object_type_t _sai_object_type_query(sai_object_id_t);

private:
  // sai_object_id_t switch_id;
  // sai adapter threading handlers
  static pcap_fd_t cpu_port[2];
  static std::vector<netdev_fd_t> active_netdevs;
  std::thread SaiAdapterThread;
  static bool pcap_loop_started;
  static std::mutex m;
  static std::condition_variable cv;
  // TODO - move to switch_metadata.
  static hostif_trap_id_table_t hostif_trap_id_table;
  static adapter_packet_handler_fn wildcard_entry;
  static int sniff_pipe_fd[2];
  static int nl_fd;
  void startSaiAdapterMain();
  void endSaiAdapterMain();
  void SaiAdapterMain();
  void release_pcap_lock();
  //
  void PacketSniffer();
  static uint32_t
  get_bridge_id_from_fdb_entry(const sai_fdb_entry_t *fdb_entry);
  static void cpu_port_packetHandler(u_char *, const struct pcap_pkthdr *,
                            const u_char *);
  static void build_fdb_entry(sai_mac_t mac, sai_fdb_entry_bridge_type_t bridge_type, sai_vlan_id_t vlan_id, sai_object_id_t bridge_id, sai_fdb_entry_t *fdb_entry);
  static void learn_mac(u_char *, cpu_hdr_t *, int);
  static void lookup_hostif_trap_id_table(u_char *packet, cpu_hdr_t *, int);
  static void add_hostif_trap_id_table_entry(uint16_t,
                                             adapter_packet_handler_fn);
  static void netdev_phys_port_fn(u_char *, cpu_hdr_t *, int);
  static void netdev_vlan_fn(u_char *, cpu_hdr_t *, int);
  static void phys_netdev_packet_handler(int, int, const u_char *);
  static int phys_netdev_sniffer(int, int);
  static void vlan_netdev_packet_handler(uint16_t vlan_id, int length, const u_char *packet);
  static int vlan_netdev_sniffer(int in_dev_fd, uint16_t vlan_id);
  static void update_mc_node_vlan(Vlan_obj *vlan);
  static sai_status_t init_switch(); //bool deafult_mac_set);//, bool fdb_notification_set);
  static int handle_nl_msg(char buff[], int len);
  static Port_obj* get_port_by_if_name(const char *if_name);
  static void send_link_status_message(int ifi_index, bool admin_state);
  static int get_port_ifi_index(int hw_port);
  // static void update_mc_node_bridge(Bridge_obj *bridge);
  // hostif_table_t hostif_table;
  // static hostif_table_t* hostif_table_p;
};

#endif