set -m
rm -rf logs/log.txt
sudo -v
# sudo ./simple_switch -i 0@sw_port0 -i 1@sw_port1 -i 2@sw_port2 -i 3@sw_port3 -i 4@sw_port4 -i 5@sw_port5 -i 6@sw_port6 -i 7@sw_port7 -i 8@cpu_port --log-file log --log-flush sai.json &
sudo ip netns exec sw_net ./simple_switch -i 0@sw_port0 -i 1@sw_port1 -i 2@sw_port2 -i 3@sw_port3 -i 4@sw_port4 -i 5@sw_port5 -i 6@sw_port6 -i 7@sw_port7 -i 8@cpu_port --log-file logs/log --log-flush --pcap sai.json &
sleep 2
sudo ./runtime_CLI --pre SimplePreLAG < p4src/DefaultConfig.txt
sudo ./sswitch_CLI < p4src/DefaultConfigMirror.txt
fg