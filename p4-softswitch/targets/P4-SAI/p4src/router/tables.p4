#include "../inc/defines.p4"

table table_ingress_l3_if {
	reads{
		standard_metadata.ingress_port : exact;
		vlan.vid : exact;
	}
	actions{action_set_irif; _drop;}
	size: L3_EGRESS_IF_TABLE_SIZE;
}

table table_ingress_vrf {
	reads{
		router_metadata.ingress_rif : exact;
	}
	actions{action_set_vrf; _drop;} //; action_set_acl_id }
	size: ROUTER_IF_TABLE_SIZE;
}

table table_pre_l3_trap {
	reads {
		vlan.etherType : ternary;
		ipv4.dstAddr   : lpm;
	}
	actions { action_set_trap_id; _drop;}
}

table table_ip2me_trap {
	reads {
		l4_metadata.srcPort : exact;
		l4_metadata.dstPort : exact;
		ipv4.protocol       : exact;
	}
	actions { action_set_trap_id; _drop;}
}

table table_l3_trap_id {
	reads {
		ingress_metadata.trap_id : exact;
	}
	actions {action_copy_to_cpu; action_trap_to_cpu; _nop; _drop; }
}
table table_router {
	reads{
		router_metadata.ingress_vrf : exact;
		ipv4.dstAddr : lpm;
		// router_metadata.l3_lpm_key : lpm;
	}
	actions{action_set_nhop_id; action_set_nhop_grp_id; action_set_ip2me; _drop;}
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

table table_ttl {
	reads {
		ipv4.ttl : exact;
	}
	actions {action_dec_ttl; _drop;}
}

table table_neighbor {
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

table table_egress_clone_internal {
    reads {
        standard_metadata.instance_type : exact;
    }
    actions {_nop; action_cpu_encap;} 
    // size: 16;
}

//-----------
// cpu forwarding
//-----------
table table_cpu_forward {
    reads {
        cpu_header : valid;
    }
    actions {action_forward_cpu;}
}