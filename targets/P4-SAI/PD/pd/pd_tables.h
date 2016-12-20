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

#ifndef _P4_PD_TABLES_H_
#define _P4_PD_TABLES_H_

#include <bm/pdfixed/pd_common.h>

#include "pd_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ADD ENTRIES */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_table_add_with_action_set_stp_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_xSTP_instance_match_spec_t *match_spec,
 p4_pd_prog_action_set_stp_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_lag_table_add_with_action_set_lag_l2if
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_lag_match_spec_t *match_spec,
 p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_add_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_add_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_add_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_add_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_add_with_action_learn_mac
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_learn_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_add_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_learn_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_lag_hash_table_add_with_action_set_lag_hash_size
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_lag_hash_match_spec_t *match_spec,
 p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_lag_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_add_with_action_set_out_port
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_lag_match_spec_t *match_spec,
 p4_pd_prog_action_set_out_port_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vlan_filtering_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_table_add_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_vbridge_STP_match_spec_t *match_spec,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_add_with_action_set_l2_if_type
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec,
 p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_add_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_add_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_l3_if_table_add_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_l3_if_match_spec_t *match_spec,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_table_add_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_unknown_multicast_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_fdb_table_add_with_action_forward_set_outIfType
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_fdb_match_spec_t *match_spec,
 p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_xSTP_table_add_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_xSTP_match_spec_t *match_spec,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_table_add_with_action_set_mcast_snp
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_vlan_match_spec_t *match_spec,
 p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_add_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_add_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_add_with_action_set_packet_vid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_add_with_action_set_pvid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 p4_pd_prog_action_set_pvid_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_add_with_action_set_vlan_tag_mode
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec,
 p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_vbridge_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_table_add_with_action_set_bridge_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_vbridge_match_spec_t *match_spec,
 p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_l3_interface_table_add_with_action_go_to_in_l3_if_table
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_l3_interface_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
);



/* DELETE ENTRIES */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_lag_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_lag_hash_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_l3_if_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_fdb_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_xSTP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_l3_interface_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);


/* MODIFY ENTRIES */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_table_modify_with_action_set_stp_id
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_stp_id_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_ingress_lag_table_modify_with_action_set_lag_l2if
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_modify_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_modify_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_modify_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_modify_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_modify_with_action_learn_mac
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_modify_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_lag_hash_table_modify_with_action_set_lag_hash_size
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_modify_with_action_set_out_port
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_out_port_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_table_modify_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_modify_with_action_set_l2_if_type
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_modify_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_modify_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_forward_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_l3_if_table_modify_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_forward_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_table_modify_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_fdb_table_modify_with_action_forward_set_outIfType
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_xSTP_table_modify_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_table_modify_with_action_set_mcast_snp
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_modify_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_modify_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_modify_with_action_set_packet_vid
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_modify_with_action_set_pvid
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_pvid_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_modify_with_action_set_vlan_tag_mode
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_vbridge_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_table_modify_with_action_set_bridge_id
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec
);

p4_pd_status_t
p4_pd_prog_table_l3_interface_table_modify_with_action_go_to_in_l3_if_table
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
);



/* SET DEFAULT_ACTION */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_set_default_action_action_set_stp_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_stp_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_lag_set_default_action_action_set_lag_l2if
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_set_default_action_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_set_default_action_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_set_default_action_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_set_default_action_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_set_default_action_action_learn_mac
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_set_default_action__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_lag_hash_set_default_action_action_set_lag_hash_size
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_set_default_action_action_set_out_port
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_out_port_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_set_default_action_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_set_default_action_action_set_l2_if_type
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_set_default_action_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_set_default_action_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_l3_if_set_default_action_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_set_default_action_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_fdb_set_default_action_action_forward_set_outIfType
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_xSTP_set_default_action_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_set_default_action_action_set_mcast_snp
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_set_default_action_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_set_default_action__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_set_default_action_action_set_packet_vid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_set_default_action_action_set_pvid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_pvid_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_set_default_action_action_set_vlan_tag_mode
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_vbridge_set_default_action_action_set_bridge_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
);

p4_pd_status_t
p4_pd_prog_table_l3_interface_set_default_action_action_go_to_in_l3_if_table
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
);





/* ENTRY RETRIEVAL */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_xSTP_instance_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_ingress_lag_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_lag_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_mc_fdb_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_mc_fdb_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_learn_fdb_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_learn_fdb_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_lag_hash_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_lag_hash_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_egress_lag_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_lag_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_vlan_filtering_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_vbridge_STP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_l3_if_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_l3_if_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_unknown_multicast_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_fdb_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_fdb_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_xSTP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_xSTP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_vlan_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_vbridge_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_vbridge_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);

p4_pd_status_t
p4_pd_prog_table_l3_interface_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_l3_interface_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
);




/* DIRECT COUNTERS */



/* Clean all state */
p4_pd_status_t
p4_pd_prog_clean_all(p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt);

#ifdef __cplusplus
}
#endif

#endif
