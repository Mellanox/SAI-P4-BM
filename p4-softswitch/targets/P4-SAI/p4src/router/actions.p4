#include "../inc/defines.p4"

// primitives
action _drop() {
    drop();
}

action _nop() {
	no_op();
}

action action_set_irif(in bit<3> ingress_rif) {
	router_metadata.ingress_rif = ingress_rif;
}

action action_set_vrf(in bit<8> vrf) {
	router_metadata.ingress_vrf = vrf;
	// router_metadata.l3_lpm_key = (bit<40>)(vrf<<32)+(bit<40>)(ipv4.dstAddr);
}

action action_copy_to_cpu() {
	clone_ingress_pkt_to_egress(COPY_TO_CPU_MIRROR_ID, redirect_router_FL);
}

action action_trap_to_cpu() {
	clone_ingress_pkt_to_egress(8, redirect_router_FL);
	drop();
}

action action_set_trap_id(bit<11> trap_id) {
	ingress_metadata.trap_id = trap_id;
}

action action_dec_ttl() {
	ipv4.ttl = ipv4.ttl - 1;
}
// action action_set_acl_id(){
	
//}

action action_set_nhop_grp_id(in bit<3> next_hop_group_id){
	router_metadata.next_hop_group_id = next_hop_group_id;
}

action action_set_nhop_id(in bit<8> next_hop_id){
	router_metadata.next_hop_id = next_hop_id;
}

action action_set_ip2me() {
	router_metadata.ip2me = 1;
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

action action_set_smac_vid(in bit<48> smac, in bit<12> vid){
	ethernet.srcAddr = smac;
	// add_header(vlan);
	// vlan.etherType = IPV4_TYPE;
	// ethernet.etherType = VLAN_TYPE;
	vlan.vid = vid;
	standard_metadata.egress_spec = VLAN_IF;
}

action action_cpu_encap() { 
	add_header(cpu_header);
	cpu_header.dst = standard_metadata.ingress_port;
	cpu_header.netdev_type = VLAN;
	cpu_header.trap_id = ingress_metadata.trap_id;
	standard_metadata.egress_spec = COPY_TO_CPU_MIRROR_ID;
}

action action_forward_cpu() {
	standard_metadata.egress_spec = cpu_header.dst;
	remove_header(cpu_header);
}
