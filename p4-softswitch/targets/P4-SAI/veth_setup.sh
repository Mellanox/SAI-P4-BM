#!/bin/bash
sudo -v
sw_net="sw_net"
sudo ip netns add $sw_net
for idx in 0 1 2 3 4; do
    intf0="sw_port$(($idx))"
    intf1="host_port0"
    net="host$(($idx))"
    ip="10.0.0.$(($idx))/24"
    if ! ip link show $intf0 &> /dev/null; then
        sudo ip link add name $intf0 type veth peer name $intf1
        sudo ip link set dev $intf0 netns $sw_net
        sudo ip netns exec $sw_net ip link set dev $intf0 up
    	sudo ip netns add $net
        sudo ip netns exec $net ip link set dev lo up
        sudo ip link set dev $intf1 netns $net
    	sudo ip netns exec $net ip link set dev $intf1 up
    	sudo ip netns exec $net ip address add $ip dev $intf1
	sudo ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
	sudo ip netns exec $net sysctl net.ipv6.conf.$intf1.disable_ipv6=1
        # sudo ip netns exec $net ip route add default via 10.0.0.100 dev $intf1
    fi
done

# Last host will have 3 ports, to enable LACP and lag test
net="host5"
sudo ip netns add $net
for port_idx in 0 1 2; do
	idx=port_idx+5
    intf0="sw_port$(($idx))"
    intf1="host_port$(($port_idx))"
    if ! ip link show $intf0 &> /dev/null; then
        sudo ip link add name $intf0 type veth peer name $intf1
        sudo ip link set dev $intf0 netns $sw_net
        sudo ip link set dev $intf1 netns $net
        sudo ip netns exec $sw_net ip link set dev $intf0 up
    	sudo ip netns exec $net ip link set dev $intf1 up
	sudo ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
    	sudo ip netns exec $net sysctl net.ipv6.conf.$intf1.disable_ipv6=1
    fi
done
# sudo ip netns exec $net ip route add default via 10.0.0.100 dev $intf1

# ip="10.0.0.5/24"
# intf1="host_port0"
# sudo ip netns exec $net ip address add $ip dev $intf1

intf0="cpu_port"
intf1="switch_port"
sudo ip link add name $intf0 type veth peer name $intf1
sudo ip link set dev $intf0 netns $sw_net
sudo ip netns exec $sw_net ip link set dev $intf0 up
sudo ip link set dev $intf1 up
sudo ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
sudo sysctl net.ipv6.conf.$intf1.disable_ipv6=1

intf0="router_port0"
intf1="router_port1"
sudo ip link add name $intf0 type veth peer name $intf1
sudo ip link set dev $intf0 netns $sw_net
sudo ip link set dev $intf1 netns $sw_net
sudo ip netns exec $sw_net ip link set dev $intf0 up
sudo ip netns exec $sw_net ip link set dev $intf1 up
sudo ip netns exec $sw_net sysctl net.ipv6.conf.$intf0.disable_ipv6=1
sudo ip netns exec $sw_net sysctl net.ipv6.conf.$intf1.disable_ipv6=1
