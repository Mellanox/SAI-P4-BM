#!/bin/bash
sw_net="sw_net"
if ip netns show $sw_net &> /dev/null; then
    ip netns delete $sw_net 
fi
for idx in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31; do
    intf0="host_port$(($idx))"
    if ip link show $intf0 &> /dev/null; then
    	ip link delete $intf0 type veth
	fi
done

sw_intf="switch_port"
# net="hostif_net"
if ip link show $sw_intf &> /dev/null; then
    ip link delete $sw_intf type veth
fi
# if ip netns show $net &> /dev/null; then
    # ip netns delete $net 
# fi