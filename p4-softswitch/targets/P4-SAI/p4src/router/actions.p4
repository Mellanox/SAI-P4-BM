#include "../inc/defines.p4"

// primitives
action _drop() {
    drop();
}

action _nop() {
	no_op();
}


action action_set_vrf(in bit<8> vrf) {
	router_metadata.ingress_vrf = vrf;
	router_metadata.l3_lpm_key = (bit<40>)(vrf<<32)+(bit<40>)(ipv4.dstAddr);// + ipv4.dstAddr;
}

// action action_set_acl_id(){
	
//}

action action_set_nhop_grp_id(in bit<3> next_hop_group_id){
	router_metadata.next_hop_group_id = next_hop_group_id;
}

action action_set_nhop_id(in bit<8> next_hop_id, in bit<2>pkt_action){
	router_metadata.packet_action = pkt_action;
	router_metadata.next_hop_id = next_hop_id;
}

action action_set_erif_set_nh_dstip_from_pkt(in bit<3> egress_rif){
	router_metadata.next_hop_dst_ip = ipv4.dstAddr;
	router_metadata.egress_rif = egress_rif;
}

action action_set_erif_set_nh_dstip(in bit<32> next_hop_dst_ip , in bit<3> egress_rif){
	router_metadata.next_hop_dst_ip = next_hop_dst_ip;
	router_metadata.egress_rif = egress_rif;
	
}

// 	 action action_chk_ttl_mtu(){	
//}

action action_set_packet_dmac(in bit<48> dmac){
	ethernet.dstAddr = dmac;	
}

action action_trap_to_cpu(){

}

//TODO :action_trap ;action_forward ;action_drop; action_copy_to_cpu}

action action_set_smac_vid(in bit<48> smac, in bit<12> vid){
	ethernet.srcAddr = smac;
	vlan.vid = vid;
	standard_metadata.egress_spec = VLAN_IF;
}

