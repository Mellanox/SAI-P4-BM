const bit<8> TCP_PROTOCOL_NUM  = 0x06;
const bit<8> UDP_PROTOCOL_NUM  = 0x11;

const bit<16> ETHERTYPE_IPV4 = 0x0800;
const bit<16> ETHERTYPE_IPV6 = 0x86dd;
const bit<16> ETHERTYPE_VLAN = 0x8100;
const bit<16> ETHERTYPE_ARP  = 0x0806;

parser fixParser(packet_in b,
                out hdr p,
                inout metadata meta,
                inout standard_metadata_t standard_metadata)
{
    state start{
        b.extract(p.ethernet);
        transition select(p.ethernet.etherType){
            ETHERTYPE_VLAN : parse_vlan;
            ETHERTYPE_IPV4 : parse_ipv4;
            default: accept;
        }
    }
    state parse_vlan {
        b.extract(p.vlan);
        transition select(p.vlan.etherType) {
            ETHERTYPE_IPV4 : parse_ipv4;
            default: accept;
        }
    }

    state parse_ipv4 {
        b.extract(p.ipv4);
        transition select(p.ipv4.protocol) {
            TCP_PROTOCOL_NUM 	: parse_tcp;
            UDP_PROTOCOL_NUM	: parse_udp;
            default: accept;
        }
    }

    state parse_udp {
    	b.extract(p.udp);
        transition accept;
    }

    state parse_tcp {
    	b.extract(p.tcp);
        transition accept;
    }
}

control fixDeparser(packet_out b,in hdr p) {
    apply {
        b.emit(p.ethernet);
        b.emit(p.vlan);
        b.emit(p.ipv4);
        b.emit(p.udp);
        b.emit(p.tcp);
    }
}



