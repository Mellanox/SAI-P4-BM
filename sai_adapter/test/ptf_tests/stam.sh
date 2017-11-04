sudo ip link set host_port5 down
sudo ip link set host_port3 down
sudo ip netns exec sw_net ip link set sw_port4 down
sudo ip netns exec sw_net ip link set sw_port5 down
sudo ip netns exec sw_net ip link set sw_port3 down
sudo ip link set host_port3 up
sudo ip netns exec sw_net ip link set sw_port3 up
sudo ip netns exec sw_net ip link set sw_port4 up
sudo ip netns exec sw_net ip link set sw_port5 up
sudo ip link set host_port5 up