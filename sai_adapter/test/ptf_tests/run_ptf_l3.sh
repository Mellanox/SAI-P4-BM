sudo ./ptf/ptf --test-dir tests_old/ --pypath sai_thrift_src/gen-py --test-params port_map_file=\"port_map.ini\" --interface 0@host_port0 --interface 1@host_port1 --interface 2@host_port2 \
    	--interface 3@host_port3 --interface 4@host_port4 --interface 5@host_port5 --interface 6@host_port6 --interface 7@host_port7 --interface 8@vlan1 \
    	sail3.ArpTest
    	# sail3.L3IPv4HostTest
    	