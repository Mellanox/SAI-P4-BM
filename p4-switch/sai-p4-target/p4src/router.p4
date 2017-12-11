control ingress_router(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"

    action action_set_vrf(bit<8> vrf) {
        meta.router_metadata.ingress_vrf = vrf;
        // meta.router_metadata.l3_lpm_key = (bit<40>)(vrf<<32)+(bit<40>)(headers.ipv4.dstAddr);
    }

    table table_ingress_vrf {
        key = {
            meta.router_metadata.ingress_rif : exact;
        }
        actions = {action_set_vrf; drop;} //; action_set_acl_id }
        //size: ROUTER_IF_TABLE_SIZE;
    }

    table table_pre_l3_trap {
        key = {
            headers.ethernet.etherType : ternary; //TODO: take inner ethertype (ethernet \ vlan \ vlan2)
            headers.ipv4.dstAddr   : lpm;
            headers.arp_ipv4.opcode     : ternary;
        }
        actions = { action_set_trap_id; drop;}
    }

    table table_ip2me_trap {
        key = {
            meta.l4_metadata.srcPort : exact;
            meta.l4_metadata.dstPort : exact;
            headers.ipv4.protocol       : exact;
        }
        actions = { action_set_trap_id; drop;}
    }

    action action_trap_to_cpu() {
        meta.egress_metadata.netdev_type = NETDEV_TYPE_VLAN; 
        meta.egress_metadata.clone_dst = (bit<16>) meta.ingress_metadata.vid;
        action_trap_to_cpu_common();
    }

    action action_copy_to_cpu() {
        meta.egress_metadata.netdev_type = NETDEV_TYPE_VLAN; 
        meta.egress_metadata.clone_dst = (bit<16>) meta.ingress_metadata.vid;
        action_copy_to_cpu_common();
    }

    table table_l3_trap_id {
        key = {
            meta.ingress_metadata.trap_id : exact;
        }
        actions = {action_copy_to_cpu; action_trap_to_cpu; nop; drop; }
    }

    action action_set_nhop_grp_id(bit<3> next_hop_group_id){
        meta.router_metadata.next_hop_group_id = next_hop_group_id;
        // meta.router_metadata.nhop_table = 1;
    }

    action action_set_nhop_id(bit<8> next_hop_id){
        meta.router_metadata.next_hop_id = next_hop_id;
        meta.router_metadata.nhop_table = 1;
    }

    action action_set_erif_set_nh_dstip_from_pkt(bit<8> egress_rif, bit<3> erif_type){
        meta.router_metadata.next_hop_dst_ip = headers.ipv4.dstAddr;
        meta.router_metadata.egress_rif = egress_rif;
        meta.router_metadata.erif_type = erif_type;
    }

    action action_set_erif_set_nh_dstip(bit<32> next_hop_dst_ip , bit<8> egress_rif, bit<3> erif_type){
        meta.router_metadata.next_hop_dst_ip = next_hop_dst_ip;
        meta.router_metadata.egress_rif = egress_rif;
        meta.router_metadata.erif_type = erif_type;    
    }

    action action_set_ip2me() {}

    table table_router {
        key = {
            meta.router_metadata.ingress_vrf : exact;
            headers.ipv4.dstAddr : lpm;
            // meta.router_metadata.l3_lpm_key : lpm;
        }
        actions = {action_set_nhop_id; action_set_nhop_grp_id; action_set_ip2me; action_set_erif_set_nh_dstip_from_pkt; drop;}
        //size: ROUTER_LPM_TABLE_SIZE;
    }

    table table_next_hop_group {
        key = {
            meta.router_metadata.next_hop_group_id : exact;
            //meta.router_metadata.next_hop_hash  : exact; // TODO
        }
        actions = { action_set_nhop_id ; drop;}
        //size: NHOP_GRP_TABLE_SIZE;
    }


    table table_next_hop {
        key = {
            meta.router_metadata.next_hop_id : exact;
        }
        actions = { action_set_erif_set_nh_dstip; drop;}
        //size: NHOP_TABLE_SIZE;
    }

    apply{
        table_ingress_vrf.apply();
        // table_L3_ingress_acl.apply(); TODO
        if (!(table_pre_l3_trap.apply().hit)) {
            switch (table_router.apply().action_run) {
                action_set_nhop_grp_id : { table_next_hop_group.apply(); }
                action_set_ip2me: { table_ip2me_trap.apply(); }
            }
            if (meta.router_metadata.nhop_table == 1) {
                table_next_hop.apply();
            }
        }
        table_l3_trap_id.apply();
    }
}

