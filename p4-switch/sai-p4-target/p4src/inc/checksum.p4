control computeChecksum(inout hdr headers, inout metadata meta) {
	Checksum16() ck;
	apply {
		headers.ipv4.checksum = ck.get({headers.ipv4.version,
	            headers.ipv4.ihl,
	            headers.ipv4.diffserv,
	            headers.ipv4.ipv4_length,
	            headers.ipv4.id,
	            headers.ipv4.flags,
	            headers.ipv4.offset,
	            headers.ipv4.ttl,
	            headers.ipv4.protocol,
	            headers.ipv4.srcAddr,
	            headers.ipv4.dstAddr});
	}
}

	// apply {
	    // update_checksum(true,
	    //     { hdr.ipv4.version,
	    //         hdr.ipv4.ihl,
	    //         hdr.ipv4.diffserv,
	    //         hdr.ipv4.totalLen,
	    //         hdr.ipv4.identification,
	    //         hdr.ipv4.flags,
	    //         hdr.ipv4.fragOffset,
	    //         hdr.ipv4.ttl,
	    //         hdr.ipv4.protocol,
	    //         hdr.ipv4.srcAddr,
	    //         hdr.ipv4.dstAddr,
	    //         hdr.ipv4.options
	    //     },
	    //     hdr.ipv4.hdrChecksum, HashAlgorithm.csum16);
	// }