sudo ./ptf/ptf --test-dir tests-sai-repo/ --pypath sai_thrift_src/gen-py --test-params "port_map_file='port_map.ini'" --interface 0@host_port0 --interface 1@host_port1 --interface 2@host_port2 \
    	--interface 3@host_port3 --interface 4@host_port4 --interface 5@host_port5 --interface 6@host_port6 --interface 7@host_port7  \
    	sail2.L2FloodTest
    	# sail2.L2AccessToAccessVlanTest  sail2.L2TrunkToTrunkVlanTest sail2.L2AccessToTrunkVlanTest sail2.L2TrunkToAccessVlanTest sail2.L2FloodTest sail2.L2LagTest sail2.L2VlanBcastUcastTest