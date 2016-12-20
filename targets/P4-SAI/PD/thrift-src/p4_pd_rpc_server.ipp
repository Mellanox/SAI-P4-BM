
#include "p4_prefix.h"

#include <iostream>

#include <string.h>

#include "pd/pd.h"

#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>

using namespace  ::p4_pd_rpc;
using namespace  ::res_pd_rpc;

namespace {

void bytes_meter_spec_thrift_to_pd(
    const prog_bytes_meter_spec_t &meter_spec,
    p4_pd_bytes_meter_spec_t *pd_meter_spec) {
  pd_meter_spec->cir_kbps = meter_spec.cir_kbps;
  pd_meter_spec->cburst_kbits = meter_spec.cburst_kbits;
  pd_meter_spec->pir_kbps = meter_spec.pir_kbps;
  pd_meter_spec->pburst_kbits = meter_spec.pburst_kbits;
  pd_meter_spec->meter_type = meter_spec.color_aware ?
      PD_METER_TYPE_COLOR_AWARE : PD_METER_TYPE_COLOR_UNAWARE;
}

void packets_meter_spec_thrift_to_pd(
    const prog_packets_meter_spec_t &meter_spec,
    p4_pd_packets_meter_spec_t *pd_meter_spec) {
  pd_meter_spec->cir_pps = meter_spec.cir_pps;
  pd_meter_spec->cburst_pkts = meter_spec.cburst_pkts;
  pd_meter_spec->pir_pps = meter_spec.pir_pps;
  pd_meter_spec->pburst_pkts = meter_spec.pburst_pkts;
   pd_meter_spec->meter_type = meter_spec.color_aware ?
       PD_METER_TYPE_COLOR_AWARE : PD_METER_TYPE_COLOR_UNAWARE;
}

}  // namespace


class progHandler : virtual public progIf {
private:
  class CbWrap {
    CbWrap() {}

    int wait() {
      std::unique_lock<std::mutex> lock(cb_mutex);
      while(cb_status == 0) {
        cb_condvar.wait(lock);
      }
      return 0;
    }

    void notify() {
      std::unique_lock<std::mutex> lock(cb_mutex);
      assert(cb_status == 0);
      cb_status = 1;
      cb_condvar.notify_one();
    }

    static void cb_fn(int device_id, void *cookie) {
      (void) device_id;
      CbWrap *inst = static_cast<CbWrap *>(cookie);
      inst->notify();
    }

    CbWrap(const CbWrap &other) = delete;
    CbWrap &operator=(const CbWrap &other) = delete;

    CbWrap(CbWrap &&other) = delete;
    CbWrap &operator=(CbWrap &&other) = delete;

   private:
    std::mutex cb_mutex{};
    std::condition_variable cb_condvar{};
    int cb_status{0};
  };

public:
    progHandler() {
      pthread_mutex_init(&mac_learn_digest_mutex, NULL);
    }

    // Table entry add functions

