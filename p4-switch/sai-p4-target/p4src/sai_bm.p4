#include <core.p4>
#include <sai_model.p4>
#include "inc/constants.p4"
#include "inc/headers.p4"
#include "inc/metadata.p4"
#include "inc/parser.p4"

control verifyChecksum(in hdr headers, inout metadata meta) {   
    apply {  }
}

control computeChecksum(inout hdr headers, inout metadata meta){
    apply{}
}

#include "port.p4"
#include "bridge.p4"
#include "router.p4"

SaiSwitch(
    fixParser(),
    verifyChecksum(),
    ingress() ,
    egress() ,
    ingress_bridge(),
    egress_bridge(),
    ingress_router(),
    egress_router(),
    computeChecksum(),
    fixDeparser()
    )main;