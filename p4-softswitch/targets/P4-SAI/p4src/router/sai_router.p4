// This is P4 source for sai bridge


// includes
#include "../inc/headers.p4"
#include "../inc/parser.p4"
#include "../inc/defines.p4"
#include "../inc/field_lists.p4"
#include "tables.p4"
#include "actions.p4"

// headers
header   ethernet_t 	  ethernet;
header   vlan_t 		  vlan;
header   ipv4_t 		  ipv4;
header   tcp_t 			  tcp;
header   udp_t			  udp;
header   cpu_header_t     cpu_header;  

// metadata
metadata 	ingress_metadata_t 	 ingress_metadata; // TODO remove - needed for parser - cpu host if
metadata 	router_metadata_t 	 router_metadata;

control ingress {
	control_1q_uni_router();
}

control control_1q_uni_router{
	apply(table_ingress_vrf);
	// apply(table_L3_ingress_acl); TODO
	apply(table_router){
		action_set_nhop_grp_id{
			apply(table_next_hop_group);
		}
	}
	apply(table_next_hop);	
	//apply(table_erif_check); TODO - mtu size, etc..
	apply(table_neighbor);
	apply(table_egress_L3_if);
	// apply(table_L3_egress_acl); TODO
}


control egress{
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


