#include "../inc/sai_adapter.h"

StandardClient *sai_adapter::bm_client_ptr;
SimplePreLAGClient *sai_adapter::bm_client_mc_ptr;
sai_id_map_t *sai_adapter::sai_id_map_ptr;
Switch_metadata *sai_adapter::switch_metadata_ptr;
std::vector<sai_object_id_t> *sai_adapter::switch_list_ptr;
std::shared_ptr<spdlog::logger> *sai_adapter::logger;
bool sai_adapter::pcap_loop_started;
std::mutex sai_adapter::m;
std::condition_variable sai_adapter::cv;
hostif_trap_id_table_t sai_adapter::hostif_trap_id_table;
pcap_fd_t sai_adapter::cpu_port[1];
int sai_adapter::sniff_pipe_fd[2];
int sai_adapter::nl_fd;
std::vector<netdev_fd_t> sai_adapter::active_netdevs;
adapter_packet_handler_fn sai_adapter::wildcard_entry;

sai_adapter::sai_adapter()
    : //  constructor pre initializations
      socket(new TSocket("localhost", bm_port)),
      transport(new TBufferedTransport(socket)),
      bprotocol(new TBinaryProtocol(transport)),
      protocol(new TMultiplexedProtocol(bprotocol, "standard")),
      bm_client(protocol),
      mc_protocol(new TMultiplexedProtocol(bprotocol, "simple_pre_lag")),
      bm_client_mc(mc_protocol)
       {
  // logger
  logger_o = spdlog::get("logger");
  if (logger_o == 0) {
    logger_o = spdlog::basic_logger_mt("logger", "log.txt");
    logger_o->flush_on(spdlog::level::info);     // make err
    spdlog::set_pattern("[thread %t %T.%e] [%l] %v ");
  }
  logger = &logger_o;

  // start P4 link
  switch_list_ptr = &switch_list;
  switch_metadata_ptr = &switch_metadata;
  bm_client_ptr = &bm_client;
  bm_client_mc_ptr = &bm_client_mc;
  sai_id_map_ptr = &sai_id_map;
  wildcard_entry = NULL;
  transport->open();

  // api set
  switch_api.create_switch = &sai_adapter::create_switch;
  switch_api.remove_switch = &sai_adapter::remove_switch;
  switch_api.get_switch_attribute = &sai_adapter::get_switch_attribute;
  switch_api.set_switch_attribute = &sai_adapter::set_switch_attribute;

  port_api.create_port = &sai_adapter::create_port;
  port_api.remove_port = &sai_adapter::remove_port;
  port_api.set_port_attribute = &sai_adapter::set_port_attribute;
  port_api.get_port_attribute = &sai_adapter::get_port_attribute;
  port_api.get_port_stats = &sai_adapter::get_port_stats;

  bridge_api.create_bridge = &sai_adapter::create_bridge;
  bridge_api.remove_bridge = &sai_adapter::remove_bridge;
  bridge_api.get_bridge_attribute = &sai_adapter::get_bridge_attribute;
  bridge_api.set_bridge_attribute = &sai_adapter::set_bridge_attribute;
  bridge_api.create_bridge_port = &sai_adapter::create_bridge_port;
  bridge_api.remove_bridge_port = &sai_adapter::remove_bridge_port;
  bridge_api.get_bridge_port_attribute =
      &sai_adapter::get_bridge_port_attribute;
  bridge_api.set_bridge_port_attribute =
      &sai_adapter::set_bridge_port_attribute;

  fdb_api.create_fdb_entry = &sai_adapter::create_fdb_entry;
  fdb_api.remove_fdb_entry = &sai_adapter::remove_fdb_entry;
  // fdb_api.set_fdb_entry_attribute = &sai_adapter::set_fdb_entry_attribute;
  // fdb_api.get_fdb_entry_attribute = &sai_adapter::get_fdb_entry_attribute;
  fdb_api.flush_fdb_entries = &sai_adapter::flush_fdb_entries;

  vlan_api.create_vlan = &sai_adapter::create_vlan;
  vlan_api.remove_vlan = &sai_adapter::remove_vlan;
  vlan_api.set_vlan_attribute = &sai_adapter::set_vlan_attribute;
  vlan_api.get_vlan_attribute = &sai_adapter::get_vlan_attribute;
  vlan_api.create_vlan_member = &sai_adapter::create_vlan_member;
  vlan_api.remove_vlan_member = &sai_adapter::remove_vlan_member;
  vlan_api.set_vlan_member_attribute = &sai_adapter::set_vlan_member_attribute;
  vlan_api.get_vlan_member_attribute = &sai_adapter::get_vlan_member_attribute;
  vlan_api.get_vlan_stats = &sai_adapter::get_vlan_stats;
  vlan_api.clear_vlan_stats = &sai_adapter::clear_vlan_stats;

  lag_api.create_lag = &sai_adapter::create_lag;
  lag_api.remove_lag = &sai_adapter::remove_lag;
  lag_api.create_lag_member = &sai_adapter::create_lag_member;
  lag_api.remove_lag_member = &sai_adapter::remove_lag_member;
  lag_api.get_lag_member_attribute = &sai_adapter::get_lag_member_attribute;

  hostif_api.create_hostif = &sai_adapter::create_hostif;
  hostif_api.remove_hostif = &sai_adapter::remove_hostif;
  hostif_api.set_hostif_attribute = &sai_adapter::set_hostif_attribute;
  hostif_api.get_hostif_attribute = &sai_adapter::get_hostif_attribute;
  hostif_api.create_hostif_table_entry =
      &sai_adapter::create_hostif_table_entry;
  hostif_api.remove_hostif_table_entry =
      &sai_adapter::remove_hostif_table_entry;
  hostif_api.create_hostif_trap_group = &sai_adapter::create_hostif_trap_group;
  hostif_api.remove_hostif_trap_group = &sai_adapter::remove_hostif_trap_group;
  hostif_api.set_hostif_trap_group_attribute = &sai_adapter::set_hostif_trap_group_attribute;
  hostif_api.get_hostif_trap_group_attribute = &sai_adapter::get_hostif_trap_group_attribute;
  hostif_api.create_hostif_trap = &sai_adapter::create_hostif_trap;
  hostif_api.remove_hostif_trap = &sai_adapter::remove_hostif_trap;

  router_interface_api.create_router_interface = &sai_adapter::create_router_interface;
  router_interface_api.remove_router_interface = &sai_adapter::remove_router_interface;
  router_interface_api.set_router_interface_attribute = &sai_adapter::set_router_interface_attribute;
  router_interface_api.get_router_interface_attribute = &sai_adapter::get_router_interface_attribute;

  virtual_router_api.create_virtual_router = &sai_adapter::create_virtual_router;
  virtual_router_api.remove_virtual_router = &sai_adapter::remove_virtual_router;
  virtual_router_api.set_virtual_router_attribute = &sai_adapter::set_virtual_router_attribute;
  virtual_router_api.get_virtual_router_attribute = &sai_adapter::get_virtual_router_attribute;

  neighbor_api.create_neighbor_entry = &sai_adapter::create_neighbor_entry;
  neighbor_api.remove_neighbor_entry = &sai_adapter::remove_neighbor_entry;

  next_hop_api.create_next_hop = &sai_adapter::create_next_hop;
  next_hop_api.remove_next_hop = &sai_adapter::remove_next_hop;

  route_api.create_route_entry = &sai_adapter::create_route_entry;
  route_api.remove_route_entry = &sai_adapter::remove_route_entry;

  policer_api.create_policer = &sai_adapter::create_policer;
  policer_api.remove_policer = &sai_adapter::remove_policer;
  policer_api.set_policer_attribute = &sai_adapter::set_policer_attribute;
  policer_api.get_policer_attribute = &sai_adapter::get_policer_attribute;
  policer_api.get_policer_stats = &sai_adapter::get_policer_stats;
  policer_api.clear_policer_stats = &sai_adapter::clear_policer_stats;
  mirror_api.create_mirror_session = &sai_adapter::create_mirror_session;
  mirror_api.remove_mirror_session = &sai_adapter::remove_mirror_session;
  mirror_api.set_mirror_session_attribute = &sai_adapter::set_mirror_session_attribute;
  mirror_api.get_mirror_session_attribute = &sai_adapter::get_mirror_session_attribute;
  stp_api.create_stp = &sai_adapter::create_stp;
  stp_api.remove_stp = &sai_adapter::remove_stp;
  stp_api.set_stp_attribute = &sai_adapter::set_stp_attribute;
  stp_api.get_stp_attribute = &sai_adapter::get_stp_attribute;
  stp_api.create_stp_port = &sai_adapter::create_stp_port;
  stp_api.remove_stp_port = &sai_adapter::remove_stp_port;
  stp_api.set_stp_port_attribute = &sai_adapter::set_stp_port_attribute;
  stp_api.get_stp_port_attribute = &sai_adapter::get_stp_port_attribute;
  stp_api.create_stp_ports = &sai_adapter::create_stp_ports;
  stp_api.remove_stp_ports = &sai_adapter::remove_stp_ports;
  samplepacket_api.create_samplepacket = &sai_adapter::create_samplepacket;
  samplepacket_api.remove_samplepacket = &sai_adapter::remove_samplepacket;
  samplepacket_api.set_samplepacket_attribute = &sai_adapter::set_samplepacket_attribute;
  samplepacket_api.get_samplepacket_attribute = &sai_adapter::get_samplepacket_attribute;
  wred_api.create_wred = &sai_adapter::create_wred;
  wred_api.remove_wred = &sai_adapter::remove_wred;
  wred_api.set_wred_attribute = &sai_adapter::set_wred_attribute;
  wred_api.get_wred_attribute = &sai_adapter::get_wred_attribute;
  qos_map_api.create_qos_map = &sai_adapter::create_qos_map;
  qos_map_api.remove_qos_map = &sai_adapter::remove_qos_map;
  qos_map_api.set_qos_map_attribute = &sai_adapter::set_qos_map_attribute;
  qos_map_api.get_qos_map_attribute = &sai_adapter::get_qos_map_attribute;
  queue_api.create_queue = &sai_adapter::create_queue;
  queue_api.remove_queue = &sai_adapter::remove_queue;
  queue_api.set_queue_attribute = &sai_adapter::set_queue_attribute;
  queue_api.get_queue_attribute = &sai_adapter::get_queue_attribute;
  queue_api.get_queue_stats = &sai_adapter::get_queue_stats;
  queue_api.clear_queue_stats = &sai_adapter::clear_queue_stats;
  scheduler_group_api.create_scheduler_group = &sai_adapter::create_scheduler_group;
  scheduler_group_api.remove_scheduler_group = &sai_adapter::remove_scheduler_group;
  scheduler_group_api.set_scheduler_group_attribute = &sai_adapter::set_scheduler_group_attribute;
  scheduler_group_api.get_scheduler_group_attribute = &sai_adapter::get_scheduler_group_attribute;
  scheduler_api.create_scheduler = &sai_adapter::create_scheduler;
  scheduler_api.remove_scheduler = &sai_adapter::remove_scheduler;
  scheduler_api.set_scheduler_attribute = &sai_adapter::set_scheduler_attribute;
  scheduler_api.get_scheduler_attribute = &sai_adapter::get_scheduler_attribute;
  hash_api.create_hash = &sai_adapter::create_hash;
  hash_api.remove_hash = &sai_adapter::remove_hash;
  hash_api.set_hash_attribute = &sai_adapter::set_hash_attribute;
  hash_api.get_hash_attribute = &sai_adapter::get_hash_attribute;
  udf_api.create_udf = &sai_adapter::create_udf;
  udf_api.remove_udf = &sai_adapter::remove_udf;
  udf_api.set_udf_attribute = &sai_adapter::set_udf_attribute;
  udf_api.get_udf_attribute = &sai_adapter::get_udf_attribute;
  udf_api.create_udf_match = &sai_adapter::create_udf_match;
  udf_api.remove_udf_match = &sai_adapter::remove_udf_match;
  udf_api.set_udf_match_attribute = &sai_adapter::set_udf_match_attribute;
  udf_api.get_udf_match_attribute = &sai_adapter::get_udf_match_attribute;
  udf_api.create_udf_group = &sai_adapter::create_udf_group;
  udf_api.remove_udf_group = &sai_adapter::remove_udf_group;
  udf_api.set_udf_group_attribute = &sai_adapter::set_udf_group_attribute;
  udf_api.get_udf_group_attribute = &sai_adapter::get_udf_group_attribute;
  l2mc_group_api.create_l2mc_group = &sai_adapter::create_l2mc_group;
  l2mc_group_api.remove_l2mc_group = &sai_adapter::remove_l2mc_group;
  l2mc_group_api.set_l2mc_group_attribute = &sai_adapter::set_l2mc_group_attribute;
  l2mc_group_api.get_l2mc_group_attribute = &sai_adapter::get_l2mc_group_attribute;
  l2mc_group_api.create_l2mc_group_member = &sai_adapter::create_l2mc_group_member;
  l2mc_group_api.remove_l2mc_group_member = &sai_adapter::remove_l2mc_group_member;
  l2mc_group_api.set_l2mc_group_member_attribute = &sai_adapter::set_l2mc_group_member_attribute;
  l2mc_group_api.get_l2mc_group_member_attribute = &sai_adapter::get_l2mc_group_member_attribute;
  ipmc_group_api.create_ipmc_group = &sai_adapter::create_ipmc_group;
  ipmc_group_api.remove_ipmc_group = &sai_adapter::remove_ipmc_group;
  ipmc_group_api.set_ipmc_group_attribute = &sai_adapter::set_ipmc_group_attribute;
  ipmc_group_api.get_ipmc_group_attribute = &sai_adapter::get_ipmc_group_attribute;
  ipmc_group_api.create_ipmc_group_member = &sai_adapter::create_ipmc_group_member;
  ipmc_group_api.remove_ipmc_group_member = &sai_adapter::remove_ipmc_group_member;
  ipmc_group_api.set_ipmc_group_member_attribute = &sai_adapter::set_ipmc_group_member_attribute;
  ipmc_group_api.get_ipmc_group_member_attribute = &sai_adapter::get_ipmc_group_member_attribute;
  tunnel_api.create_tunnel_map = &sai_adapter::create_tunnel_map;
  tunnel_api.remove_tunnel_map = &sai_adapter::remove_tunnel_map;
  tunnel_api.set_tunnel_map_attribute = &sai_adapter::set_tunnel_map_attribute;
  tunnel_api.get_tunnel_map_attribute = &sai_adapter::get_tunnel_map_attribute;
  tunnel_api.create_tunnel = &sai_adapter::create_tunnel;
  tunnel_api.remove_tunnel = &sai_adapter::remove_tunnel;
  tunnel_api.set_tunnel_attribute = &sai_adapter::set_tunnel_attribute;
  tunnel_api.get_tunnel_attribute = &sai_adapter::get_tunnel_attribute;
  tunnel_api.create_tunnel_term_table_entry = &sai_adapter::create_tunnel_term_table_entry;
  tunnel_api.remove_tunnel_term_table_entry = &sai_adapter::remove_tunnel_term_table_entry;
  tunnel_api.set_tunnel_term_table_entry_attribute = &sai_adapter::set_tunnel_term_table_entry_attribute;
  tunnel_api.get_tunnel_term_table_entry_attribute = &sai_adapter::get_tunnel_term_table_entry_attribute;
  tunnel_api.create_tunnel_map_entry = &sai_adapter::create_tunnel_map_entry;
  tunnel_api.remove_tunnel_map_entry = &sai_adapter::remove_tunnel_map_entry;
  tunnel_api.set_tunnel_map_entry_attribute = &sai_adapter::set_tunnel_map_entry_attribute;
  tunnel_api.get_tunnel_map_entry_attribute = &sai_adapter::get_tunnel_map_entry_attribute;
  l2mc_api.create_l2mc_entry = &sai_adapter::create_l2mc_entry;
  l2mc_api.remove_l2mc_entry = &sai_adapter::remove_l2mc_entry;
  l2mc_api.set_l2mc_entry_attribute = &sai_adapter::set_l2mc_entry_attribute;
  l2mc_api.get_l2mc_entry_attribute = &sai_adapter::get_l2mc_entry_attribute;
  ipmc_api.create_ipmc_entry = &sai_adapter::create_ipmc_entry;
  ipmc_api.remove_ipmc_entry = &sai_adapter::remove_ipmc_entry;
  ipmc_api.set_ipmc_entry_attribute = &sai_adapter::set_ipmc_entry_attribute;
  ipmc_api.get_ipmc_entry_attribute = &sai_adapter::get_ipmc_entry_attribute;
  mcast_fdb_api.create_mcast_fdb_entry = &sai_adapter::create_mcast_fdb_entry;
  mcast_fdb_api.remove_mcast_fdb_entry = &sai_adapter::remove_mcast_fdb_entry;
  mcast_fdb_api.set_mcast_fdb_entry_attribute = &sai_adapter::set_mcast_fdb_entry_attribute;
  mcast_fdb_api.get_mcast_fdb_entry_attribute = &sai_adapter::get_mcast_fdb_entry_attribute;
  buffer_api.create_buffer_pool = &sai_adapter::create_buffer_pool;
  buffer_api.remove_buffer_pool = &sai_adapter::remove_buffer_pool;
  buffer_api.set_buffer_pool_attribute = &sai_adapter::set_buffer_pool_attribute;
  buffer_api.get_buffer_pool_attribute = &sai_adapter::get_buffer_pool_attribute;
  buffer_api.get_buffer_pool_stats = &sai_adapter::get_buffer_pool_stats;
  buffer_api.clear_buffer_pool_stats = &sai_adapter::clear_buffer_pool_stats;
  buffer_api.create_ingress_priority_group = &sai_adapter::create_ingress_priority_group;
  buffer_api.remove_ingress_priority_group = &sai_adapter::remove_ingress_priority_group;
  buffer_api.set_ingress_priority_group_attribute = &sai_adapter::set_ingress_priority_group_attribute;
  buffer_api.get_ingress_priority_group_attribute = &sai_adapter::get_ingress_priority_group_attribute;
  buffer_api.get_ingress_priority_group_stats = &sai_adapter::get_ingress_priority_group_stats;
  buffer_api.clear_ingress_priority_group_stats = &sai_adapter::clear_ingress_priority_group_stats;
  buffer_api.create_buffer_profile = &sai_adapter::create_buffer_profile;
  buffer_api.remove_buffer_profile = &sai_adapter::remove_buffer_profile;
  buffer_api.set_buffer_profile_attribute = &sai_adapter::set_buffer_profile_attribute;
  buffer_api.get_buffer_profile_attribute = &sai_adapter::get_buffer_profile_attribute;
  acl_api.create_acl_table = &sai_adapter::create_acl_table;
  acl_api.remove_acl_table = &sai_adapter::remove_acl_table;
  acl_api.set_acl_table_attribute = &sai_adapter::set_acl_table_attribute;
  acl_api.get_acl_table_attribute = &sai_adapter::get_acl_table_attribute;
  acl_api.create_acl_entry = &sai_adapter::create_acl_entry;
  acl_api.remove_acl_entry = &sai_adapter::remove_acl_entry;
  acl_api.set_acl_entry_attribute = &sai_adapter::set_acl_entry_attribute;
  acl_api.get_acl_entry_attribute = &sai_adapter::get_acl_entry_attribute;
  acl_api.create_acl_counter = &sai_adapter::create_acl_counter;
  acl_api.remove_acl_counter = &sai_adapter::remove_acl_counter;
  acl_api.set_acl_counter_attribute = &sai_adapter::set_acl_counter_attribute;
  acl_api.get_acl_counter_attribute = &sai_adapter::get_acl_counter_attribute;
  acl_api.create_acl_range = &sai_adapter::create_acl_range;
  acl_api.remove_acl_range = &sai_adapter::remove_acl_range;
  acl_api.set_acl_range_attribute = &sai_adapter::set_acl_range_attribute;
  acl_api.get_acl_range_attribute = &sai_adapter::get_acl_range_attribute;
  acl_api.create_acl_table_group = &sai_adapter::create_acl_table_group;
  acl_api.remove_acl_table_group = &sai_adapter::remove_acl_table_group;
  acl_api.set_acl_table_group_attribute = &sai_adapter::set_acl_table_group_attribute;
  acl_api.get_acl_table_group_attribute = &sai_adapter::get_acl_table_group_attribute;
  acl_api.create_acl_table_group_member = &sai_adapter::create_acl_table_group_member;
  acl_api.remove_acl_table_group_member = &sai_adapter::remove_acl_table_group_member;
  acl_api.set_acl_table_group_member_attribute = &sai_adapter::set_acl_table_group_member_attribute;
  acl_api.get_acl_table_group_member_attribute = &sai_adapter::get_acl_table_group_member_attribute;

  startSaiAdapterMain();
  (*logger)->info("BM connection started on port {}", bm_port);
}

