sudo ip netns exec host5 ip link set host_port0 down
sudo ip netns exec host5 ip link set host_port1 down
sudo ip netns exec host5 ip link set host_port2 down
sudo ip netns exec host5 teamd -d -f lacp_2.conf
sudo ip netns exec host5 ip link set team2 up
sudo ip netns exec host5 ip addr add 10.0.0.20/24 dev team2