control egress_router(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"
    // table table_erif_check {
    //  key = {
    //      meta.egress_metadata.erif : exact;
    //  }
    //  actions = { action_chk_ttl_mtu; drop;}
    //  size: ;
    // }

    action action_dec_ttl() {
        headers.ipv4.ttl = headers.ipv4.ttl - 1;
    }

    table table_ttl {
        key = {
            headers.ipv4.ttl : exact;
        }
        actions = {action_dec_ttl; drop;}
    }

    action action_egress_trap_to_cpu() {
        // clone_egress_pkt_to_egress(COPY_TO_CPU_MIRROR_ID, redirect_router_FL);
        // drop();
        // standard_metadata.egress_spec = COPY_TO_CPU_MIRROR_ID;
        // standard_metadata.egress_spec = 250;
    }

    action action_set_packet_dmac(bit<48> dmac){
        headers.ethernet.dstAddr = dmac;    
    }

    table table_neighbor {
        key = {
            meta.router_metadata.egress_rif      : exact;
            meta.router_metadata.next_hop_dst_ip     : exact;
        }
        actions = {action_set_packet_dmac; action_egress_trap_to_cpu; drop;}
            //TODO :action_trap ;action_forward ;action_drop; action_copy_to_cpu}
        //size: NEIGH_TABLE_SIZE;
    }

    action action_set_smac_vid(bit<48> smac, bit<12> vid, bit<8> bridge_port) {
        headers.ethernet.srcAddr = smac;
        meta.ingress_metadata.vid = vid;
        meta.ingress_metadata.bridge_port = bridge_port;
    }

    action action_set_smac_out_if(bit<48> smac, bit<8> out_if, bit<2> out_if_type) {
        headers.ethernet.srcAddr = smac;
        meta.egress_metadata.out_if        = out_if;
        meta.egress_metadata.out_if_type   = out_if_type;
        standard_metadata.egress_spec = (bit<9>) out_if; 
    }

    table table_egress_L3_if {
        key = {
            meta.router_metadata.egress_rif : exact;
        }
        actions = {action_set_smac_vid; action_set_smac_out_if; drop; } // TODO check if type is ok here - not mentioned in visio
        //size: L3_EGRESS_IF_TABLE_SIZE;
    }

    action action_cpu_encap() { 
        // add_header(headers.cpu_header);
        headers.cpu_header.setValid();
        headers.cpu_header.dst = (bit<16>) headers.vlan.vid;
        headers.cpu_header.netdev_type = NETDEV_TYPE_VLAN;
        // headers.ethernet.etherType = headers.vlan.etherType;
        // remove_header(vlan);
        // headers.vlan.setInvalid();
        headers.cpu_header.trap_id = meta.ingress_metadata.trap_id;
        standard_metadata.egress_spec = (bit<9>) COPY_TO_CPU_MIRROR_ID;
    }

    table table_egress_router_clone_internal {
        key = {
            standard_metadata.instance_type : exact;
        }
        actions = {nop; action_cpu_encap;} 
        // size: 16;
    }

    //-----------
    // cpu forwarding
    //-----------

    action action_forward_cpu() {
        headers.vlan.setValid();
        headers.vlan.etherType = headers.ethernet.etherType;
        headers.ethernet.etherType = ETHERTYPE_VLAN;
        headers.vlan.vid = (bit<12>) headers.cpu_header.dst;
        headers.cpu_header.setInvalid();
    }

    // table table_cpu_forward {
    //     key = {
    //         headers.cpu_header.isValid() : exact;
    //     }
    //     actions = {action_forward_cpu;}
    // }

    apply{
        if (!(table_egress_router_clone_internal.apply().hit)) {
            table_ttl.apply();
            table_neighbor.apply();
            table_egress_L3_if.apply();
        }
    }
}