sai_adapter::~sai_adapter() {
  endSaiAdapterMain();
  transport->close();
  (*logger)->info("BM clients closed\n");
}

sai_status_t sai_adapter::sai_api_query(sai_api_t sai_api_id,
                                        void **api_method_table) {
  switch (sai_api_id) {
  case SAI_API_PORT:
    *api_method_table = &port_api;
    break;
  case SAI_API_BRIDGE:
    *api_method_table = &bridge_api;
    break;
  case SAI_API_FDB:
    *api_method_table = &fdb_api;
    break;
  case SAI_API_SWITCH:
    *api_method_table = &switch_api;
    break;
  case SAI_API_VLAN:
    *api_method_table = &vlan_api;
    break;
  case SAI_API_LAG:
    *api_method_table = &lag_api;
    break;
  case SAI_API_HOSTIF:
    *api_method_table = &hostif_api;
    break;
  case SAI_API_VIRTUAL_ROUTER:
    *api_method_table = &virtual_router_api;
    break;
  case SAI_API_ROUTE:
    *api_method_table = &route_api;
    break;
  case SAI_API_NEXT_HOP:
    *api_method_table = &next_hop_api;
    break;
  case SAI_API_NEXT_HOP_GROUP:
    *api_method_table = &next_hop_group_api;
    break;
  case SAI_API_ROUTER_INTERFACE:
    *api_method_table = &router_interface_api;
    break;
  case SAI_API_NEIGHBOR:
    *api_method_table = &neighbor_api;
    break;
  case SAI_API_ACL:
    *api_method_table = &acl_api;
    break;
  case SAI_API_MIRROR:
    *api_method_table = &mirror_api;
    break;
  case SAI_API_SAMPLEPACKET:
    *api_method_table = &samplepacket_api;
    break;
  case SAI_API_STP:
    *api_method_table = &stp_api;
    break;
  case SAI_API_POLICER:
    *api_method_table = &policer_api;
    break;
  case SAI_API_WRED:
    *api_method_table = &wred_api;
    break;
  case SAI_API_QOS_MAP:
    *api_method_table = &qos_map_api;
    break;
  case SAI_API_QUEUE:
    *api_method_table = &queue_api;
    break;
  case SAI_API_SCHEDULER:
    *api_method_table = &scheduler_api;
    break;
  case SAI_API_SCHEDULER_GROUP:
    *api_method_table = &scheduler_group_api;
    break;
  case SAI_API_BUFFER:
    *api_method_table = &buffer_api;
    break;
  case SAI_API_HASH:
    *api_method_table = &hash_api;
    break;
  case SAI_API_UDF:
    *api_method_table = &udf_api;
    break;
  case SAI_API_TUNNEL:
    *api_method_table = &tunnel_api;
    break;
  case SAI_API_L2MC:
    *api_method_table = &l2mc_api;
    break;
  case SAI_API_IPMC:
    *api_method_table = &ipmc_api;
    break;
  case SAI_API_RPF_GROUP:
    *api_method_table = &rpf_group_api;
    break;
  case SAI_API_L2MC_GROUP:
    *api_method_table = &l2mc_group_api;
    break;
  case SAI_API_IPMC_GROUP:
    *api_method_table = &ipmc_group_api;
    break;
  case SAI_API_MCAST_FDB:
    *api_method_table = &mcast_fdb_api;
    break;
  default:
    (*logger)->info("unsupported api request made ({})", sai_api_id);
    return SAI_STATUS_FAILURE;
  }
  return SAI_STATUS_SUCCESS;
}

