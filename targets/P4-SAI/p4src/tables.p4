#include "defines.p4"

table table_ingress_lag {
    reads {
        standard_metadata.ingress_port : exact;
    }
    actions {action_set_lag_l2if;}
    size : PHY_PORT_NUM;
}

table table_accepted_frame_type {
    reads {
        ingress_metadata.l2_if : exact;
        // ingress_metadata.is_tagged : exact;
        vlan : valid;
    }
    actions {action_set_pvid;action_set_packet_vid;_drop;}
    //size : 1; // TODO
}

table table_ingress_l2_interface_type {
    reads {
        ingress_metadata.l2_if : exact;
        ingress_metadata.vid   : exact;
    }
    actions {action_set_l2_if_type; _drop;}
    //size : 1; TODO
}

//-----------
// ingress 1d bridge
//-----------
table table_vbridge {
    reads {
        ingress_metadata.bridge_port : exact;
    }
    actions {action_set_bridge_id; _drop;}
    //size : 1; TODO
}

table table_vbridge_STP {
    reads {
        ingress_metadata.bridge_port : exact;
    }
    actions {action_set_stp_state;}
    //size : 1; TODO
}
//-----------
// ingress 1q bridge
//-----------

table table_ingress_vlan_filtering{
	reads{
		ingress_metadata.l2_if : exact;
        ingress_metadata.vid   : exact;
	}
	actions{_drop;_nop;}
}

table table_ingress_vlan{
    reads{
        ingress_metadata.vid   : exact;
    }
    actions{action_set_mcast_snp;}
}

table table_xSTP_instance{
	reads{
		ingress_metadata.vid   : exact;
	}
	actions{action_set_stp_id;}
}
table table_xSTP {
    reads {
        ingress_metadata.bridge_port : exact;
        ingress_metadata.stp_id 	 : exact;
    }
    actions {action_set_stp_state;}
    //size : 1; TODO
}

//-----------
// fdb
//-----------

table table_learn_fdb {
    reads {
        ethernet.srcAddr 			: exact;
        ingress_metadata.bridge_id 	: exact;
    }
    actions {_nop;action_learn_mac;}
    //size : 1; TODO
}

table table_l3_interface {
    reads {
    	ethernet.dstAddr			: exact;
    	ingress_metadata.bridge_id 	: exact;
    }
    actions {action_go_to_in_l3_if_table; }//action_go_to_fdb_table;}
    //size : 1; TODO
}
//---------
// unicast:
//---------
table table_fdb { // TODO ask if can be melded into l3 interface table...
    reads {
    	ethernet.dstAddr		   : exact;
        ingress_metadata.bridge_id : exact;
    }
    actions {action_forward_set_outIfType;}
    size : FDB_TABLE_SIZE;
}

table table_l3_if{ // TODO - definition
	reads{
		ethernet.dstAddr		   : exact;
        ingress_metadata.bridge_id : exact;
	}
	actions{action_forward;}//action_go_to_fdb_table;}
}

table table_unknown_unicast {
    reads {
        ingress_metadata.bridge_id : exact;
    }
    actions {action_forward;_drop;}
    //size : 1; // TODO
}
//---------
// multicast:
//---------
table table_mc_fdb{
    reads{
        ethernet.dstAddr           : exact;
        ingress_metadata.bridge_id : exact;
    }
    actions{action_forward_mc_set_if_list;action_set_mc_fdb_miss;}
}

table table_mc_l2_sg_g{// IP MC
    reads{
        ingress_metadata.bridge_id  : exact;
        ipv4.srcAddr                : exact;
        ipv4.dstAddr                : exact;
    }
    actions{action_forward_mc_set_if_list;action_set_mc_fdb_miss;}
}

table table_unknown_multicast{
    reads{
      ingress_metadata.bridge_id : exact;
    }
    actions{action_forward_mc_set_if_list;}
}
//-----------
// egress 1d bridge
//-----------

table table_egress_vbridge_STP {
    reads {
        ingress_metadata.bridge_port : exact; //TODO maybe egress? who set br_port?
    }
    actions {action_set_egress_stp_state; _drop;}
    //size : 1; // TODO
}

table table_egress_vbridge {
    reads {
        ingress_metadata.bridge_port : exact; //TODO maybe egress? who set br_port?
    }
    actions {action_set_vlan_tag_mode; _drop;}
    //size : 1; // TODO
}

//-----------
// egress 1q bridge
//-----------

table table_egress_xSTP{
    reads{
        egress_metadata.out_if  : exact;
        ingress_metadata.stp_id : exact;
    }
    actions {action_set_egress_stp_state; _drop;}
}

table table_egress_vlan_filtering{
    reads{
        egress_metadata.out_if  : exact;
        ingress_metadata.vid    : exact;
    }
    actions{_drop;action_tag_vlan;action_untag_vlan}
}

//-----------
// egress lag/phy
//-----------
table table_lag_hash { //TODO: FW flow to add ports as lag, should edit this table with lag size
    reads {
        egress_metadata.out_if : exact;
    }
    actions {action_set_lag_hash_size;}
}

table table_egress_lag {
    reads {
        egress_metadata.out_if : exact;
        egress_metadata.hash_val : exact;
    }
    actions {action_set_out_port; _drop;}
    //size : 1; // TODO
}

// table table_egress_port {
    // reads {
        // egress_metadata.out_if : exact;
    // }
    // actions {action_set_out_port;}
// }