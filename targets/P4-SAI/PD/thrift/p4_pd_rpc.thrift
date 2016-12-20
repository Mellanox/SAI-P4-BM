# Copyright 2013-present Barefoot Networks, Inc. 
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# P4 Thrift RPC Input


include "res.thrift"

namespace py p4_pd_rpc
namespace cpp p4_pd_rpc
namespace c_glib p4_pd_rpc

typedef i32 EntryHandle_t
typedef i32 MemberHandle_t
typedef i32 GroupHandle_t
typedef binary MacAddr_t
typedef binary IPv6_t

struct prog_counter_value_t {
  1: required i64 packets;
  2: required i64 bytes;
}

struct prog_counter_flags_t {
  1: required bool read_hw_sync;
}

struct prog_packets_meter_spec_t {
  1: required i32 cir_pps;
  2: required i32 cburst_pkts;
  3: required i32 pir_pps;
  4: required i32 pburst_pkts;
  5: required bool color_aware;  // ignored for now
}

struct prog_bytes_meter_spec_t {
  1: required i32 cir_kbps;
  2: required i32 cburst_kbits;
  3: required i32 pir_kbps;
  4: required i32 pburst_kbits;
  5: required bool color_aware;  // ignored for now
}

# Match structs

struct prog_table_xSTP_instance_match_spec_t {
  1: required i16 ingress_metadata_vid;
}

struct prog_table_ingress_lag_match_spec_t {
  1: required i16 standard_metadata_ingress_port;
}

struct prog_table_mc_fdb_match_spec_t {
  1: required MacAddr_t ethernet_dstAddr;
  2: required byte ingress_metadata_bridge_id;
}

struct prog_table_mc_l2_sg_g_match_spec_t {
  1: required byte ingress_metadata_bridge_id;
  2: required i32 ipv4_srcAddr;
  3: required i32 ipv4_dstAddr;
}

struct prog_table_learn_fdb_match_spec_t {
  1: required MacAddr_t ethernet_srcAddr;
  2: required byte ingress_metadata_bridge_id;
}

struct prog_table_lag_hash_match_spec_t {
  1: required byte egress_metadata_out_if;
}

struct prog_table_egress_lag_match_spec_t {
  1: required byte egress_metadata_out_if;
  2: required byte egress_metadata_hash_val;
}

struct prog_table_egress_vlan_filtering_match_spec_t {
  1: required byte egress_metadata_out_if;
  2: required i16 ingress_metadata_vid;
}

struct prog_table_vbridge_STP_match_spec_t {
  1: required byte ingress_metadata_bridge_port;
}

struct prog_table_ingress_l2_interface_type_match_spec_t {
  1: required byte ingress_metadata_l2_if;
  2: required i16 ingress_metadata_vid;
}

struct prog_table_egress_vbridge_STP_match_spec_t {
  1: required byte ingress_metadata_bridge_port;
}

struct prog_table_unknown_unicast_match_spec_t {
  1: required byte ingress_metadata_bridge_id;
}

struct prog_table_l3_if_match_spec_t {
  1: required MacAddr_t ethernet_dstAddr;
  2: required byte ingress_metadata_bridge_id;
}

struct prog_table_unknown_multicast_match_spec_t {
  1: required byte ingress_metadata_bridge_id;
}

struct prog_table_fdb_match_spec_t {
  1: required MacAddr_t ethernet_dstAddr;
  2: required byte ingress_metadata_bridge_id;
}

struct prog_table_xSTP_match_spec_t {
  1: required byte ingress_metadata_bridge_port;
  2: required byte ingress_metadata_stp_id;
}

struct prog_table_ingress_vlan_match_spec_t {
  1: required i16 ingress_metadata_vid;
}

struct prog_table_egress_xSTP_match_spec_t {
  1: required byte egress_metadata_out_if;
  2: required byte ingress_metadata_stp_id;
}

struct prog_table_ingress_vlan_filtering_match_spec_t {
  1: required byte ingress_metadata_l2_if;
  2: required i16 ingress_metadata_vid;
}

