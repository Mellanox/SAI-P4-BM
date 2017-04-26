set -m
rm -rf logs/router_log.txt
sudo -v
sudo ip netns exec sw_net ./simple_switch -i 0@router_port1 --log-file logs/router_log --log-flush --pcap --thrift-port 9091 sai_router.json &
sleep 2
./runtime_CLI --pre SimplePreLAG < p4src/router/DefaultConfig.txt
# ./sswitch_CLI < p4src/router/DefaultConfigMirror.txt
fg