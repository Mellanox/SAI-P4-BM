// Parser Defintion file.
// Defines how stream of bytes
// that enters the switch,
// get parsed into meaningfull packets. 

#define TCP_PROTOCOL_NUM	0x06
#define UDP_PROTOCOL_NUM	0x11
#define VLAN_TYPE 		0x8100
#define IPV4_TYPE		0x0800


// parser starts here
parser start {
    return parse_ethernet;
}

// ethernet parser - decide next layer according the ethertype
parser parse_ethernet {
    extract(ethernet);
    return parse_ethertype;
}

parser parse_ethertype {
    return select(latest.etherType) {
    	VLAN_TYPE : parse_vlan;
        IPV4_TYPE : parse_ipv4;
        default: ingress;
    }
}

parser parse_vlan {
    extract(vlan);
    return parse_ethertype;
}

// IPv4 parser, decide next layer according to protocol field
parser parse_ipv4 {
    extract(ipv4);
    return select(latest.protocol) {
        TCP_PROTOCOL_NUM 	: parse_tcp;
        UDP_PROTOCOL_NUM	: parse_udp;
        default 			: ingress;
    }
}

// TCP parser, next layer are irrelvant for us, thus not included
// (cosidered payload data).
parser parse_udp {
	extract(udp);
	return ingress;
}

parser parse_tcp {
	extract(tcp);
	return ingress;
}
