sudo ip link add name "port0_netdev" type dummy
sudo ip link add name "port1_netdev" type dummy
sudo ip link set port0_netdev down
sudo ip link set port1_netdev down

sudo teamd -d -f lacp_1.conf