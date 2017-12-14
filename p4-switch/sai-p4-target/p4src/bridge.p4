control ingress_bridge(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"
    
    action action_set_bridge_id(bit<12> bridge_id){
        meta.ingress_metadata.bridge_id = bridge_id;
    }

    action action_set_bridge_id_vid(){
        meta.ingress_metadata.bridge_id = meta.ingress_metadata.vid;
    }

    action action_set_stp_id(bit<3> stp_id){
        meta.ingress_metadata.stp_id = stp_id;
    }

    action action_set_stp_state(bit<3> stp_state){ // TODO: need to enforce STP state  
        meta.ingress_metadata.stp_state = stp_state;
    }

    //-----------
    // ingress 1d bridge
    //-----------
    table table_bridge_id_1d {
        key =  {
            meta.ingress_metadata.bridge_port : exact;
        }
        actions = {action_set_bridge_id; drop;}
    }

    table table_vbridge_STP {
        key = {
            meta.ingress_metadata.bridge_port : exact;
        }
        actions = {action_set_stp_state;}
        //size : 1; TODO
    }

    //-----------
    // ingress 1q bridge
    //-----------

    table table_bridge_id_1q {
        key = {
            meta.ingress_metadata.vid : exact;
        }
        actions = {action_set_bridge_id;action_set_bridge_id_vid;}//why drop 
    }

    table table_ingress_vlan_filtering{
        key = {
            meta.ingress_metadata.bridge_port : exact;
            meta.ingress_metadata.vid   : exact;
        }
        actions = {drop;nop;}
    }

    table table_xSTP_instance{
        key = {
            meta.ingress_metadata.vid   : exact;
        }
        actions = {action_set_stp_id;}
    }

    table table_xSTP {
        key = {
            meta.ingress_metadata.bridge_port : exact;
            meta.ingress_metadata.stp_id      : exact;
        }
        actions = {action_set_stp_state;drop;}
        //size : 1; TODO
    }

    //-----------
    // fdb
    //-----------
    action action_learn_mac() {
        meta.ingress_metadata.trap_id = MAC_LEARN_TRAP_ID;   //TODO, should this be configurable to support hostif interface(?)
        meta.egress_metadata.netdev_type = NETDEV_TYPE_PORT; 
        meta.egress_metadata.clone_dst = (bit<16>) meta.ingress_metadata.bridge_port;
        action_copy_to_cpu_common();
        // clone3(CloneType.IB2E, COPY_TO_CPU_MIRROR_ID, {meta.ingress_metadata.trap_id, meta.egress_metadata.netdev_type, meta.egress_metadata.clone_dst});
    }

    table table_learn_fdb {
        key = {
            headers.ethernet.srcAddr            : exact;
            meta.ingress_metadata.bridge_id  : exact;
        }
        actions = {nop;action_learn_mac;}
        //size : 1; TODO
    }

    action action_set_egress_br_port(bit<8> br_port){
        meta.egress_metadata.bridge_port = br_port;
    }

    table table_l3_interface {
        key = {
            headers.ethernet.dstAddr            : exact;
            meta.ingress_metadata.bridge_id  : exact;
        }
        actions = {action_set_egress_br_port;}//action_go_to_fdb_table;}
        //size : 1; TODO
    }

    //---------
    // unicast:
    //---------
    table table_fdb { // TODO ask if can be melded into l3 interface table...
        key = {
            headers.ethernet.dstAddr           : exact;
            meta.ingress_metadata.bridge_id : exact;
        }
        actions = {action_set_egress_br_port;}
        // size : FDB_TABLE_SIZE;
    }

    //---------
    // multicast:
    //---------
    // action action_set_mcast_lookup_mode(bit<2> mcast_mode){
    //     meta.ingress_metadata.mcast_mode =mcast_mode;
    // }
    // table table_mc_lookup_mode{
    //     key = {
    //         meta.ingress_metadata.vid   : exact;
    //     }
    //     actions = {action_set_mcast_lookup_mode;}
    // }
    // table table_mc_fdb{
    //     key = {
    //         headers.ethernet.dstAddr           : exact;
    //         meta.ingress_metadata.bridge_id : exact;
    //     }
    //     actions = {action_forward_mc_set_if_list;action_set_mc_fdb_miss;}
    // }

    // table table_mc_l2_sg_g{// IP MC
    //     key = {
    //         meta.ingress_metadata.bridge_id  : exact;
    //         headers.ipv4.srcAddr                : exact;
    //         headers.ipv4.dstAddr                : exact;
    //     }
    //     actions = {action_forward_mc_set_if_list;action_set_mc_fdb_miss;}
    // }

    // table table_unknown_multicast_nonip{
    //     key = {
    //       meta.ingress_metadata.bridge_id : exact;
    //     }
    //     actions = {action_forward_mc_set_if_list;}
    // }

    // table table_unknown_multicast_ipv4{
    //     key = {
    //       meta.ingress_metadata.bridge_id : exact;
    //     }
    //     actions = {action_forward_mc_set_if_list;}
    // }

    //table table_unknown_multicast_ipv6{
    //    key = {
    //      meta.ingress_metadata.bridge_id : exact;
    //    }
    //    actions = {action_forward_mc_set_if_list;}
    //}

    //---------
    // broadcast / flood:
    //---------

    action action_forward_mc_set_if_list(bit<16> mcast_grp){
        standard_metadata.mcast_grp = mcast_grp;
    }

    table table_broadcast {
        key = {
          meta.ingress_metadata.bridge_id : exact;
        }
        actions = {action_forward_mc_set_if_list;}
    }

    table table_flood {
        key = {
            meta.ingress_metadata.bridge_id : exact;
        }
        actions = {action_forward_mc_set_if_list;}
    }

    apply{
        if(meta.ingress_metadata.l2_if_type == L2_IF_1D_BRIDGE) { //.1D bridge
            table_bridge_id_1d.apply();
            table_vbridge_STP.apply();
        } else{  //.1Q bridge
            table_bridge_id_1q.apply();
            table_ingress_vlan_filtering.apply();
            table_xSTP_instance.apply();
            table_xSTP.apply();
        }
        if ((meta.ingress_metadata.bridge_port != (bit<8>) COPY_TO_CPU_MIRROR_ID) && (meta.ingress_metadata.bridge_port != ROUTER_BRIDGE_PORT))
            table_learn_fdb.apply();
        if((headers.ethernet.dstAddr&0x010000000000)==0x0){   //unicast 
            if (!(table_l3_interface.apply().hit)) {
                if (!(table_fdb.apply().hit)) {
                    table_flood.apply();
                }
            }
        } else if(headers.ethernet.dstAddr==0xffffffffffff){  //broadcast
            table_broadcast.apply();
        } else { //multicast
            //  table_mc_lookup_mode.apply();
            // //non ip multicast 
            // if((meta.ingress_metadata.isip==0) or (meta.ingress_metadata.mcast_mode==MAC_BASE_MC_LOOKUP))//non ip or multicast mode == FDB
            //     table_mc_fdb.apply();
            // else if((meta.ingress_metadata.isip==1) and (meta.ingress_metadata.mcast_mode==SG_IP_BASE_MC_LOOKUP))
            //     table_mc_l2_sg_g.apply(); 
            // //TBD add * G table              
            // //FDB miss flow 
            // if(meta.ingress_metadata.mc_fdb_miss==1) {
            //     //non ip
            //     if(meta.ingress_metadata.isip==1)
            //         table_unknown_multicast_ipv4.apply();
            //     else 
            //         table_unknown_multicast_nonip.apply();
            // }
        }
    }
}

