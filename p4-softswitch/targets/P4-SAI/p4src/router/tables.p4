#include "../inc/defines.p4"


table table_ingress_vrf {
	reads{
		router_metadata.ingress_rif : exact;
	}
	actions{action_set_vrf; _drop;} //; action_set_acl_id }
	size: ROUTER_IF_TABLE_SIZE;
}

table table_router {
	reads{
		router_metadata.l3_lpm_key : lpm;
	}
	actions{action_set_nhop_id; action_set_nhop_grp_id; _drop;}
	size: ROUTER_LPM_TABLE_SIZE;
}

table table_next_hop_group {
	reads{
		router_metadata.next_hop_group_id : exact;
		//router_metadata.next_hop_hash  : exact; // TODO
	}
	actions{ action_set_nhop_id ; _drop;}
	size: NHOP_GRP_TABLE_SIZE;
}


table table_next_hop {
	reads{
		router_metadata.next_hop_id : exact;
	}
	actions{ action_set_erif_set_nh_dstip ; action_set_erif_set_nh_dstip_from_pkt  ; _drop;}
	size: NHOP_TABLE_SIZE;
}

// table table_erif_check {
// 	reads{
// 		egress_metadata.erif : exact;
// 	}
// 	actions{ action_chk_ttl_mtu; _drop;}
// 	size: ;
// }

table table_Neigh {
	reads{
		router_metadata.egress_rif 		: exact;
		router_metadata.next_hop_dst_ip 	: exact;
	}
	actions{action_set_packet_dmac; action_trap_to_cpu;}
		//TODO :action_trap ;action_forward ;action_drop; action_copy_to_cpu}
	size: NEIGH_TABLE_SIZE;
}

table table_egress_L3_if {
	reads{
		router_metadata.egress_rif : exact;
	}
	actions{action_set_smac_vid; _drop; } // TODO check if type is ok here - not mentioned in visio
	size: L3_EGRESS_IF_TABLE_SIZE;
}
