rm -f log_16.txt
sudo ip netns exec sw_net ./simple_switch -i 0@sw_port0 -i 1@sw_port1 --log-file log_16 --log-flush sai_bm_16.json