struct prog_table_accepted_frame_type_match_spec_t {
  1: required byte ingress_metadata_l2_if;
  2: required byte vlan_valid;
}

struct prog_table_egress_vbridge_match_spec_t {
  1: required byte ingress_metadata_bridge_port;
}

struct prog_table_vbridge_match_spec_t {
  1: required byte ingress_metadata_bridge_port;
}

struct prog_table_l3_interface_match_spec_t {
  1: required MacAddr_t ethernet_dstAddr;
  2: required byte ingress_metadata_bridge_id;
}



# Action structs

struct prog_action_set_egress_stp_state_action_spec_t {
  1: required byte action_stp_state;
}

struct prog_action_set_lag_l2if_action_spec_t {
  1: required byte action_is_lag;
  2: required i16 action_lag_id;
  3: required byte action_l2_if;
}

struct prog_action_set_vlan_tag_mode_action_spec_t {
  1: required byte action_pcp;
  2: required byte action_cfi;
  3: required i16 action_vid;
  4: required i16 action_ethType;
  5: required byte action_tag_mode;
}

struct prog_action_set_out_port_action_spec_t {
  1: required byte action_port;
}

struct prog_action_forward_set_outIfType_action_spec_t {
  1: required byte action_out_if;
  2: required byte action_out_if_type;
}

/* action_learn_mac has no parameters */

/* action_forward_mc_set_if_list has no parameters */

/* action_set_mc_fdb_miss has no parameters */

struct prog_action_forward_action_spec_t {
  1: required byte action_port;
}

/* action_go_to_in_l3_if_table has no parameters */

struct prog_action_set_lag_hash_size_action_spec_t {
  1: required byte action_lag_size;
}

struct prog_action_set_l2_if_type_action_spec_t {
  1: required byte action_l2_if_type;
  2: required byte action_bridge_port;
}

struct prog_action_set_bridge_id_action_spec_t {
  1: required byte action_bridge_id;
}

/* _nop has no parameters */

/* _drop has no parameters */

struct prog_action_set_mcast_snp_action_spec_t {
  1: required byte action_mcast_snp;
}

/* action_set_packet_vid has no parameters */

struct prog_action_set_pvid_action_spec_t {
  1: required i16 action_pvid;
}

struct prog_action_set_stp_id_action_spec_t {
  1: required byte action_stp_id;
}

struct prog_action_set_stp_state_action_spec_t {
  1: required byte action_stp_state;
}



struct prog_mac_learn_digest_digest_entry_t {
  1: required list<byte> ethernet_srcAddr;
  2: required byte ingress_metadata_bridge_id;
  3: required i16 standard_metadata_ingress_port;
}

struct prog_mac_learn_digest_digest_msg_t {
  1: required res.DevTarget_t             dev_tgt;
  2: required list<prog_mac_learn_digest_digest_entry_t> msg;
  3: required i64                     msg_ptr;
}


