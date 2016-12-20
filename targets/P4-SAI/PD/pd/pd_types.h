/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Antonin Bas (antonin@barefootnetworks.com)
 *
 */

#ifndef _P4_PD_TYPES_H_
#define _P4_PD_TYPES_H_

#include <stdint.h>

/* MATCH STRUCTS */

typedef struct p4_pd_prog_table_xSTP_instance_match_spec {
  uint16_t ingress_metadata_vid;
} p4_pd_prog_table_xSTP_instance_match_spec_t;

typedef struct p4_pd_prog_table_ingress_lag_match_spec {
  uint16_t standard_metadata_ingress_port;
} p4_pd_prog_table_ingress_lag_match_spec_t;

typedef struct p4_pd_prog_table_mc_fdb_match_spec {
  uint8_t ethernet_dstAddr[6];
  uint8_t ingress_metadata_bridge_id;
} p4_pd_prog_table_mc_fdb_match_spec_t;

typedef struct p4_pd_prog_table_mc_l2_sg_g_match_spec {
  uint8_t ingress_metadata_bridge_id;
  uint32_t ipv4_srcAddr;
  uint32_t ipv4_dstAddr;
} p4_pd_prog_table_mc_l2_sg_g_match_spec_t;

typedef struct p4_pd_prog_table_learn_fdb_match_spec {
  uint8_t ethernet_srcAddr[6];
  uint8_t ingress_metadata_bridge_id;
} p4_pd_prog_table_learn_fdb_match_spec_t;

typedef struct p4_pd_prog_table_lag_hash_match_spec {
  uint8_t egress_metadata_out_if;
} p4_pd_prog_table_lag_hash_match_spec_t;

typedef struct p4_pd_prog_table_egress_lag_match_spec {
  uint8_t egress_metadata_out_if;
  uint8_t egress_metadata_hash_val;
} p4_pd_prog_table_egress_lag_match_spec_t;

typedef struct p4_pd_prog_table_egress_vlan_filtering_match_spec {
  uint8_t egress_metadata_out_if;
  uint16_t ingress_metadata_vid;
} p4_pd_prog_table_egress_vlan_filtering_match_spec_t;

typedef struct p4_pd_prog_table_vbridge_STP_match_spec {
  uint8_t ingress_metadata_bridge_port;
} p4_pd_prog_table_vbridge_STP_match_spec_t;

typedef struct p4_pd_prog_table_ingress_l2_interface_type_match_spec {
  uint8_t ingress_metadata_l2_if;
  uint16_t ingress_metadata_vid;
} p4_pd_prog_table_ingress_l2_interface_type_match_spec_t;

typedef struct p4_pd_prog_table_egress_vbridge_STP_match_spec {
  uint8_t ingress_metadata_bridge_port;
} p4_pd_prog_table_egress_vbridge_STP_match_spec_t;

typedef struct p4_pd_prog_table_unknown_unicast_match_spec {
  uint8_t ingress_metadata_bridge_id;
} p4_pd_prog_table_unknown_unicast_match_spec_t;

typedef struct p4_pd_prog_table_l3_if_match_spec {
  uint8_t ethernet_dstAddr[6];
  uint8_t ingress_metadata_bridge_id;
} p4_pd_prog_table_l3_if_match_spec_t;

typedef struct p4_pd_prog_table_unknown_multicast_match_spec {
  uint8_t ingress_metadata_bridge_id;
} p4_pd_prog_table_unknown_multicast_match_spec_t;

typedef struct p4_pd_prog_table_fdb_match_spec {
  uint8_t ethernet_dstAddr[6];
  uint8_t ingress_metadata_bridge_id;
} p4_pd_prog_table_fdb_match_spec_t;

typedef struct p4_pd_prog_table_xSTP_match_spec {
  uint8_t ingress_metadata_bridge_port;
  uint8_t ingress_metadata_stp_id;
} p4_pd_prog_table_xSTP_match_spec_t;

typedef struct p4_pd_prog_table_ingress_vlan_match_spec {
  uint16_t ingress_metadata_vid;
} p4_pd_prog_table_ingress_vlan_match_spec_t;

