rm *.svg
rm log.txt filtered_log.txt

echo Melding logs
python scripts/log_aggregation.py

echo Extracting packets flows
grep -E ': hit|: miss|Processing packet received|Action entry is ' log.txt > filtered_log.txt

echo Creating svg files
python scripts/animate_flow.py #-i filterd_log.txt -o visio_log -vis 

echo Presenting svg with firefox
# firefox flow_1q_packet_0.svg flow_router_uni_packet_0.svg &