service prog {

    # Table entry add functions
    EntryHandle_t table_xSTP_instance_table_add_with_action_set_stp_id(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_xSTP_instance_match_spec_t match_spec, 4:prog_action_set_stp_id_action_spec_t action_spec);
    EntryHandle_t table_ingress_lag_table_add_with_action_set_lag_l2if(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_ingress_lag_match_spec_t match_spec, 4:prog_action_set_lag_l2if_action_spec_t action_spec);
    EntryHandle_t table_mc_fdb_table_add_with_action_set_mc_fdb_miss(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_mc_fdb_match_spec_t match_spec);
    EntryHandle_t table_mc_fdb_table_add_with_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_mc_fdb_match_spec_t match_spec);
    EntryHandle_t table_mc_l2_sg_g_table_add_with_action_set_mc_fdb_miss(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_mc_l2_sg_g_match_spec_t match_spec);
    EntryHandle_t table_mc_l2_sg_g_table_add_with_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_mc_l2_sg_g_match_spec_t match_spec);
    EntryHandle_t table_learn_fdb_table_add_with_action_learn_mac(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_learn_fdb_match_spec_t match_spec);
    EntryHandle_t table_learn_fdb_table_add_with__nop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_learn_fdb_match_spec_t match_spec);
    EntryHandle_t table_lag_hash_table_add_with_action_set_lag_hash_size(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_lag_hash_match_spec_t match_spec, 4:prog_action_set_lag_hash_size_action_spec_t action_spec);
    EntryHandle_t table_egress_lag_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_lag_match_spec_t match_spec);
    EntryHandle_t table_egress_lag_table_add_with_action_set_out_port(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_lag_match_spec_t match_spec, 4:prog_action_set_out_port_action_spec_t action_spec);
    EntryHandle_t table_egress_vlan_filtering_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_vlan_filtering_match_spec_t match_spec);
    EntryHandle_t table_vbridge_STP_table_add_with_action_set_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_vbridge_STP_match_spec_t match_spec, 4:prog_action_set_stp_state_action_spec_t action_spec);
    EntryHandle_t table_ingress_l2_interface_type_table_add_with_action_set_l2_if_type(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_ingress_l2_interface_type_match_spec_t match_spec, 4:prog_action_set_l2_if_type_action_spec_t action_spec);
    EntryHandle_t table_ingress_l2_interface_type_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_ingress_l2_interface_type_match_spec_t match_spec);
    EntryHandle_t table_egress_vbridge_STP_table_add_with_action_set_egress_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_vbridge_STP_match_spec_t match_spec, 4:prog_action_set_egress_stp_state_action_spec_t action_spec);
    EntryHandle_t table_egress_vbridge_STP_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_vbridge_STP_match_spec_t match_spec);
    EntryHandle_t table_unknown_unicast_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_unknown_unicast_match_spec_t match_spec);
    EntryHandle_t table_unknown_unicast_table_add_with_action_forward(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_unknown_unicast_match_spec_t match_spec, 4:prog_action_forward_action_spec_t action_spec);
    EntryHandle_t table_l3_if_table_add_with_action_forward(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_l3_if_match_spec_t match_spec, 4:prog_action_forward_action_spec_t action_spec);
    EntryHandle_t table_unknown_multicast_table_add_with_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_unknown_multicast_match_spec_t match_spec);
    EntryHandle_t table_fdb_table_add_with_action_forward_set_outIfType(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_fdb_match_spec_t match_spec, 4:prog_action_forward_set_outIfType_action_spec_t action_spec);
    EntryHandle_t table_xSTP_table_add_with_action_set_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_xSTP_match_spec_t match_spec, 4:prog_action_set_stp_state_action_spec_t action_spec);
    EntryHandle_t table_ingress_vlan_table_add_with_action_set_mcast_snp(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_ingress_vlan_match_spec_t match_spec, 4:prog_action_set_mcast_snp_action_spec_t action_spec);
    EntryHandle_t table_egress_xSTP_table_add_with_action_set_egress_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_xSTP_match_spec_t match_spec, 4:prog_action_set_egress_stp_state_action_spec_t action_spec);
    EntryHandle_t table_egress_xSTP_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_xSTP_match_spec_t match_spec);
    EntryHandle_t table_ingress_vlan_filtering_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_ingress_vlan_filtering_match_spec_t match_spec);
    EntryHandle_t table_ingress_vlan_filtering_table_add_with__nop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_ingress_vlan_filtering_match_spec_t match_spec);
    EntryHandle_t table_accepted_frame_type_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_accepted_frame_type_match_spec_t match_spec);
    EntryHandle_t table_accepted_frame_type_table_add_with_action_set_packet_vid(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_accepted_frame_type_match_spec_t match_spec);
    EntryHandle_t table_accepted_frame_type_table_add_with_action_set_pvid(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_accepted_frame_type_match_spec_t match_spec, 4:prog_action_set_pvid_action_spec_t action_spec);
    EntryHandle_t table_egress_vbridge_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_vbridge_match_spec_t match_spec);
    EntryHandle_t table_egress_vbridge_table_add_with_action_set_vlan_tag_mode(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_egress_vbridge_match_spec_t match_spec, 4:prog_action_set_vlan_tag_mode_action_spec_t action_spec);
    EntryHandle_t table_vbridge_table_add_with__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_vbridge_match_spec_t match_spec);
    EntryHandle_t table_vbridge_table_add_with_action_set_bridge_id(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_vbridge_match_spec_t match_spec, 4:prog_action_set_bridge_id_action_spec_t action_spec);
    EntryHandle_t table_l3_interface_table_add_with_action_go_to_in_l3_if_table(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_table_l3_interface_match_spec_t match_spec);

    # Table entry modify functions
    i32 table_xSTP_instance_table_modify_with_action_set_stp_id(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_stp_id_action_spec_t action_spec);
    i32 table_ingress_lag_table_modify_with_action_set_lag_l2if(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_lag_l2if_action_spec_t action_spec);
    i32 table_mc_fdb_table_modify_with_action_set_mc_fdb_miss(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_mc_fdb_table_modify_with_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_mc_l2_sg_g_table_modify_with_action_set_mc_fdb_miss(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_mc_l2_sg_g_table_modify_with_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_learn_fdb_table_modify_with_action_learn_mac(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_learn_fdb_table_modify_with__nop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_lag_hash_table_modify_with_action_set_lag_hash_size(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_lag_hash_size_action_spec_t action_spec);
    i32 table_egress_lag_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_lag_table_modify_with_action_set_out_port(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_out_port_action_spec_t action_spec);
    i32 table_egress_vlan_filtering_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_vbridge_STP_table_modify_with_action_set_stp_state(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_stp_state_action_spec_t action_spec);
    i32 table_ingress_l2_interface_type_table_modify_with_action_set_l2_if_type(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_l2_if_type_action_spec_t action_spec);
    i32 table_ingress_l2_interface_type_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_vbridge_STP_table_modify_with_action_set_egress_stp_state(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_egress_stp_state_action_spec_t action_spec);
    i32 table_egress_vbridge_STP_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_unknown_unicast_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_unknown_unicast_table_modify_with_action_forward(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_forward_action_spec_t action_spec);
    i32 table_l3_if_table_modify_with_action_forward(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_forward_action_spec_t action_spec);
    i32 table_unknown_multicast_table_modify_with_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_fdb_table_modify_with_action_forward_set_outIfType(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_forward_set_outIfType_action_spec_t action_spec);
    i32 table_xSTP_table_modify_with_action_set_stp_state(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_stp_state_action_spec_t action_spec);
    i32 table_ingress_vlan_table_modify_with_action_set_mcast_snp(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_mcast_snp_action_spec_t action_spec);
    i32 table_egress_xSTP_table_modify_with_action_set_egress_stp_state(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_egress_stp_state_action_spec_t action_spec);
    i32 table_egress_xSTP_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_ingress_vlan_filtering_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_ingress_vlan_filtering_table_modify_with__nop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_accepted_frame_type_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_accepted_frame_type_table_modify_with_action_set_packet_vid(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_accepted_frame_type_table_modify_with_action_set_pvid(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_pvid_action_spec_t action_spec);
    i32 table_egress_vbridge_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_vbridge_table_modify_with_action_set_vlan_tag_mode(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_vlan_tag_mode_action_spec_t action_spec);
    i32 table_vbridge_table_modify_with__drop(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_vbridge_table_modify_with_action_set_bridge_id(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry, 4:prog_action_set_bridge_id_action_spec_t action_spec);
    i32 table_l3_interface_table_modify_with_action_go_to_in_l3_if_table(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);

    # Table entry delete functions
    i32 table_xSTP_instance_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_ingress_lag_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_mc_fdb_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_mc_l2_sg_g_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_learn_fdb_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_lag_hash_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_lag_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_vlan_filtering_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_vbridge_STP_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_ingress_l2_interface_type_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_vbridge_STP_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_unknown_unicast_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_l3_if_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_unknown_multicast_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_fdb_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_xSTP_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_ingress_vlan_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_xSTP_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_ingress_vlan_filtering_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_accepted_frame_type_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_egress_vbridge_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_vbridge_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);
    i32 table_l3_interface_table_delete(1:res.SessionHandle_t sess_hdl, 2:byte dev_id, 3:EntryHandle_t entry);

    # Table set default action functions
    EntryHandle_t table_xSTP_instance_set_default_action_action_set_stp_id(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_stp_id_action_spec_t action_spec);
    EntryHandle_t table_ingress_lag_set_default_action_action_set_lag_l2if(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_lag_l2if_action_spec_t action_spec);
    EntryHandle_t table_mc_fdb_set_default_action_action_set_mc_fdb_miss(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_mc_fdb_set_default_action_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_mc_l2_sg_g_set_default_action_action_set_mc_fdb_miss(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_mc_l2_sg_g_set_default_action_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_learn_fdb_set_default_action_action_learn_mac(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_learn_fdb_set_default_action__nop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_lag_hash_set_default_action_action_set_lag_hash_size(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_lag_hash_size_action_spec_t action_spec);
    EntryHandle_t table_egress_lag_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_egress_lag_set_default_action_action_set_out_port(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_out_port_action_spec_t action_spec);
    EntryHandle_t table_egress_vlan_filtering_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_vbridge_STP_set_default_action_action_set_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_stp_state_action_spec_t action_spec);
    EntryHandle_t table_ingress_l2_interface_type_set_default_action_action_set_l2_if_type(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_l2_if_type_action_spec_t action_spec);
    EntryHandle_t table_ingress_l2_interface_type_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_egress_vbridge_STP_set_default_action_action_set_egress_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_egress_stp_state_action_spec_t action_spec);
    EntryHandle_t table_egress_vbridge_STP_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_unknown_unicast_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_unknown_unicast_set_default_action_action_forward(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_forward_action_spec_t action_spec);
    EntryHandle_t table_l3_if_set_default_action_action_forward(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_forward_action_spec_t action_spec);
    EntryHandle_t table_unknown_multicast_set_default_action_action_forward_mc_set_if_list(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_fdb_set_default_action_action_forward_set_outIfType(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_forward_set_outIfType_action_spec_t action_spec);
    EntryHandle_t table_xSTP_set_default_action_action_set_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_stp_state_action_spec_t action_spec);
    EntryHandle_t table_ingress_vlan_set_default_action_action_set_mcast_snp(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_mcast_snp_action_spec_t action_spec);
    EntryHandle_t table_egress_xSTP_set_default_action_action_set_egress_stp_state(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_egress_stp_state_action_spec_t action_spec);
    EntryHandle_t table_egress_xSTP_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_ingress_vlan_filtering_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_ingress_vlan_filtering_set_default_action__nop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_accepted_frame_type_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_accepted_frame_type_set_default_action_action_set_packet_vid(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_accepted_frame_type_set_default_action_action_set_pvid(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_pvid_action_spec_t action_spec);
    EntryHandle_t table_egress_vbridge_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_egress_vbridge_set_default_action_action_set_vlan_tag_mode(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_vlan_tag_mode_action_spec_t action_spec);
    EntryHandle_t table_vbridge_set_default_action__drop(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);
    EntryHandle_t table_vbridge_set_default_action_action_set_bridge_id(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt, 3:prog_action_set_bridge_id_action_spec_t action_spec);
    EntryHandle_t table_l3_interface_set_default_action_action_go_to_in_l3_if_table(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);

    # INDIRECT ACTION DATA AND MATCH SELECT






    # clean all state
    i32 clean_all(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);

    # clean table state
    i32 tables_clean_all(1:res.SessionHandle_t sess_hdl, 2:res.DevTarget_t dev_tgt);

    # counters



    # meters


    # registers


    void set_learning_timeout(1: res.SessionHandle_t sess_hdl, 2: byte dev_id, 3: i32 msecs);

    void mac_learn_digest_register(1: res.SessionHandle_t sess_hdl, 2: byte dev_id);
    void mac_learn_digest_deregister(1: res.SessionHandle_t sess_hdl, 2: byte dev_id);
    prog_mac_learn_digest_digest_msg_t mac_learn_digest_get_digest(1: res.SessionHandle_t sess_hdl);
    void mac_learn_digest_digest_notify_ack(1: res.SessionHandle_t sess_hdl, 2: i64 msg_ptr);
}
