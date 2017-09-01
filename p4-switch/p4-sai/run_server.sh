#!/bin/sh
handler()
{
    sudo kill -s INT $ROUTER_PID
    sudo kill -s INT $BRIDGE_PID
}

set -m
rm -rf logs/*.txt
sudo -v
sudo ip netns exec sw_net ./simple_switch -i 0@router_port1 -i 8@router_cpu_port --thrift-port 9091 --log-file logs/router_log --log-flush --pcap --notifications-addr ipc:///tmp/bmv2-router-notifications.ipc sai_router.json &
export ROUTER_PID=$!
sudo ip netns exec sw_net ./simple_switch -i 0@sw_port0 -i 1@sw_port1 -i 2@sw_port2 -i 3@sw_port3 -i 4@sw_port4 -i 5@sw_port5 -i 6@sw_port6 -i 7@sw_port7 -i 8@cpu_port -i 9@router_port0 --log-file logs/bridge_log --log-flush --pcap sai_bridge.json &
export BRIDGE_PID=$!
sleep 2
./runtime_CLI --pre SimplePreLAG < p4src/bridge/DefaultConfig.txt
./sswitch_CLI < p4src/bridge/DefaultConfigMirror.txt
./runtime_CLI --pre SimplePreLAG --thrift-port 9091 < p4src/router/DefaultConfig.txt
./sswitch_CLI --thrift-port 9091 < p4src/router/DefaultConfigMirror.txt

echo "router and bridge are running send SIGINT to close"

trap handler INT
sleep inf
# sudo kill -s SIGINT 3319
