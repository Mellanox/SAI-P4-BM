control ingress(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"
    
    action action_set_lag_l2if(bit is_lag, bit<8> l2_if) {
        meta.ingress_metadata.is_lag =   is_lag;
        meta.ingress_metadata.l2_if  =   l2_if;
    }

    action action_set_l2if() { 
        meta.ingress_metadata.l2_if = (bit<8>) standard_metadata.ingress_port;
    }

    table table_ingress_lag {
        key = {
            standard_metadata.ingress_port : exact;
        }
        actions = {action_set_lag_l2if;action_set_l2if;}
    }

    table table_drop_tagged_internal {
        key = {
            meta.ingress_metadata.drop_tagged : exact;
        }
        actions = {drop;nop;}
        size = 1;
    }

    table table_drop_untagged_internal {
        key = {
            meta.ingress_metadata.drop_untagged : exact;
        }
        actions = {drop;nop;}
        size = 1;
    }

    table table_l2_trap {
        key = {
            headers.ethernet.dstAddr : exact;
        }
        actions = {action_set_trap_id;}
    }

    action action_trap_to_cpu() {
        meta.egress_metadata.netdev_type = NETDEV_TYPE_PORT;
        meta.egress_metadata.clone_dst = (bit<16>) standard_metadata.ingress_port;
        // meta.egress_metadata.netdev_type = NETDEV_TYPE_PORT;
        action_trap_to_cpu_common();
    }

    action action_copy_to_cpu() {
        meta.egress_metadata.netdev_type = NETDEV_TYPE_PORT;
        meta.egress_metadata.clone_dst = (bit<16>) standard_metadata.ingress_port;
        action_copy_to_cpu_common();
    }

    table table_trap_id { //TODO: move this?
        key = {
            meta.ingress_metadata.trap_id : exact;
        }
        actions = {drop;nop;action_copy_to_cpu;action_trap_to_cpu;} 
    }

    action action_set_port_configurations(bit<12> pvid, bit bind_mode, bit<32> mtu, bit drop_tagged, bit drop_untagged) {
        meta.ingress_metadata.vid = pvid;
        meta.ingress_metadata.bind_mode = bind_mode;
        meta.ingress_metadata.mtu = mtu;
        meta.ingress_metadata.drop_tagged = drop_tagged;
        meta.ingress_metadata.drop_untagged = drop_untagged;
    }

    table table_port_configurations {
        key = {
            meta.ingress_metadata.l2_if : exact;
        }
        actions = {action_set_port_configurations;}
    }

    action action_set_packet_vid(){
        meta.ingress_metadata.vid = headers.vlan.vid;
    }

    table table_port_set_packet_vid_internal {  
        key = {
            meta.ingress_metadata.is_tagged : exact;
        }
        actions = {action_set_packet_vid;}
        size = 1; 
    }

    action action_set_l2_if_type(bit<2> l2_if_type, bit<8> bridge_port){
        // L2_BRIDGE_PORT_WDT
        meta.ingress_metadata.l2_if_type = l2_if_type;
        meta.ingress_metadata.bridge_port = bridge_port; 
    }

    table table_port_ingress_interface_type {// should be 
        key = {
            meta.ingress_metadata.l2_if: exact;
        }
        actions = {action_set_l2_if_type; drop;}
    }

    table table_subport_ingress_interface_type {
        key = {
            meta.ingress_metadata.l2_if : exact;
            meta.ingress_metadata.vid   : exact;
        }
        actions = {action_set_l2_if_type; drop;}
    }

    action action_cpu_forward_to_vlan() { //forward to ingress 1Q bridge after setting vid
        meta.ingress_metadata.l2_if_type = L2_IF_1Q_BRIDGE;
        meta.ingress_metadata.vid = (bit<12>) headers.cpu_header.dst;
        meta.ingress_metadata.bridge_port = (bit<8>) COPY_TO_CPU_MIRROR_ID;//ROUTER_BRIDGE_PORT;
        headers.cpu_header.setInvalid();
    }

    table table_cpu_forward {
        key = {
            headers.cpu_header.netdev_type : exact;
        }
        actions = {action_cpu_forward_to_vlan;}
        const entries = {
            (NETDEV_TYPE_VLAN) : action_cpu_forward_to_vlan();
        }
    }

    apply { 
        if (meta.ingress_metadata.cpu_port == 0) {
            table_ingress_lag.apply(); //TODO: rename table?
            table_port_configurations.apply();
            // table_accepted_frame_type.apply();
            if (meta.ingress_metadata.is_tagged==1) { 
                // table_port_PVID.apply();
            // } else {
                table_port_set_packet_vid_internal.apply();
                table_drop_tagged_internal.apply();
            } else {
                table_drop_untagged_internal.apply();
            }
            // table_port_mode.apply();
            table_l2_trap.apply();
            table_trap_id.apply();
            // table_check_port_mtu.apply(; //TODO
            //table_ingress_acl.apply(); // TODO
            if(meta.ingress_metadata.bind_mode == PORT_MODE_PORT) 
                table_port_ingress_interface_type.apply();
            else
                table_subport_ingress_interface_type.apply();
        } else {
            table_cpu_forward.apply();
        }
    }
}

