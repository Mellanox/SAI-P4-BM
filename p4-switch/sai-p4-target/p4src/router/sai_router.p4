// This is P4 source for sai bridge


// includes
#include "../inc/headers.p4"
#include "../inc/parser.p4"
#include "../inc/defines.p4"
#include "../inc/field_lists.p4"
#include "tables.p4"
#include "actions.p4"

// metadata
metadata 	router_metadata_t 	 router_metadata;

control ingress {
	if (ingress_metadata.cpu_port == 0) {
		control_1q_uni_router();
	}
}

control control_1q_uni_router{
	apply(table_ingress_l3_if);
	apply(table_ingress_vrf);
	// apply(table_L3_ingress_acl); TODO
	apply(table_pre_l3_trap) {

		miss {
			apply(table_router){
				action_set_nhop_grp_id{
					apply(table_next_hop_group);
				}
			}
			if (router_metadata.ip2me == 1) {
				apply(table_ip2me_trap);
			} else {
				apply(table_next_hop);	
			}
		}
	}
	apply(table_l3_trap_id);
}


control egress{
	//apply(table_erif_check); TODO - mtu size, etc..
	// apply(table_L3_egress_acl); TODO

	if (ingress_metadata.cpu_port == 1) {
		control_cpu();
	} else {
		apply(table_egress_clone_internal) {
			miss {
				apply(table_ttl);
				apply(table_neighbor);
				apply(table_egress_L3_if);
			}
		}
	}
}

control control_cpu {
	if (cpu_header.netdev_type == VLAN) {
		apply(table_cpu_forward);
	}
}
	

// control control_unicast_fdb{
// 	apply(table_learn_fdb); //TODO: is this only relevant for unicast?
// 	apply(table_l3_interface){//should be for unicast only TDB
// 		miss{ 
// 				apply(table_fdb) {
// 					miss { 
// 						apply(table_flood);
// 					}
// 				}
// 			}
// 	 }
// }