typedef struct p4_pd_prog_table_egress_xSTP_match_spec {
  uint8_t egress_metadata_out_if;
  uint8_t ingress_metadata_stp_id;
} p4_pd_prog_table_egress_xSTP_match_spec_t;

typedef struct p4_pd_prog_table_ingress_vlan_filtering_match_spec {
  uint8_t ingress_metadata_l2_if;
  uint16_t ingress_metadata_vid;
} p4_pd_prog_table_ingress_vlan_filtering_match_spec_t;

typedef struct p4_pd_prog_table_accepted_frame_type_match_spec {
  uint8_t ingress_metadata_l2_if;
  uint8_t vlan_valid;
} p4_pd_prog_table_accepted_frame_type_match_spec_t;

typedef struct p4_pd_prog_table_egress_vbridge_match_spec {
  uint8_t ingress_metadata_bridge_port;
} p4_pd_prog_table_egress_vbridge_match_spec_t;

typedef struct p4_pd_prog_table_vbridge_match_spec {
  uint8_t ingress_metadata_bridge_port;
} p4_pd_prog_table_vbridge_match_spec_t;

typedef struct p4_pd_prog_table_l3_interface_match_spec {
  uint8_t ethernet_dstAddr[6];
  uint8_t ingress_metadata_bridge_id;
} p4_pd_prog_table_l3_interface_match_spec_t;



/* ACTION STRUCTS */

typedef struct p4_pd_prog_action_set_egress_stp_state_action_spec {
  uint8_t action_stp_state;
} p4_pd_prog_action_set_egress_stp_state_action_spec_t;

typedef struct p4_pd_prog_action_set_lag_l2if_action_spec {
  uint8_t action_is_lag;
  uint16_t action_lag_id;
  uint8_t action_l2_if;
} p4_pd_prog_action_set_lag_l2if_action_spec_t;

typedef struct p4_pd_prog_action_set_vlan_tag_mode_action_spec {
  uint8_t action_pcp;
  uint8_t action_cfi;
  uint16_t action_vid;
  uint16_t action_ethType;
  uint8_t action_tag_mode;
} p4_pd_prog_action_set_vlan_tag_mode_action_spec_t;

typedef struct p4_pd_prog_action_set_out_port_action_spec {
  uint8_t action_port;
} p4_pd_prog_action_set_out_port_action_spec_t;

typedef struct p4_pd_prog_action_forward_set_outIfType_action_spec {
  uint8_t action_out_if;
  uint8_t action_out_if_type;
} p4_pd_prog_action_forward_set_outIfType_action_spec_t;

/* action_learn_mac has no parameters */

/* action_forward_mc_set_if_list has no parameters */

/* action_set_mc_fdb_miss has no parameters */

typedef struct p4_pd_prog_action_forward_action_spec {
  uint8_t action_port;
} p4_pd_prog_action_forward_action_spec_t;

/* action_go_to_in_l3_if_table has no parameters */

typedef struct p4_pd_prog_action_set_lag_hash_size_action_spec {
  uint8_t action_lag_size;
} p4_pd_prog_action_set_lag_hash_size_action_spec_t;

typedef struct p4_pd_prog_action_set_l2_if_type_action_spec {
  uint8_t action_l2_if_type;
  uint8_t action_bridge_port;
} p4_pd_prog_action_set_l2_if_type_action_spec_t;

typedef struct p4_pd_prog_action_set_bridge_id_action_spec {
  uint8_t action_bridge_id;
} p4_pd_prog_action_set_bridge_id_action_spec_t;

/* _nop has no parameters */

/* _drop has no parameters */

typedef struct p4_pd_prog_action_set_mcast_snp_action_spec {
  uint8_t action_mcast_snp;
} p4_pd_prog_action_set_mcast_snp_action_spec_t;

/* action_set_packet_vid has no parameters */

typedef struct p4_pd_prog_action_set_pvid_action_spec {
  uint16_t action_pvid;
} p4_pd_prog_action_set_pvid_action_spec_t;

typedef struct p4_pd_prog_action_set_stp_id_action_spec {
  uint8_t action_stp_id;
} p4_pd_prog_action_set_stp_id_action_spec_t;

typedef struct p4_pd_prog_action_set_stp_state_action_spec {
  uint8_t action_stp_state;
} p4_pd_prog_action_set_stp_state_action_spec_t;


#endif
