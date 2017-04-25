#include "../inc/defines.p4"

table table_ingress_L3_if {
	reads{
		ingress_metadata.bridge_port 	: exact;
		ingress_metadata.vlan			: exact;
	}
	actions{action_set_ingress_rif}
	size: L3_IF_TABLE_SIZE; // TODO
}

table table_ingress_vrf {
	reads{
		ingress_metadata.rif : exact;
	}
	actions{action_set_vrf ; action_set_acl_id }
	size: ROUTER_IF_TABLE_SIZE;
}

table table_router {
	reads{
		ingress_metadata.l3_LPM_key : lpm;
	}
	actions{action_set_pkt_type_set_nhop}
	size: ROUTER_LPM_TABLE_SIZE;
}

table table_next_hop {
	reads{
		egress_metadata.next_hop : exact;
	}
	actions{ action_set_erif_set_nh_dstip }
	size: NHOP_TABLE_SIZE;
}

// table table_erif_check {
// 	reads{
// 		egress_metadata.erif : exact;
// 	}
// 	actions{ action_chk_ttl_mtu}
// 	size: ;
// }

table table_Neigh {
	reads{
		egress_metadata.erif 		: exact;
		egress_metadata.nh_dstip 	: exact;
	}
	actions{action_set_packet_dmac}
		//TODO :action_trap ;action_forward ;action_drop; action_copy_to_cpu}
	size: ;
}

table table_egress_L3_if {
	reads{
		egress_metadata.erif : exact;
	}
	actions{action_set_smac_vid }
	size: ;
}

// egress tables:
table  {
	reads{
		a : exact;
	}
	actions{ ; ; }
	size: ;
}