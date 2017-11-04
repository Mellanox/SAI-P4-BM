action drop() {
	 mark_to_drop();
}

action nop() {}

action action_set_trap_id(bit<16> trap_id) {
	meta.ingress_metadata.trap_id = trap_id;
}
