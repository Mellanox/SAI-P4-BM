action drop() {
	 mark_to_drop();
}

action nop() {}

action action_set_trap_id(bit<16> trap_id) {
	meta.ingress_metadata.trap_id = trap_id;
}


action action_copy_to_cpu() {
	// clone_ingress_pkt_to_egress(COPY_TO_CPU_MIRROR_ID, redirect_bridge_FL);
}

action action_trap_to_cpu() {
	// clone_ingress_pkt_to_egress(COPY_TO_CPU_MIRROR_ID, redirect_bridge_FL);
	drop();
}