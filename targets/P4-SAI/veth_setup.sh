#!/bin/bash
for idx in 0 1 2 3 4 5 6 7 8; do
    intf0="sw_port$(($idx))"
    intf1="host_port$(($idx))"
    net="host_net$(($idx))"
    ip="10.0.0.$(($idx))/24"
    if ! ip link show $intf0 &> /dev/null; then
        ip link add name $intf0 type veth peer name $intf1
        ip link set dev $intf0 up
    	ip netns add $net
        ip link set dev $intf1 netns $net
    	ip netns exec $net ip link set dev $intf1 up
    	ip netns exec $net ip address add $ip dev $intf1
    fi
    # ip link set dev $intf1 netns $net up
done

