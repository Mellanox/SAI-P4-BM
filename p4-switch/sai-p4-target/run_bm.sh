#!/bin/sh
handler()
{
    kill -s INT $ROUTER_PID
    kill -s INT $BRIDGE_PID
}

set -m
rm -rf logs/*.txt
ip netns exec sw_net ./simple_switch -i 0@router_port1 -i 250@router_cpu_port --thrift-port 9091 --log-file logs/router_log --log-flush --pcap --notifications-addr ipc:///tmp/bmv2-router-notifications.ipc sai_router.json &
export ROUTER_PID=$!
ip netns exec sw_net ./simple_switch -i 0@sw_port0 -i 1@sw_port1 -i 2@sw_port2 -i 3@sw_port3 -i 4@sw_port4 -i 5@sw_port5 -i 6@sw_port6 -i 7@sw_port7 -i 7@sw_port7 -i 8@sw_port8 -i 9@sw_port9 -i 10@sw_port10 -i 11@sw_port11 -i 12@sw_port12 -i 13@sw_port13 -i 14@sw_port14 -i 15@sw_port15 -i 16@sw_port16 -i 17@sw_port17 -i 18@sw_port18 -i 19@sw_port19 -i 20@sw_port20 -i 21@sw_port21 -i 22@sw_port22 -i 23@sw_port23 -i 24@sw_port24 -i 25@sw_port25 -i 26@sw_port26 -i 27@sw_port27 -i 28@sw_port28 -i 29@sw_port29 -i 30@sw_port30 -i 31@sw_port31 -i 250@cpu_port -i 251@router_port0 --log-file logs/bridge_log --log-flush --pcap sai_bridge.json &
export BRIDGE_PID=$!
sleep 2
./runtime_CLI --pre SimplePreLAG < default_config/bridge_default_config.txt
./sswitch_CLI < default_config/bridge_default_config_mirror.txt
./runtime_CLI --pre SimplePreLAG --thrift-port 9091 < default_config/router_default_config.txt
./sswitch_CLI --thrift-port 9091 < default_config/router_default_config_mirror.txt


echo "router and bridge are running send SIGINT to close"

trap handler INT
sleep inf
# sudo kill -s SIGINT 3319
