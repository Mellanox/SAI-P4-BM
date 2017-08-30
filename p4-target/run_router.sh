set -m
rm -rf logs/router_log.txt
sudo -v
sudo ip netns exec sw_net ./simple_switch -i 0@router_port1 --thrift-port 9091 --notifications-addr ipc:///tmp/bmv2-router-notifications.ipc sai_router.json &
sleep 2
./runtime_CLI --pre SimplePreLAG --thrift-port 9091 < p4src/router/DefaultConfig.txt
# ./sswitch_CLI < p4src/router/DefaultConfigMirror.txt
fg