control egress_bridge(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"

    // --------------
    // egress bridge
    // --------------
    action action_forward_set_outIfType(bit<8> out_if,bit<2> out_if_type){
        meta.egress_metadata.out_if        = out_if;
        meta.egress_metadata.out_if_type   = out_if_type;
        standard_metadata.egress_spec = (bit<9>) out_if; 
    }

    table table_egress_br_port_to_if {
        key = {
            meta.egress_metadata.bridge_port : exact;
        }
        actions = {action_forward_set_outIfType; drop;}
    }

    table table_bridge_loopback_filter {
        key = {
            meta.egress_metadata.bridge_port : exact;
        }
        actions = {nop;drop;}
    }

    //-----------
    // egress 1d bridge
    //-----------
    action action_set_egress_stp_state(bit<2> stp_state){
        meta.egress_metadata.stp_state = stp_state;
    }

    table table_egress_vbridge_STP {
        key = {
            meta.egress_metadata.bridge_port : exact; 
        }
        actions = {action_set_egress_stp_state; drop;}
        //size : 1; // TODO
    }

    action action_set_vlan(bit<12> vid) {
        meta.ingress_metadata.vid = vid;
    }

    table table_egress_set_vlan {
        key = {
            meta.egress_metadata.bridge_port : exact;
        }
        actions = {action_set_vlan;}
    }

    //-----------
    // egress 1q bridge
    //-----------
    table table_egress_xSTP{
        key = {
            meta.egress_metadata.bridge_port  : exact;
            meta.ingress_metadata.stp_id : exact;
        }
        actions = {action_set_egress_stp_state; drop;}
    }

    table table_egress_vlan_filtering {
        key = {
            meta.egress_metadata.bridge_port  : exact;
            meta.ingress_metadata.vid    : exact;
        }
        actions = {drop; nop; } 
    }

    action action_set_irif(bit<8> ingress_rif) {
        meta.router_metadata.ingress_rif = ingress_rif;
    }
    
    table table_ingress_l3_vlan_if {
        key = {
            meta.ingress_metadata.vid : exact;
        }
        actions = {action_set_irif; drop;}
        //size: L3_EGRESS_IF_TABLE_SIZE;
    }

    apply{
       table_egress_br_port_to_if.apply();
        if ((meta.ingress_metadata.bridge_port == meta.egress_metadata.bridge_port) && (standard_metadata.instance_type != 1)) { // not cpu, TODO: better?
            table_bridge_loopback_filter.apply();
        }
        if(meta.ingress_metadata.l2_if_type == L2_IF_1D_BRIDGE){
            table_egress_vbridge_STP.apply();
            table_egress_set_vlan.apply();
        }
        if(meta.ingress_metadata.l2_if_type == L2_IF_1Q_BRIDGE){
            table_egress_xSTP.apply();
            table_egress_vlan_filtering.apply();
        }
        if(meta.egress_metadata.out_if_type == OUT_IF_ROUTER){
            table_ingress_l3_vlan_if.apply();
        }
    }
}