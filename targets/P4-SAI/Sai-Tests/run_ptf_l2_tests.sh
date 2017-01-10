sudo ./ptf/ptf --test-dir tests/ --pypath sai_thrift_src/gen-py --interface 0@veth1 --interface 1@veth3 --interface 2@veth5 \
    	 --interface 3@veth7 --interface 4@veth9 --interface 5@veth11 --interface 6@veth13 --interface 7@veth15  \
    	 sail2_new.L21DBridgeBasicTest sail2_new.L21QBridgeAccess2AccessTest  sail2_new.L21QBridgeAccess2TrunkTest sail2_new.L21QBridgeTrunk2TrunkTest sail2_new.L21DLagTest sail2_new.L21QLagTest
    	 