sai_object_type_t sai_adapter::_sai_object_type_query(sai_object_id_t sai_object_id) {
  if (switch_metadata_ptr->switch_id == sai_object_id) {
    return SAI_OBJECT_TYPE_SWITCH;
  }
  if (switch_metadata_ptr->cpu_port_id == sai_object_id) {
    return SAI_OBJECT_TYPE_PORT;
  }
  if (switch_metadata_ptr->ports.find(sai_object_id) != switch_metadata_ptr->ports.end()) {
    return SAI_OBJECT_TYPE_PORT;
  }
  if (switch_metadata_ptr->bridge_ports.find(sai_object_id) != switch_metadata_ptr->bridge_ports.end()) {
    return SAI_OBJECT_TYPE_BRIDGE_PORT;
  }
  if (switch_metadata_ptr->bridges.find(sai_object_id) != switch_metadata_ptr->bridges.end()) {
    return SAI_OBJECT_TYPE_BRIDGE;
  }
  if (switch_metadata_ptr->vlans.find(sai_object_id) != switch_metadata_ptr->vlans.end()) {
    return SAI_OBJECT_TYPE_VLAN;
  }
  if (switch_metadata_ptr->vlan_members.find(sai_object_id) != switch_metadata_ptr->vlan_members.end()) {
    return SAI_OBJECT_TYPE_VLAN_MEMBER;
  }
  if (switch_metadata_ptr->lags.find(sai_object_id) != switch_metadata_ptr->lags.end()) {
    return SAI_OBJECT_TYPE_LAG;
  }
  if (switch_metadata_ptr->lag_members.find(sai_object_id) != switch_metadata_ptr->lag_members.end()) {
    return SAI_OBJECT_TYPE_LAG_MEMBER;
  }
  if (switch_metadata_ptr->hostifs.find(sai_object_id) != switch_metadata_ptr->hostifs.end()) {
    return SAI_OBJECT_TYPE_HOSTIF;
  }
  if (switch_metadata_ptr->hostif_table_entries.find(sai_object_id) != switch_metadata_ptr->hostif_table_entries.end()) {
    return SAI_OBJECT_TYPE_HOSTIF_TABLE_ENTRY;
  }
  if (switch_metadata_ptr->hostif_traps.find(sai_object_id) != switch_metadata_ptr->hostif_traps.end()) {
    return SAI_OBJECT_TYPE_HOSTIF_TRAP;
  }
  if (switch_metadata_ptr->hostif_trap_groups.find(sai_object_id) != switch_metadata_ptr->hostif_trap_groups.end()) {
    return SAI_OBJECT_TYPE_HOSTIF_TRAP_GROUP;
  }
  if (switch_metadata_ptr->vrs.find(sai_object_id) != switch_metadata_ptr->vrs.end()) {
    return SAI_OBJECT_TYPE_VIRTUAL_ROUTER;
  }
  if (switch_metadata_ptr->rifs.find(sai_object_id) != switch_metadata_ptr->rifs.end()) {
    return SAI_OBJECT_TYPE_ROUTER_INTERFACE;
  }
  if (switch_metadata_ptr->nhops.find(sai_object_id) != switch_metadata_ptr->nhops.end()) {
    return SAI_OBJECT_TYPE_NEXT_HOP;
  }
  return SAI_OBJECT_TYPE_NULL;
}

