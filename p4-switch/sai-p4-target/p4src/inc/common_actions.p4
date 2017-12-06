action drop() {
	 mark_to_drop();
}

action nop() {}

action action_set_trap_id(bit<16> trap_id) {
	meta.ingress_metadata.trap_id = trap_id;
}


action action_copy_to_cpu_common() {
	// clone3(CloneType.IB2E, COPY_TO_CPU_MIRROR_ID, {meta.ingress_metadata.trap_id, meta.egress_metadata.netdev_type, meta.egress_metadata.clone_dst});
	clone(CloneType.IB2E, COPY_TO_CPU_MIRROR_ID);
}

action action_trap_to_cpu_common() {
	// clone3(CloneType.IB2E, COPY_TO_CPU_MIRROR_ID, {meta.ingress_metadata.trap_id, meta.egress_metadata.netdev_type, meta.egress_metadata.clone_dst});
	clone(CloneType.IB2E, COPY_TO_CPU_MIRROR_ID);
	drop();
}