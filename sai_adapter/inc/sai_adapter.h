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
#include <team.h>
#include <thread>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace bm_runtime::standard;

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

// static StandardClient* bm_client_ptr;
// static sai_id_map_t* sai_id_map_ptr;
string parse_param(uint64_t param, uint32_t num_of_bytes);
BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes);
BmMatchParam parse_valid_match_param(bool param);
uint64_t parse_mac_64(uint8_t const mac_8[6]);
void print_mac_to_log(const uint8_t *, std::shared_ptr<spdlog::logger>);
class sai_adapter {
public:
  // thrift
  boost::shared_ptr<TTransport> socket;
  boost::shared_ptr<TTransport> transport;
  boost::shared_ptr<TProtocol> bprotocol;
  boost::shared_ptr<TProtocol> protocol;
  StandardClient bm_client;
  // generals
  sai_id_map_t sai_id_map;
  Switch_metadata switch_metadata;
  vector<sai_object_id_t> switch_list;
  uint32_t list[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  // logger
  std::shared_ptr<spdlog::logger> logger_o;
  static vector<sai_object_id_t> *switch_list_ptr;
  static sai_id_map_t *sai_id_map_ptr;
  static StandardClient *bm_client_ptr;
  static Switch_metadata *switch_metadata_ptr;
  static std::shared_ptr<spdlog::logger> *logger;
  // switch
  static sai_status_t create_switch(sai_object_id_t *switch_id,
                                    uint32_t attr_count,
                                    const sai_attribute_t *attr_list);
  static sai_status_t get_switch_attribute(sai_object_id_t switch_id,
                                           sai_uint32_t attr_count,
                                           sai_attribute_t *attr_list);
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

  // api s
  sai_port_api_t port_api;
  sai_bridge_api_t bridge_api;
  sai_fdb_api_t fdb_api;
  sai_switch_api_t switch_api;
  sai_vlan_api_t vlan_api;
  sai_lag_api_t lag_api;
  sai_hostif_api_t hostif_api;
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
  // hostif_table_t hostif_table;
  // static hostif_table_t* hostif_table_p;
};

#endif