    EntryHandle_t table_xSTP_instance_table_add_with_action_set_stp_id(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_xSTP_instance_match_spec_t &match_spec, const prog_action_set_stp_id_action_spec_t &action_spec) {
        std::cerr << "In table_xSTP_instance_table_add_with_action_set_stp_id\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_xSTP_instance_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_vid = match_spec.ingress_metadata_vid;

        p4_pd_prog_action_set_stp_id_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_id = action_spec.action_stp_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_xSTP_instance_table_add_with_action_set_stp_id(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_ingress_lag_table_add_with_action_set_lag_l2if(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_ingress_lag_match_spec_t &match_spec, const prog_action_set_lag_l2if_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_lag_table_add_with_action_set_lag_l2if\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_ingress_lag_match_spec_t pd_match_spec;
        pd_match_spec.standard_metadata_ingress_port = match_spec.standard_metadata_ingress_port;

        p4_pd_prog_action_set_lag_l2if_action_spec_t pd_action_spec;
        pd_action_spec.action_is_lag = action_spec.action_is_lag;
        pd_action_spec.action_lag_id = action_spec.action_lag_id;
        pd_action_spec.action_l2_if = action_spec.action_l2_if;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_ingress_lag_table_add_with_action_set_lag_l2if(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_mc_fdb_table_add_with_action_set_mc_fdb_miss(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_mc_fdb_match_spec_t &match_spec) {
        std::cerr << "In table_mc_fdb_table_add_with_action_set_mc_fdb_miss\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_mc_fdb_match_spec_t pd_match_spec;
	memcpy(pd_match_spec.ethernet_dstAddr, match_spec.ethernet_dstAddr.c_str(), 6);
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_mc_fdb_table_add_with_action_set_mc_fdb_miss(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_mc_fdb_table_add_with_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_mc_fdb_match_spec_t &match_spec) {
        std::cerr << "In table_mc_fdb_table_add_with_action_forward_mc_set_if_list\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_mc_fdb_match_spec_t pd_match_spec;
	memcpy(pd_match_spec.ethernet_dstAddr, match_spec.ethernet_dstAddr.c_str(), 6);
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_mc_fdb_table_add_with_action_forward_mc_set_if_list(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_mc_l2_sg_g_table_add_with_action_set_mc_fdb_miss(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_mc_l2_sg_g_match_spec_t &match_spec) {
        std::cerr << "In table_mc_l2_sg_g_table_add_with_action_set_mc_fdb_miss\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_mc_l2_sg_g_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;
        pd_match_spec.ipv4_srcAddr = match_spec.ipv4_srcAddr;
        pd_match_spec.ipv4_dstAddr = match_spec.ipv4_dstAddr;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_mc_l2_sg_g_table_add_with_action_set_mc_fdb_miss(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_mc_l2_sg_g_table_add_with_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_mc_l2_sg_g_match_spec_t &match_spec) {
        std::cerr << "In table_mc_l2_sg_g_table_add_with_action_forward_mc_set_if_list\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_mc_l2_sg_g_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;
        pd_match_spec.ipv4_srcAddr = match_spec.ipv4_srcAddr;
        pd_match_spec.ipv4_dstAddr = match_spec.ipv4_dstAddr;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_mc_l2_sg_g_table_add_with_action_forward_mc_set_if_list(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_learn_fdb_table_add_with_action_learn_mac(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_learn_fdb_match_spec_t &match_spec) {
        std::cerr << "In table_learn_fdb_table_add_with_action_learn_mac\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_learn_fdb_match_spec_t pd_match_spec;
	memcpy(pd_match_spec.ethernet_srcAddr, match_spec.ethernet_srcAddr.c_str(), 6);
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_learn_fdb_table_add_with_action_learn_mac(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_learn_fdb_table_add_with__nop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_learn_fdb_match_spec_t &match_spec) {
        std::cerr << "In table_learn_fdb_table_add_with__nop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_learn_fdb_match_spec_t pd_match_spec;
	memcpy(pd_match_spec.ethernet_srcAddr, match_spec.ethernet_srcAddr.c_str(), 6);
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_learn_fdb_table_add_with__nop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_lag_hash_table_add_with_action_set_lag_hash_size(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_lag_hash_match_spec_t &match_spec, const prog_action_set_lag_hash_size_action_spec_t &action_spec) {
        std::cerr << "In table_lag_hash_table_add_with_action_set_lag_hash_size\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_lag_hash_match_spec_t pd_match_spec;
        pd_match_spec.egress_metadata_out_if = match_spec.egress_metadata_out_if;

        p4_pd_prog_action_set_lag_hash_size_action_spec_t pd_action_spec;
        pd_action_spec.action_lag_size = action_spec.action_lag_size;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_lag_hash_table_add_with_action_set_lag_hash_size(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_lag_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_lag_match_spec_t &match_spec) {
        std::cerr << "In table_egress_lag_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_lag_match_spec_t pd_match_spec;
        pd_match_spec.egress_metadata_out_if = match_spec.egress_metadata_out_if;
        pd_match_spec.egress_metadata_hash_val = match_spec.egress_metadata_hash_val;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_lag_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_lag_table_add_with_action_set_out_port(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_lag_match_spec_t &match_spec, const prog_action_set_out_port_action_spec_t &action_spec) {
        std::cerr << "In table_egress_lag_table_add_with_action_set_out_port\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_lag_match_spec_t pd_match_spec;
        pd_match_spec.egress_metadata_out_if = match_spec.egress_metadata_out_if;
        pd_match_spec.egress_metadata_hash_val = match_spec.egress_metadata_hash_val;

        p4_pd_prog_action_set_out_port_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_lag_table_add_with_action_set_out_port(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_vlan_filtering_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_vlan_filtering_match_spec_t &match_spec) {
        std::cerr << "In table_egress_vlan_filtering_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_vlan_filtering_match_spec_t pd_match_spec;
        pd_match_spec.egress_metadata_out_if = match_spec.egress_metadata_out_if;
        pd_match_spec.ingress_metadata_vid = match_spec.ingress_metadata_vid;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_vlan_filtering_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_vbridge_STP_table_add_with_action_set_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_vbridge_STP_match_spec_t &match_spec, const prog_action_set_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_vbridge_STP_table_add_with_action_set_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_vbridge_STP_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;

        p4_pd_prog_action_set_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_vbridge_STP_table_add_with_action_set_stp_state(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_ingress_l2_interface_type_table_add_with_action_set_l2_if_type(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_ingress_l2_interface_type_match_spec_t &match_spec, const prog_action_set_l2_if_type_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_l2_interface_type_table_add_with_action_set_l2_if_type\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_ingress_l2_interface_type_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_l2_if = match_spec.ingress_metadata_l2_if;
        pd_match_spec.ingress_metadata_vid = match_spec.ingress_metadata_vid;

        p4_pd_prog_action_set_l2_if_type_action_spec_t pd_action_spec;
        pd_action_spec.action_l2_if_type = action_spec.action_l2_if_type;
        pd_action_spec.action_bridge_port = action_spec.action_bridge_port;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_ingress_l2_interface_type_table_add_with_action_set_l2_if_type(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_ingress_l2_interface_type_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_ingress_l2_interface_type_match_spec_t &match_spec) {
        std::cerr << "In table_ingress_l2_interface_type_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_ingress_l2_interface_type_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_l2_if = match_spec.ingress_metadata_l2_if;
        pd_match_spec.ingress_metadata_vid = match_spec.ingress_metadata_vid;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_ingress_l2_interface_type_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_vbridge_STP_table_add_with_action_set_egress_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_vbridge_STP_match_spec_t &match_spec, const prog_action_set_egress_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_egress_vbridge_STP_table_add_with_action_set_egress_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_vbridge_STP_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;

        p4_pd_prog_action_set_egress_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_vbridge_STP_table_add_with_action_set_egress_stp_state(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_vbridge_STP_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_vbridge_STP_match_spec_t &match_spec) {
        std::cerr << "In table_egress_vbridge_STP_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_vbridge_STP_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_vbridge_STP_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_unknown_unicast_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_unknown_unicast_match_spec_t &match_spec) {
        std::cerr << "In table_unknown_unicast_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_unknown_unicast_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_unknown_unicast_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_unknown_unicast_table_add_with_action_forward(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_unknown_unicast_match_spec_t &match_spec, const prog_action_forward_action_spec_t &action_spec) {
        std::cerr << "In table_unknown_unicast_table_add_with_action_forward\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_unknown_unicast_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_prog_action_forward_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_unknown_unicast_table_add_with_action_forward(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_l3_if_table_add_with_action_forward(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_l3_if_match_spec_t &match_spec, const prog_action_forward_action_spec_t &action_spec) {
        std::cerr << "In table_l3_if_table_add_with_action_forward\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_l3_if_match_spec_t pd_match_spec;
	memcpy(pd_match_spec.ethernet_dstAddr, match_spec.ethernet_dstAddr.c_str(), 6);
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_prog_action_forward_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_l3_if_table_add_with_action_forward(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_unknown_multicast_table_add_with_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_unknown_multicast_match_spec_t &match_spec) {
        std::cerr << "In table_unknown_multicast_table_add_with_action_forward_mc_set_if_list\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_unknown_multicast_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_unknown_multicast_table_add_with_action_forward_mc_set_if_list(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_fdb_table_add_with_action_forward_set_outIfType(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_fdb_match_spec_t &match_spec, const prog_action_forward_set_outIfType_action_spec_t &action_spec) {
        std::cerr << "In table_fdb_table_add_with_action_forward_set_outIfType\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_fdb_match_spec_t pd_match_spec;
	memcpy(pd_match_spec.ethernet_dstAddr, match_spec.ethernet_dstAddr.c_str(), 6);
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_prog_action_forward_set_outIfType_action_spec_t pd_action_spec;
        pd_action_spec.action_out_if = action_spec.action_out_if;
        pd_action_spec.action_out_if_type = action_spec.action_out_if_type;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_fdb_table_add_with_action_forward_set_outIfType(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_xSTP_table_add_with_action_set_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_xSTP_match_spec_t &match_spec, const prog_action_set_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_xSTP_table_add_with_action_set_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_xSTP_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;
        pd_match_spec.ingress_metadata_stp_id = match_spec.ingress_metadata_stp_id;

        p4_pd_prog_action_set_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_xSTP_table_add_with_action_set_stp_state(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_ingress_vlan_table_add_with_action_set_mcast_snp(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_ingress_vlan_match_spec_t &match_spec, const prog_action_set_mcast_snp_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_vlan_table_add_with_action_set_mcast_snp\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_ingress_vlan_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_vid = match_spec.ingress_metadata_vid;

        p4_pd_prog_action_set_mcast_snp_action_spec_t pd_action_spec;
        pd_action_spec.action_mcast_snp = action_spec.action_mcast_snp;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_ingress_vlan_table_add_with_action_set_mcast_snp(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_xSTP_table_add_with_action_set_egress_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_xSTP_match_spec_t &match_spec, const prog_action_set_egress_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_egress_xSTP_table_add_with_action_set_egress_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_xSTP_match_spec_t pd_match_spec;
        pd_match_spec.egress_metadata_out_if = match_spec.egress_metadata_out_if;
        pd_match_spec.ingress_metadata_stp_id = match_spec.ingress_metadata_stp_id;

        p4_pd_prog_action_set_egress_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_xSTP_table_add_with_action_set_egress_stp_state(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_xSTP_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_xSTP_match_spec_t &match_spec) {
        std::cerr << "In table_egress_xSTP_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_xSTP_match_spec_t pd_match_spec;
        pd_match_spec.egress_metadata_out_if = match_spec.egress_metadata_out_if;
        pd_match_spec.ingress_metadata_stp_id = match_spec.ingress_metadata_stp_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_xSTP_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_ingress_vlan_filtering_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_ingress_vlan_filtering_match_spec_t &match_spec) {
        std::cerr << "In table_ingress_vlan_filtering_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_ingress_vlan_filtering_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_l2_if = match_spec.ingress_metadata_l2_if;
        pd_match_spec.ingress_metadata_vid = match_spec.ingress_metadata_vid;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_ingress_vlan_filtering_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_ingress_vlan_filtering_table_add_with__nop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_ingress_vlan_filtering_match_spec_t &match_spec) {
        std::cerr << "In table_ingress_vlan_filtering_table_add_with__nop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_ingress_vlan_filtering_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_l2_if = match_spec.ingress_metadata_l2_if;
        pd_match_spec.ingress_metadata_vid = match_spec.ingress_metadata_vid;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_ingress_vlan_filtering_table_add_with__nop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_accepted_frame_type_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_accepted_frame_type_match_spec_t &match_spec) {
        std::cerr << "In table_accepted_frame_type_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_accepted_frame_type_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_l2_if = match_spec.ingress_metadata_l2_if;
        pd_match_spec.vlan_valid = match_spec.vlan_valid;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_accepted_frame_type_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_accepted_frame_type_table_add_with_action_set_packet_vid(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_accepted_frame_type_match_spec_t &match_spec) {
        std::cerr << "In table_accepted_frame_type_table_add_with_action_set_packet_vid\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_accepted_frame_type_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_l2_if = match_spec.ingress_metadata_l2_if;
        pd_match_spec.vlan_valid = match_spec.vlan_valid;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_accepted_frame_type_table_add_with_action_set_packet_vid(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_accepted_frame_type_table_add_with_action_set_pvid(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_accepted_frame_type_match_spec_t &match_spec, const prog_action_set_pvid_action_spec_t &action_spec) {
        std::cerr << "In table_accepted_frame_type_table_add_with_action_set_pvid\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_accepted_frame_type_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_l2_if = match_spec.ingress_metadata_l2_if;
        pd_match_spec.vlan_valid = match_spec.vlan_valid;

        p4_pd_prog_action_set_pvid_action_spec_t pd_action_spec;
        pd_action_spec.action_pvid = action_spec.action_pvid;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_accepted_frame_type_table_add_with_action_set_pvid(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_vbridge_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_vbridge_match_spec_t &match_spec) {
        std::cerr << "In table_egress_vbridge_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_vbridge_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_vbridge_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_egress_vbridge_table_add_with_action_set_vlan_tag_mode(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_egress_vbridge_match_spec_t &match_spec, const prog_action_set_vlan_tag_mode_action_spec_t &action_spec) {
        std::cerr << "In table_egress_vbridge_table_add_with_action_set_vlan_tag_mode\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_egress_vbridge_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;

        p4_pd_prog_action_set_vlan_tag_mode_action_spec_t pd_action_spec;
        pd_action_spec.action_pcp = action_spec.action_pcp;
        pd_action_spec.action_cfi = action_spec.action_cfi;
        pd_action_spec.action_vid = action_spec.action_vid;
        pd_action_spec.action_ethType = action_spec.action_ethType;
        pd_action_spec.action_tag_mode = action_spec.action_tag_mode;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_egress_vbridge_table_add_with_action_set_vlan_tag_mode(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_vbridge_table_add_with__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_vbridge_match_spec_t &match_spec) {
        std::cerr << "In table_vbridge_table_add_with__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_vbridge_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_vbridge_table_add_with__drop(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_vbridge_table_add_with_action_set_bridge_id(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_vbridge_match_spec_t &match_spec, const prog_action_set_bridge_id_action_spec_t &action_spec) {
        std::cerr << "In table_vbridge_table_add_with_action_set_bridge_id\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_vbridge_match_spec_t pd_match_spec;
        pd_match_spec.ingress_metadata_bridge_port = match_spec.ingress_metadata_bridge_port;

        p4_pd_prog_action_set_bridge_id_action_spec_t pd_action_spec;
        pd_action_spec.action_bridge_id = action_spec.action_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_vbridge_table_add_with_action_set_bridge_id(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_action_spec, &pd_entry);
        return pd_entry;
    }

    EntryHandle_t table_l3_interface_table_add_with_action_go_to_in_l3_if_table(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_table_l3_interface_match_spec_t &match_spec) {
        std::cerr << "In table_l3_interface_table_add_with_action_go_to_in_l3_if_table\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_table_l3_interface_match_spec_t pd_match_spec;
	memcpy(pd_match_spec.ethernet_dstAddr, match_spec.ethernet_dstAddr.c_str(), 6);
        pd_match_spec.ingress_metadata_bridge_id = match_spec.ingress_metadata_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        p4_pd_prog_table_l3_interface_table_add_with_action_go_to_in_l3_if_table(sess_hdl, pd_dev_tgt, &pd_match_spec, &pd_entry);
        return pd_entry;
    }



    // Table entry modify functions

    EntryHandle_t table_xSTP_instance_table_modify_with_action_set_stp_id(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_stp_id_action_spec_t &action_spec) {
        std::cerr << "In table_xSTP_instance_table_modify_with_action_set_stp_id\n";

        p4_pd_prog_action_set_stp_id_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_id = action_spec.action_stp_id;


        return p4_pd_prog_table_xSTP_instance_table_modify_with_action_set_stp_id(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_ingress_lag_table_modify_with_action_set_lag_l2if(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_lag_l2if_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_lag_table_modify_with_action_set_lag_l2if\n";

        p4_pd_prog_action_set_lag_l2if_action_spec_t pd_action_spec;
        pd_action_spec.action_is_lag = action_spec.action_is_lag;
        pd_action_spec.action_lag_id = action_spec.action_lag_id;
        pd_action_spec.action_l2_if = action_spec.action_l2_if;


        return p4_pd_prog_table_ingress_lag_table_modify_with_action_set_lag_l2if(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_mc_fdb_table_modify_with_action_set_mc_fdb_miss(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_mc_fdb_table_modify_with_action_set_mc_fdb_miss\n";


        return p4_pd_prog_table_mc_fdb_table_modify_with_action_set_mc_fdb_miss(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_mc_fdb_table_modify_with_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_mc_fdb_table_modify_with_action_forward_mc_set_if_list\n";


        return p4_pd_prog_table_mc_fdb_table_modify_with_action_forward_mc_set_if_list(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_mc_l2_sg_g_table_modify_with_action_set_mc_fdb_miss(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_mc_l2_sg_g_table_modify_with_action_set_mc_fdb_miss\n";


        return p4_pd_prog_table_mc_l2_sg_g_table_modify_with_action_set_mc_fdb_miss(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_mc_l2_sg_g_table_modify_with_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_mc_l2_sg_g_table_modify_with_action_forward_mc_set_if_list\n";


        return p4_pd_prog_table_mc_l2_sg_g_table_modify_with_action_forward_mc_set_if_list(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_learn_fdb_table_modify_with_action_learn_mac(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_learn_fdb_table_modify_with_action_learn_mac\n";


        return p4_pd_prog_table_learn_fdb_table_modify_with_action_learn_mac(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_learn_fdb_table_modify_with__nop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_learn_fdb_table_modify_with__nop\n";


        return p4_pd_prog_table_learn_fdb_table_modify_with__nop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_lag_hash_table_modify_with_action_set_lag_hash_size(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_lag_hash_size_action_spec_t &action_spec) {
        std::cerr << "In table_lag_hash_table_modify_with_action_set_lag_hash_size\n";

        p4_pd_prog_action_set_lag_hash_size_action_spec_t pd_action_spec;
        pd_action_spec.action_lag_size = action_spec.action_lag_size;


        return p4_pd_prog_table_lag_hash_table_modify_with_action_set_lag_hash_size(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_egress_lag_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_lag_table_modify_with__drop\n";


        return p4_pd_prog_table_egress_lag_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_egress_lag_table_modify_with_action_set_out_port(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_out_port_action_spec_t &action_spec) {
        std::cerr << "In table_egress_lag_table_modify_with_action_set_out_port\n";

        p4_pd_prog_action_set_out_port_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;


        return p4_pd_prog_table_egress_lag_table_modify_with_action_set_out_port(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_egress_vlan_filtering_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_vlan_filtering_table_modify_with__drop\n";


        return p4_pd_prog_table_egress_vlan_filtering_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_vbridge_STP_table_modify_with_action_set_stp_state(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_vbridge_STP_table_modify_with_action_set_stp_state\n";

        p4_pd_prog_action_set_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;


        return p4_pd_prog_table_vbridge_STP_table_modify_with_action_set_stp_state(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_ingress_l2_interface_type_table_modify_with_action_set_l2_if_type(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_l2_if_type_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_l2_interface_type_table_modify_with_action_set_l2_if_type\n";

        p4_pd_prog_action_set_l2_if_type_action_spec_t pd_action_spec;
        pd_action_spec.action_l2_if_type = action_spec.action_l2_if_type;
        pd_action_spec.action_bridge_port = action_spec.action_bridge_port;


        return p4_pd_prog_table_ingress_l2_interface_type_table_modify_with_action_set_l2_if_type(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_ingress_l2_interface_type_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_ingress_l2_interface_type_table_modify_with__drop\n";


        return p4_pd_prog_table_ingress_l2_interface_type_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_egress_vbridge_STP_table_modify_with_action_set_egress_stp_state(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_egress_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_egress_vbridge_STP_table_modify_with_action_set_egress_stp_state\n";

        p4_pd_prog_action_set_egress_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;


        return p4_pd_prog_table_egress_vbridge_STP_table_modify_with_action_set_egress_stp_state(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_egress_vbridge_STP_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_vbridge_STP_table_modify_with__drop\n";


        return p4_pd_prog_table_egress_vbridge_STP_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_unknown_unicast_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_unknown_unicast_table_modify_with__drop\n";


        return p4_pd_prog_table_unknown_unicast_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_unknown_unicast_table_modify_with_action_forward(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_forward_action_spec_t &action_spec) {
        std::cerr << "In table_unknown_unicast_table_modify_with_action_forward\n";

        p4_pd_prog_action_forward_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;


        return p4_pd_prog_table_unknown_unicast_table_modify_with_action_forward(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_l3_if_table_modify_with_action_forward(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_forward_action_spec_t &action_spec) {
        std::cerr << "In table_l3_if_table_modify_with_action_forward\n";

        p4_pd_prog_action_forward_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;


        return p4_pd_prog_table_l3_if_table_modify_with_action_forward(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_unknown_multicast_table_modify_with_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_unknown_multicast_table_modify_with_action_forward_mc_set_if_list\n";


        return p4_pd_prog_table_unknown_multicast_table_modify_with_action_forward_mc_set_if_list(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_fdb_table_modify_with_action_forward_set_outIfType(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_forward_set_outIfType_action_spec_t &action_spec) {
        std::cerr << "In table_fdb_table_modify_with_action_forward_set_outIfType\n";

        p4_pd_prog_action_forward_set_outIfType_action_spec_t pd_action_spec;
        pd_action_spec.action_out_if = action_spec.action_out_if;
        pd_action_spec.action_out_if_type = action_spec.action_out_if_type;


        return p4_pd_prog_table_fdb_table_modify_with_action_forward_set_outIfType(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_xSTP_table_modify_with_action_set_stp_state(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_xSTP_table_modify_with_action_set_stp_state\n";

        p4_pd_prog_action_set_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;


        return p4_pd_prog_table_xSTP_table_modify_with_action_set_stp_state(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_ingress_vlan_table_modify_with_action_set_mcast_snp(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_mcast_snp_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_vlan_table_modify_with_action_set_mcast_snp\n";

        p4_pd_prog_action_set_mcast_snp_action_spec_t pd_action_spec;
        pd_action_spec.action_mcast_snp = action_spec.action_mcast_snp;


        return p4_pd_prog_table_ingress_vlan_table_modify_with_action_set_mcast_snp(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_egress_xSTP_table_modify_with_action_set_egress_stp_state(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_egress_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_egress_xSTP_table_modify_with_action_set_egress_stp_state\n";

        p4_pd_prog_action_set_egress_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;


        return p4_pd_prog_table_egress_xSTP_table_modify_with_action_set_egress_stp_state(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_egress_xSTP_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_xSTP_table_modify_with__drop\n";


        return p4_pd_prog_table_egress_xSTP_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_ingress_vlan_filtering_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_ingress_vlan_filtering_table_modify_with__drop\n";


        return p4_pd_prog_table_ingress_vlan_filtering_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_ingress_vlan_filtering_table_modify_with__nop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_ingress_vlan_filtering_table_modify_with__nop\n";


        return p4_pd_prog_table_ingress_vlan_filtering_table_modify_with__nop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_accepted_frame_type_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_accepted_frame_type_table_modify_with__drop\n";


        return p4_pd_prog_table_accepted_frame_type_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_accepted_frame_type_table_modify_with_action_set_packet_vid(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_accepted_frame_type_table_modify_with_action_set_packet_vid\n";


        return p4_pd_prog_table_accepted_frame_type_table_modify_with_action_set_packet_vid(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_accepted_frame_type_table_modify_with_action_set_pvid(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_pvid_action_spec_t &action_spec) {
        std::cerr << "In table_accepted_frame_type_table_modify_with_action_set_pvid\n";

        p4_pd_prog_action_set_pvid_action_spec_t pd_action_spec;
        pd_action_spec.action_pvid = action_spec.action_pvid;


        return p4_pd_prog_table_accepted_frame_type_table_modify_with_action_set_pvid(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_egress_vbridge_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_vbridge_table_modify_with__drop\n";


        return p4_pd_prog_table_egress_vbridge_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_egress_vbridge_table_modify_with_action_set_vlan_tag_mode(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_vlan_tag_mode_action_spec_t &action_spec) {
        std::cerr << "In table_egress_vbridge_table_modify_with_action_set_vlan_tag_mode\n";

        p4_pd_prog_action_set_vlan_tag_mode_action_spec_t pd_action_spec;
        pd_action_spec.action_pcp = action_spec.action_pcp;
        pd_action_spec.action_cfi = action_spec.action_cfi;
        pd_action_spec.action_vid = action_spec.action_vid;
        pd_action_spec.action_ethType = action_spec.action_ethType;
        pd_action_spec.action_tag_mode = action_spec.action_tag_mode;


        return p4_pd_prog_table_egress_vbridge_table_modify_with_action_set_vlan_tag_mode(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_vbridge_table_modify_with__drop(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_vbridge_table_modify_with__drop\n";


        return p4_pd_prog_table_vbridge_table_modify_with__drop(sess_hdl, dev_id, entry);
    }

    EntryHandle_t table_vbridge_table_modify_with_action_set_bridge_id(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry, const prog_action_set_bridge_id_action_spec_t &action_spec) {
        std::cerr << "In table_vbridge_table_modify_with_action_set_bridge_id\n";

        p4_pd_prog_action_set_bridge_id_action_spec_t pd_action_spec;
        pd_action_spec.action_bridge_id = action_spec.action_bridge_id;


        return p4_pd_prog_table_vbridge_table_modify_with_action_set_bridge_id(sess_hdl, dev_id, entry, &pd_action_spec);
    }

    EntryHandle_t table_l3_interface_table_modify_with_action_go_to_in_l3_if_table(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_l3_interface_table_modify_with_action_go_to_in_l3_if_table\n";


        return p4_pd_prog_table_l3_interface_table_modify_with_action_go_to_in_l3_if_table(sess_hdl, dev_id, entry);
    }



    // Table entry delete functions

    int32_t table_xSTP_instance_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_xSTP_instance_table_delete\n";

        return p4_pd_prog_table_xSTP_instance_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_ingress_lag_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_ingress_lag_table_delete\n";

        return p4_pd_prog_table_ingress_lag_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_mc_fdb_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_mc_fdb_table_delete\n";

        return p4_pd_prog_table_mc_fdb_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_mc_l2_sg_g_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_mc_l2_sg_g_table_delete\n";

        return p4_pd_prog_table_mc_l2_sg_g_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_learn_fdb_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_learn_fdb_table_delete\n";

        return p4_pd_prog_table_learn_fdb_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_lag_hash_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_lag_hash_table_delete\n";

        return p4_pd_prog_table_lag_hash_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_egress_lag_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_lag_table_delete\n";

        return p4_pd_prog_table_egress_lag_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_egress_vlan_filtering_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_vlan_filtering_table_delete\n";

        return p4_pd_prog_table_egress_vlan_filtering_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_vbridge_STP_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_vbridge_STP_table_delete\n";

        return p4_pd_prog_table_vbridge_STP_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_ingress_l2_interface_type_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_ingress_l2_interface_type_table_delete\n";

        return p4_pd_prog_table_ingress_l2_interface_type_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_egress_vbridge_STP_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_vbridge_STP_table_delete\n";

        return p4_pd_prog_table_egress_vbridge_STP_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_unknown_unicast_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_unknown_unicast_table_delete\n";

        return p4_pd_prog_table_unknown_unicast_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_l3_if_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_l3_if_table_delete\n";

        return p4_pd_prog_table_l3_if_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_unknown_multicast_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_unknown_multicast_table_delete\n";

        return p4_pd_prog_table_unknown_multicast_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_fdb_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_fdb_table_delete\n";

        return p4_pd_prog_table_fdb_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_xSTP_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_xSTP_table_delete\n";

        return p4_pd_prog_table_xSTP_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_ingress_vlan_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_ingress_vlan_table_delete\n";

        return p4_pd_prog_table_ingress_vlan_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_egress_xSTP_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_xSTP_table_delete\n";

        return p4_pd_prog_table_egress_xSTP_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_ingress_vlan_filtering_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_ingress_vlan_filtering_table_delete\n";

        return p4_pd_prog_table_ingress_vlan_filtering_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_accepted_frame_type_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_accepted_frame_type_table_delete\n";

        return p4_pd_prog_table_accepted_frame_type_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_egress_vbridge_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_egress_vbridge_table_delete\n";

        return p4_pd_prog_table_egress_vbridge_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_vbridge_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_vbridge_table_delete\n";

        return p4_pd_prog_table_vbridge_table_delete(sess_hdl, dev_id, entry);
    }

    int32_t table_l3_interface_table_delete(const SessionHandle_t sess_hdl, const int8_t dev_id, const EntryHandle_t entry) {
        std::cerr << "In table_l3_interface_table_delete\n";

        return p4_pd_prog_table_l3_interface_table_delete(sess_hdl, dev_id, entry);
    }


    // set default action

    int32_t table_xSTP_instance_set_default_action_action_set_stp_id(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_stp_id_action_spec_t &action_spec) {
        std::cerr << "In table_xSTP_instance_set_default_action_action_set_stp_id\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_stp_id_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_id = action_spec.action_stp_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_xSTP_instance_set_default_action_action_set_stp_id(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_ingress_lag_set_default_action_action_set_lag_l2if(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_lag_l2if_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_lag_set_default_action_action_set_lag_l2if\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_lag_l2if_action_spec_t pd_action_spec;
        pd_action_spec.action_is_lag = action_spec.action_is_lag;
        pd_action_spec.action_lag_id = action_spec.action_lag_id;
        pd_action_spec.action_l2_if = action_spec.action_l2_if;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_ingress_lag_set_default_action_action_set_lag_l2if(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_mc_fdb_set_default_action_action_set_mc_fdb_miss(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_mc_fdb_set_default_action_action_set_mc_fdb_miss\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_mc_fdb_set_default_action_action_set_mc_fdb_miss(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_mc_fdb_set_default_action_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_mc_fdb_set_default_action_action_forward_mc_set_if_list\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_mc_fdb_set_default_action_action_forward_mc_set_if_list(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_mc_l2_sg_g_set_default_action_action_set_mc_fdb_miss(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_mc_l2_sg_g_set_default_action_action_set_mc_fdb_miss\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_mc_l2_sg_g_set_default_action_action_set_mc_fdb_miss(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_mc_l2_sg_g_set_default_action_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_mc_l2_sg_g_set_default_action_action_forward_mc_set_if_list\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_mc_l2_sg_g_set_default_action_action_forward_mc_set_if_list(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_learn_fdb_set_default_action_action_learn_mac(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_learn_fdb_set_default_action_action_learn_mac\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_learn_fdb_set_default_action_action_learn_mac(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_learn_fdb_set_default_action__nop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_learn_fdb_set_default_action__nop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_learn_fdb_set_default_action__nop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_lag_hash_set_default_action_action_set_lag_hash_size(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_lag_hash_size_action_spec_t &action_spec) {
        std::cerr << "In table_lag_hash_set_default_action_action_set_lag_hash_size\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_lag_hash_size_action_spec_t pd_action_spec;
        pd_action_spec.action_lag_size = action_spec.action_lag_size;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_lag_hash_set_default_action_action_set_lag_hash_size(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_lag_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_egress_lag_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_lag_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_lag_set_default_action_action_set_out_port(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_out_port_action_spec_t &action_spec) {
        std::cerr << "In table_egress_lag_set_default_action_action_set_out_port\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_out_port_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_lag_set_default_action_action_set_out_port(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_vlan_filtering_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_egress_vlan_filtering_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_vlan_filtering_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_vbridge_STP_set_default_action_action_set_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_vbridge_STP_set_default_action_action_set_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_vbridge_STP_set_default_action_action_set_stp_state(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_ingress_l2_interface_type_set_default_action_action_set_l2_if_type(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_l2_if_type_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_l2_interface_type_set_default_action_action_set_l2_if_type\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_l2_if_type_action_spec_t pd_action_spec;
        pd_action_spec.action_l2_if_type = action_spec.action_l2_if_type;
        pd_action_spec.action_bridge_port = action_spec.action_bridge_port;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_ingress_l2_interface_type_set_default_action_action_set_l2_if_type(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_ingress_l2_interface_type_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_ingress_l2_interface_type_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_ingress_l2_interface_type_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_vbridge_STP_set_default_action_action_set_egress_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_egress_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_egress_vbridge_STP_set_default_action_action_set_egress_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_egress_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_vbridge_STP_set_default_action_action_set_egress_stp_state(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_vbridge_STP_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_egress_vbridge_STP_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_vbridge_STP_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_unknown_unicast_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_unknown_unicast_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_unknown_unicast_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_unknown_unicast_set_default_action_action_forward(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_forward_action_spec_t &action_spec) {
        std::cerr << "In table_unknown_unicast_set_default_action_action_forward\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_forward_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_unknown_unicast_set_default_action_action_forward(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_l3_if_set_default_action_action_forward(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_forward_action_spec_t &action_spec) {
        std::cerr << "In table_l3_if_set_default_action_action_forward\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_forward_action_spec_t pd_action_spec;
        pd_action_spec.action_port = action_spec.action_port;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_l3_if_set_default_action_action_forward(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_unknown_multicast_set_default_action_action_forward_mc_set_if_list(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_unknown_multicast_set_default_action_action_forward_mc_set_if_list\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_unknown_multicast_set_default_action_action_forward_mc_set_if_list(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_fdb_set_default_action_action_forward_set_outIfType(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_forward_set_outIfType_action_spec_t &action_spec) {
        std::cerr << "In table_fdb_set_default_action_action_forward_set_outIfType\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_forward_set_outIfType_action_spec_t pd_action_spec;
        pd_action_spec.action_out_if = action_spec.action_out_if;
        pd_action_spec.action_out_if_type = action_spec.action_out_if_type;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_fdb_set_default_action_action_forward_set_outIfType(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_xSTP_set_default_action_action_set_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_xSTP_set_default_action_action_set_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_xSTP_set_default_action_action_set_stp_state(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_ingress_vlan_set_default_action_action_set_mcast_snp(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_mcast_snp_action_spec_t &action_spec) {
        std::cerr << "In table_ingress_vlan_set_default_action_action_set_mcast_snp\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_mcast_snp_action_spec_t pd_action_spec;
        pd_action_spec.action_mcast_snp = action_spec.action_mcast_snp;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_ingress_vlan_set_default_action_action_set_mcast_snp(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_xSTP_set_default_action_action_set_egress_stp_state(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_egress_stp_state_action_spec_t &action_spec) {
        std::cerr << "In table_egress_xSTP_set_default_action_action_set_egress_stp_state\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_egress_stp_state_action_spec_t pd_action_spec;
        pd_action_spec.action_stp_state = action_spec.action_stp_state;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_xSTP_set_default_action_action_set_egress_stp_state(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_xSTP_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_egress_xSTP_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_xSTP_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_ingress_vlan_filtering_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_ingress_vlan_filtering_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_ingress_vlan_filtering_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_ingress_vlan_filtering_set_default_action__nop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_ingress_vlan_filtering_set_default_action__nop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_ingress_vlan_filtering_set_default_action__nop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_accepted_frame_type_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_accepted_frame_type_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_accepted_frame_type_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_accepted_frame_type_set_default_action_action_set_packet_vid(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_accepted_frame_type_set_default_action_action_set_packet_vid\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_accepted_frame_type_set_default_action_action_set_packet_vid(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_accepted_frame_type_set_default_action_action_set_pvid(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_pvid_action_spec_t &action_spec) {
        std::cerr << "In table_accepted_frame_type_set_default_action_action_set_pvid\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_pvid_action_spec_t pd_action_spec;
        pd_action_spec.action_pvid = action_spec.action_pvid;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_accepted_frame_type_set_default_action_action_set_pvid(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_vbridge_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_egress_vbridge_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_vbridge_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_egress_vbridge_set_default_action_action_set_vlan_tag_mode(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_vlan_tag_mode_action_spec_t &action_spec) {
        std::cerr << "In table_egress_vbridge_set_default_action_action_set_vlan_tag_mode\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_vlan_tag_mode_action_spec_t pd_action_spec;
        pd_action_spec.action_pcp = action_spec.action_pcp;
        pd_action_spec.action_cfi = action_spec.action_cfi;
        pd_action_spec.action_vid = action_spec.action_vid;
        pd_action_spec.action_ethType = action_spec.action_ethType;
        pd_action_spec.action_tag_mode = action_spec.action_tag_mode;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_egress_vbridge_set_default_action_action_set_vlan_tag_mode(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_vbridge_set_default_action__drop(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_vbridge_set_default_action__drop\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_vbridge_set_default_action__drop(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }

    int32_t table_vbridge_set_default_action_action_set_bridge_id(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt, const prog_action_set_bridge_id_action_spec_t &action_spec) {
        std::cerr << "In table_vbridge_set_default_action_action_set_bridge_id\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_prog_action_set_bridge_id_action_spec_t pd_action_spec;
        pd_action_spec.action_bridge_id = action_spec.action_bridge_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_vbridge_set_default_action_action_set_bridge_id(sess_hdl, pd_dev_tgt, &pd_action_spec, &pd_entry);

        // return pd_entry;
    }

    int32_t table_l3_interface_set_default_action_action_go_to_in_l3_if_table(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
        std::cerr << "In table_l3_interface_set_default_action_action_go_to_in_l3_if_table\n";

        p4_pd_dev_target_t pd_dev_tgt;
        pd_dev_tgt.device_id = dev_tgt.dev_id;
        pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

        p4_pd_entry_hdl_t pd_entry;

        return p4_pd_prog_table_l3_interface_set_default_action_action_go_to_in_l3_if_table(sess_hdl, pd_dev_tgt, &pd_entry);

        // return pd_entry;
    }


  int32_t clean_all(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
      std::cerr << "In clean_all\n";

      p4_pd_dev_target_t pd_dev_tgt;
      pd_dev_tgt.device_id = dev_tgt.dev_id;
      pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

      return p4_pd_prog_clean_all(sess_hdl, pd_dev_tgt);
  }

  int32_t tables_clean_all(const SessionHandle_t sess_hdl, const DevTarget_t &dev_tgt) {
      std::cerr << "In tables_clean_all\n";

      p4_pd_dev_target_t pd_dev_tgt;
      pd_dev_tgt.device_id = dev_tgt.dev_id;
      pd_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

      assert(0);

      return 0;
  }

    // INDIRECT ACTION DATA AND MATCH SELECT




    // COUNTERS



  // METERS


  // REGISTERS


  void set_learning_timeout(const SessionHandle_t sess_hdl, const int8_t dev_id, const int32_t msecs) {
      p4_pd_prog_set_learning_timeout(sess_hdl, (const uint8_t)dev_id, msecs);
  }

  std::map<SessionHandle_t, std::list<prog_mac_learn_digest_digest_msg_t> > mac_learn_digest_digest_queues;
  pthread_mutex_t mac_learn_digest_mutex;

  p4_pd_status_t
  mac_learn_digest_receive(const SessionHandle_t sess_hdl,
                        const prog_mac_learn_digest_digest_msg_t &msg) {
    pthread_mutex_lock(&mac_learn_digest_mutex);
    assert(mac_learn_digest_digest_queues.find(sess_hdl) != mac_learn_digest_digest_queues.end());
    std::map<SessionHandle_t, std::list<prog_mac_learn_digest_digest_msg_t> >::iterator digest_queue = mac_learn_digest_digest_queues.find(sess_hdl);
    digest_queue->second.push_back(msg);
    pthread_mutex_unlock(&mac_learn_digest_mutex);

    return 0;
  }

  static p4_pd_status_t
  prog_mac_learn_digest_cb(p4_pd_sess_hdl_t sess_hdl,
                             p4_pd_prog_mac_learn_digest_digest_msg_t *msg,
                             void *cookie) {
    p4_pd_prog_mac_learn_digest_digest_msg_t *msg_ = new p4_pd_prog_mac_learn_digest_digest_msg_t();
    *msg_ = *msg;
    prog_mac_learn_digest_digest_msg_t rpc_msg;
    rpc_msg.msg_ptr = (int64_t)msg_;
    rpc_msg.dev_tgt.dev_id = msg->dev_tgt.device_id;
    rpc_msg.dev_tgt.dev_pipe_id = msg->dev_tgt.dev_pipe_id;
    for (int i = 0; (msg != NULL) && (i < msg->num_entries); ++i) {
      prog_mac_learn_digest_digest_entry_t entry;
      entry.ethernet_srcAddr.insert(entry.ethernet_srcAddr.end(), msg->entries[i].ethernet_srcAddr, msg->entries[i].ethernet_srcAddr + 6);
      entry.ingress_metadata_bridge_id = msg->entries[i].ingress_metadata_bridge_id;
      entry.standard_metadata_ingress_port = msg->entries[i].standard_metadata_ingress_port;
      rpc_msg.msg.push_back(entry);
    }
    return ((progHandler*)cookie)->mac_learn_digest_receive((SessionHandle_t)sess_hdl, rpc_msg);
  }

  void mac_learn_digest_register( const SessionHandle_t sess_hdl, const int8_t dev_id) {
    p4_pd_prog_mac_learn_digest_register(sess_hdl, dev_id, prog_mac_learn_digest_cb, this);
    pthread_mutex_lock(&mac_learn_digest_mutex);
    mac_learn_digest_digest_queues.insert(std::pair<SessionHandle_t, std::list<prog_mac_learn_digest_digest_msg_t> >(sess_hdl, std::list<prog_mac_learn_digest_digest_msg_t>()));
    pthread_mutex_unlock(&mac_learn_digest_mutex);
  }

  void mac_learn_digest_deregister(const SessionHandle_t sess_hdl, const int8_t dev_id) {
    p4_pd_prog_mac_learn_digest_deregister(sess_hdl, dev_id);
    pthread_mutex_lock(&mac_learn_digest_mutex);
    mac_learn_digest_digest_queues.erase(sess_hdl);
    pthread_mutex_unlock(&mac_learn_digest_mutex);
  }

  void mac_learn_digest_get_digest(prog_mac_learn_digest_digest_msg_t &msg, const SessionHandle_t sess_hdl) {
    msg.msg_ptr = 0;
    msg.msg.clear();

    pthread_mutex_lock(&mac_learn_digest_mutex);
    std::map<SessionHandle_t, std::list<prog_mac_learn_digest_digest_msg_t> >::iterator digest_queue = mac_learn_digest_digest_queues.find(sess_hdl);
    if (digest_queue != mac_learn_digest_digest_queues.end()) {
      if (digest_queue->second.size() > 0) {
        msg = digest_queue->second.front();
        digest_queue->second.pop_front();
      }
    }

    pthread_mutex_unlock(&mac_learn_digest_mutex);
  }

  void mac_learn_digest_digest_notify_ack(const SessionHandle_t sess_hdl, const int64_t msg_ptr) {
    p4_pd_prog_mac_learn_digest_digest_msg_t *msg = (p4_pd_prog_mac_learn_digest_digest_msg_t *) msg_ptr;
    p4_pd_prog_mac_learn_digest_notify_ack((p4_pd_sess_hdl_t)sess_hdl, msg);
    delete msg;
  }
};
