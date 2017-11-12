struct ingress_metadata_t {
	bit<8> 	port;	//PHY_PORT_NUM_WDT
	bit<8> 	l2_if;	//PHY_PORT_NUM_WDT
	bit 	is_tagged;
	bit 	is_lag;
	bit<16> lag_id; // LAG_WDT
	bit     bind_mode; 
	bit<2> 	l2_if_type;
	bit<8> 	bridge_port; //L2_BRIDGE_PORT_WDT
	bit<12> bridge_id;	 //L2_BRIDGE_NUM_WDT
	bit<3> 	stp_state;
	bit<3>	stp_id; // TODO size?
	bit<12> vid;
    bit<2>  mcast_mode;
    bit<1>  mc_fdb_miss;
    bit     ipmc;
    bit     isip;
    bit<32> mtu;
    bit     drop_tagged;
    bit     drop_untagged;
    bit<16> trap_id;
    bit     cpu_port;
    bit<64> parse_cpu;
}

struct egress_metadata_t {
	bit<2> 	out_if_type; 
    bit<8> 	out_if; // PHY_PORT_NUM_WDT TODO remove? same as standard_metadata.egress_spec?
    bit<2> 	stp_state; // same as ingress? duplication?
    bit  	tag_mode;
    bit<6> 	hash_val;// TODO for egress lag table, when it is set?
    bit<8>  bridge_port; //L2_BRIDGE_PORT_WDT 
}

// struct intrinsic_metadata_t {
//     bit <48> ingress_global_timestamp;
//     bit <8> lf_field_list;
//     bit <16> mcast_grp;
//     bit <16> egress_rid;
//     bit <8> resubmit_flag;
//     bit <8> recirculate_flag;
// }

struct router_metadata_t{
    bit<8> ingress_rif;
    bit<8> egress_rif;
    bit<3> erif_type;
    bit<8> ingress_vrf;
    bit<32> next_hop_dst_ip;
    bit<8> next_hop_id;
    bit<3> next_hop_group_id;
    bit<3> next_hop_hash;
    bit    nhop_table;
        // bit<2> packet_action;
        // bit<40> l3_lpm_key;
   
}

struct l4_metadata_t {
    bit<16> srcPort;
    bit<16> dstPort;
}

struct metadata {
	ingress_metadata_t   ingress_metadata;
	egress_metadata_t    egress_metadata;
	// intrinsic_metadata_t intrinsic_metadata;
	router_metadata_t    router_metadata;
	l4_metadata_t        l4_metadata;
}
