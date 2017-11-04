control ingress(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"
    table table_in_port {
        key = {
            standard_metadata.ingress_port : exact;
        }
        actions = {drop; nop;}
    }
    apply { 
        table_in_port.apply();
    }
}

control egress(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"
    table table_out_port {
        key = {
            standard_metadata.egress_spec : exact;
        }
        actions = {drop; nop;}
    }
    apply { 
        table_out_port.apply();
    }
}