void sai_adapter::startSaiAdapterMain() {
  // internal_init_switch();
  pcap_loop_started = false;
  SaiAdapterThread = std::thread(&sai_adapter::SaiAdapterMain, this);
  std::unique_lock<std::mutex> lk(m);
  cv.wait(lk, [] { return pcap_loop_started; });
  (*logger)->info("Sniffer initialization done");
}

void sai_adapter::endSaiAdapterMain() {
  (*logger)->info("endSaiAdapterMain");
  write(sniff_pipe_fd[1], "c", 1);
  SaiAdapterThread.join();
}

void sai_adapter::SaiAdapterMain() {
  (*logger)->info("SAI Adapter Thread Started");
  PacketSniffer();
  (*logger)->info("SAI Adapter Thread Ended");
}

std::string parse_param(uint64_t param, uint32_t num_of_bytes) {
  std::string my_string = std::string(
      static_cast<char *>(static_cast<void *>(&param)), num_of_bytes);
  std::reverse(my_string.begin(), my_string.end());
  return my_string;
}

BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes) {
  BmMatchParam match_param;
  match_param.type = BmMatchParamType::type::EXACT;
  BmMatchParamExact match_param_exact;
  match_param_exact.__set_key(parse_param(param, num_of_bytes));
  match_param.__set_exact(match_param_exact);
  return match_param;
}

BmMatchParam parse_lpm_param(uint64_t param, uint32_t num_of_bytes, uint32_t prefix_length) {
  BmMatchParam match_param;
  match_param.type = BmMatchParamType::type::LPM;
  BmMatchParamLPM match_param_lpm;
  match_param_lpm.__set_key(parse_param(param, num_of_bytes));
  match_param_lpm.__set_prefix_length(prefix_length);
  match_param.__set_lpm(match_param_lpm);
  return match_param;
}

BmMatchParam parse_ternary_param(uint64_t param, uint32_t num_of_bytes, uint64_t mask) {
  BmMatchParam match_param;
  match_param.type = BmMatchParamType::type::TERNARY;
  BmMatchParamTernary match_param_ternary;
  match_param_ternary.__set_key(parse_param(param, num_of_bytes));
  match_param_ternary.__set_mask(parse_param(mask, num_of_bytes));
  match_param.__set_ternary(match_param_ternary);
  return match_param;
}

uint64_t parse_mac_64(uint8_t const mac_8[6]) {
  uint64_t mac_64 = 0;
  memcpy(&mac_64, mac_8, 6);
  return mac_64;
}