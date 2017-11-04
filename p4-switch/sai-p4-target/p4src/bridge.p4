control ingress_bridge(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"
    table table_in_bridge  {
        key = {
            headers.ipv4.dstAddr : exact;
        }
        actions = {}
        size = 1;
    }
    apply{
        table_in_bridge.apply();
    }
}

control egress_bridge(inout hdr headers, inout metadata meta, inout standard_metadata_t standard_metadata){
    #include "inc/common_actions.p4"
    table table_out_bridge  {
        key = {
            headers.ethernet.dstAddr : ternary;
        }
        actions = {}
        size = 1;
    }
    apply{
       table_out_bridge.apply();
    }
}