#!/bin/bash
for idx in 0 1 2 3 4 5 6 7 8; do
    intf0="sw_port$(($idx))"
    intf1="host_port$(($idx))"
    net="host_net$(($idx))"
    if ip link show $intf0 &> /dev/null; then
        ip link delete $intf0 type veth
    fi
    if ip link show $intf1 &> /dev/null; then
        ip link delete $intf1 type veth
    fi
    if ip netns show $net &> /dev/null; then
        ip netns delete $net 
    fi
done
