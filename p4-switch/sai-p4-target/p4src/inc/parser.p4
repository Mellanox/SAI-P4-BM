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
        meta.ingress_metadata.cpu_port = (bit) (standard_metadata.ingress_port == (bit<9>) COPY_TO_CPU_MIRROR_ID);
        transition select(meta.ingress_metadata.cpu_port){
            1 : parse_cpu;
            default: parse_ethernet;
        }
    }

    state parse_cpu {
        b.extract(p.cpu_header);
        transition parse_ethernet;
    }

    state parse_ethernet {
        b.extract(p.ethernet);
        meta.ingress_metadata.is_tagged = 0;
        transition select(p.ethernet.etherType){
            ETHERTYPE_VLAN : parse_vlan;
            ETHERTYPE_IPV4 : parse_ipv4;
            ETHERTYPE_ARP  : parse_arp_ipv4;
            default: accept;
        }
    }

    state parse_arp_ipv4 {
        b.extract(p.arp_ipv4);
        transition accept;
    }

    state parse_vlan {
        b.extract(p.vlan);
        meta.ingress_metadata.is_tagged = (bit) (p.vlan.vid != 0);
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
        meta.l4_metadata.srcPort = p.udp.srcPort;
        meta.l4_metadata.dstPort = p.udp.dstPort;
        transition accept;
    }

    state parse_tcp {
    	b.extract(p.tcp);
        meta.l4_metadata.srcPort = p.tcp.srcPort;
        meta.l4_metadata.dstPort = p.tcp.dstPort;
        transition accept;
    }
}

control fixDeparser(packet_out b,in hdr p) {
    apply {
        b.emit(p.cpu_header);
        b.emit(p.ethernet);
        b.emit(p.vlan);
        b.emit(p.arp_ipv4);
        b.emit(p.ipv4);
        b.emit(p.udp);
        b.emit(p.tcp);
    }
}



