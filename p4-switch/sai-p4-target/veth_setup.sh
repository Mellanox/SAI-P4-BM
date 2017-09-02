#!/bin/bash
sw_net="sw_net"
ip netns add $sw_net
for idx in 0 1 2 3 4 5 6 7; do
    intf0="sw_port$(($idx))"
    intf1="host_port$(($idx))"
    if ! ip link show $intf0 &> /dev/null; then
        ip link add name $intf0 type veth peer name $intf1
        ip link set dev $intf0 netns $sw_net
        ip netns exec $sw_net ip link set dev $intf0 up
    	ip link set dev $intf1 up
    	ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
    	sysctl net.ipv6.conf.$intf1.disable_ipv6=1
    fi
done

intf0="cpu_port"
intf1="switch_port"
ip link add name $intf0 type veth peer name $intf1
ip link set dev $intf0 netns $sw_net
ip netns exec $sw_net ip link set dev $intf0 up
ip link set dev $intf1 up
ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
sysctl net.ipv6.conf.$intf1.disable_ipv6=1

intf0="router_port0"
intf1="router_port1"
ip link add name $intf0 type veth peer name $intf1
ip link set dev $intf0 netns $sw_net
ip link set dev $intf1 netns $sw_net
ip netns exec $sw_net ip link set dev $intf0 up
ip netns exec $sw_net ip link set dev $intf1 up
ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
ip netns exec $sw_net sysctl net.ipv6.conf.$intf1.disable_ipv6=1

intf0="router_cpu_port"
intf1="router_port"
ip link add name $intf0 type veth peer name $intf1
ip link set dev $intf0 netns $sw_net
ip netns exec $sw_net ip link set dev $intf0 up
ip link set dev $intf1 up
ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
sysctl net.ipv6.conf.$intf1.disable_ipv6=1