 sudo ip netns exec host5 teamd -k -t team2
 sudo ip netns exec host5 ip link set host_port0 up
 sudo ip netns exec host5 ip link set host_port1 up
 sudo ip netns exec host5 ip link set host_port2 up