control egress(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata) {
    #include "inc/common_actions.p4"
    
    action action_forward_vlan_untag(){
        headers.ethernet.etherType = headers.vlan.etherType;
        headers.vlan.setInvalid();
    }

    action action_forward_vlan_tag(bit<3> pcp, bit cfi, bit<12> vid){
        headers.vlan.setValid();
        headers.vlan.pcp = pcp;
        headers.vlan.cfi = cfi;
        headers.vlan.vid = vid;
        headers.vlan.etherType = headers.ethernet.etherType;
        headers.ethernet.etherType = ETHERTYPE_VLAN;
        // meta.egress_metadata.tag_mode = tag_mode;
    }

    table table_egress_vlan_tag {
        key = {
            meta.egress_metadata.out_if : exact;
            meta.ingress_metadata.vid : exact;
            headers.vlan.isValid() : exact;
        }
        actions = {action_forward_vlan_tag; action_forward_vlan_untag; drop;nop;}
        //size : 1; // TODO
    }

    action action_set_lag_hash_size(bit<6> lag_size) {
       hash(meta.egress_metadata.hash_val,
             HashAlgorithm.crc16,
             (bit<6>)0,
             {headers.ethernet.srcAddr, headers.ethernet.dstAddr, headers.ipv4.srcAddr, headers.ipv4.dstAddr},
             lag_size);
    }
        // modify_field_with_hash_based_offset(meta.egress_metadata.hash_val, 0, HashAlgorithm.crc16, lag_size);
        // hash(meta.egress_metadata.hash_val, 
             // HashAlgorithm.crc16,
             // 16w0,
             // {headers.ethernet.srcAddr, headers.ethernet.dstAddr, headers.ipv4.srcAddr, headers.ipv4.dstAddr},
             // lag_size);
             // lag_hash_calculate(meta.egress_metadata.hash_val, 
             // 16w0,
             // {headers.ethernet.srcAddr, headers.ethernet.dstAddr, headers.ipv4.srcAddr, headers.ipv4.dstAddr},
             // lag_size);
    // }

    table table_lag_hash {
        key = {
            meta.egress_metadata.out_if : exact;
        }
        actions = {action_set_lag_hash_size;}
    }

    action action_set_out_port(bit<8> port){
        standard_metadata.egress_spec = (bit<9>) port;
    }

    table table_egress_lag {
        key = {
            meta.egress_metadata.out_if : exact;
            meta.egress_metadata.hash_val : exact;
        }
        actions = {action_set_out_port; drop;}
        //size : 1; // TODO
    }

    action action_cpu_encap() { 
        headers.cpu_header.setValid();
        headers.cpu_header.netdev_type = meta.egress_metadata.netdev_type;//NETDEV_TYPE_PORT;
        headers.cpu_header.dst = meta.egress_metadata.clone_dst;//(bit<16>) meta.ingress_metadata.bridge_port;
        headers.cpu_header.trap_id = meta.ingress_metadata.trap_id;
    }

    apply { 
        table_egress_vlan_tag.apply();
        if (meta.egress_metadata.out_if_type == OUT_IF_LAG) { 
            table_lag_hash.apply();
            table_egress_lag.apply();
        }
        if (standard_metadata.instance_type == 1) {
            action_cpu_encap();
        }
    }
}