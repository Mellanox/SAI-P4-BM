action _nop() {
	no_op();
}

action_set_ingress_rif(in bit<3> irif){
	router_metadata.ingress_rif	= irif;
}

action_set_vrf(in bit<8> vrif){
	router_metadata.ingress_vrif = vrif;
}

// action_set_acl_id(){
	
//}

action_set_pkt_type_set_nhop(){
	
}

action_set_erif_set_nh_dstip(){
	
}

// 	 action_chk_ttl_mtu(){
	
}
action_set_packet_dmac(){
	
}

//TODO :action_trap ;action_forward ;action_drop; action_copy_to_cpu}

action_set_smac_vid(){
	
}

