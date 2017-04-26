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
metadata 	router_metadata_t 	 router_metadata;

control ingress {
	control_1q_uni_router();
}

control control_1q_uni_router{
	apply(table_ingress_L3_if);
	apply(table_ingress_vrf);
	// apply(table_L3_ingress_acl); TODO
	apply(table_router);
	//apply(table_next_hop_group); TODO
	apply(table_next_hop);
	//apply(table_erif_check); TODO - mtu size, etc..
	apply(table_Neigh);
	apply(table_egress_L3_if);
	// apply(table_L3_egress_acl); TODO
}


control egress{
	if(router_metadata.rif_type == BRIDGE_PORT_1D){
		apply(table_egress_1D_bridge);
	} 
	else if(router_metadata.rif_type == BRIDGE_PORT_1Q)
	{
		apply(table_egress_1Q_bridge);
	}
	else{
		apply(table_egress_router_port);	// router port
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


