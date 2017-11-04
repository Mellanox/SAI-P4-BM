header ethernet_h {
        bit<48> dstAddr;
        bit<48> srcAddr;
        bit<16> etherType;
}
header vlan_h {
        bit<3> 	pcp;
	    bit 	cfi;
	    bit<12>	vid;
	    bit<16>	etherType;
}

header ipv4_h {
        bit<4> 		version;
        bit<4> 		ihl;
        bit<8> 		diffserv;
        bit<16> 	ipv4_length;
        bit<16> 	id;
        bit<3> 		flags;
        bit<13> 	offset;
        bit<8> 		ttl;
        bit<8> 		protocol;
        bit<16> 	checksum;
        bit<32> 	srcAddr;
        bit<32> 	dstAddr;
}

header tcp_h {
        bit<16> 	srcPort;
        bit<16> 	dstPort;
        bit<32> 	seqNo;
        bit<32> 	ackNo;
        bit<4> 		dataOffset;
        bit<4> 		res;
        bit<8> 		flags;
        bit<16> 	window;
        bit<16> 	checksum;
        bit<16> 	urgentPtr;
}

header udp_h {
        bit<16> 	srcPort;
        bit<16> 	dstPort;
        bit<16> 	length_;
        bit<16> 	checksum;
}

header arp_ipv4_h {
    bit<16> hwType;
    bit<16> protoType;
    bit<8>  hwAddrLen;
    bit<8>  protoAddrLen;
    bit<16> opcode;
    bit<48> srcHwAddr;
    bit<32> srcProtoAddr;
    bit<48> dstHwAddr;
    bit<32> dstProtoAddr;
}

header cpu_header_h {
    bit<16>  dst;
    bit<16>  trap_id;
    bit<6>   reserved;
    bit<2>   netdev_type;
}

// headers
struct hdr {
   cpu_header_h     cpu_header;
   ethernet_h       ethernet;
   vlan_h           vlan;
   ipv4_h           ipv4;
   tcp_h            tcp;
   udp_h            udp; 
   arp_ipv4_h       arp_ipv4; 
}


