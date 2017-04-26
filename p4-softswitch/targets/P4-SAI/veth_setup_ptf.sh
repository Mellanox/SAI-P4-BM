#!/bin/bash
sudo -v
sw_net="sw_net"
sudo ip netns add $sw_net
for idx in 0 1 2 3 4 5 6 7; do
    intf0="sw_port$(($idx))"
    intf1="host_port$(($idx))"
    if ! ip link show $intf0 &> /dev/null; then
        sudo ip link add name $intf0 type veth peer name $intf1
        sudo ip link set dev $intf0 netns $sw_net
        sudo ip netns exec $sw_net ip link set dev $intf0 up
    	sudo ip link set dev $intf1 up
    fi
done

intf0="cpu_port"
intf1="switch_port"
sudo ip link add name $intf0 type veth peer name $intf1
sudo ip link set dev $intf0 netns $sw_net
sudo ip netns exec $sw_net ip link set dev $intf0 up
sudo ip link set dev $intf1 up

intf0="router_port0"
intf1="router_port1"
sudo ip link add name $intf0 type veth peer name $intf1
sudo ip link set dev $intf0 netns $sw_net
sudo ip link set dev $intf1 netns $sw_net
sudo ip netns exec $sw_net ip link set dev $intf0 up
sudo ip netns exec $sw_net ip link set dev $intf1 up

