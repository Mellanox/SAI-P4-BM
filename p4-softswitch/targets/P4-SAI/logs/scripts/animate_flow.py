# deps:
# brew install imagemagick
# pip install Wand
import sys
import json
import re


#### MAPPING log to svg ###################
# create dict from log - TODO move to json
tables = {	'table_ingress_lag':'Ingress LAG table',
			'table_l2_trap':'',
			'table_trap_id':'',
			'table_port_configurations':'accepted frame type',
			'table_port_ingress_interface_type':'Ingress l2 interface type table',
			'table_subport_ingress_interface_type':'',
			'table_bridge_id_1d':'Bridge_id',
			'table_vbridge_STP':'',
			'table_bridge_id_1q':'Bridge_id',
			'table_ingress_vlan_filtering':'Ingress vlan filtering',
			'table_mc_lookup_mode':'',
			'table_xSTP_instance':'xSTP instance',
			'table_xSTP':'Ingress xSTP',
			'table_learn_fdb':'Learning FDB',
			'table_l3_interface':'L3 interface',
			'table_fdb':'FDB',
			'table_mc_fdb':'',
			'table_mc_l2_sg_g':'',
			'table_unknown_multicast_nonip':'',
			'table_unknown_multicast_ipv4':'',
			'table_broadcast':'',
			'table_flood':'',
			'table_egress_vbridge_STP':'',
			'table_egress_vlan_tag':'',
			'table_egress_xSTP':'Egress xSTP',
			'table_egress_vlan_filtering':'egress vlan filtering',
			'table_egress_br_port_to_if':'Egress  bridge port to  interface type',
			'table_egress_set_vlan':'',
			'table_lag_hash':'',
			'table_egress_lag':'Egress LAG',
			'table_cpu_forward':'',
			'table_ingress_l3_if':'',
			'table_ingress_vrf':'',
			'table_router':'',
			'table_next_hop_group':'',
			'table_next_hop':'',
			'table_neighbor':'',
			'table_egress_L3_if':'',
		}




#### GETTING hit/miss per svg table ###################
table_names_list = re.compile('|'.join(re.escape(key) for key in tables))

log_path = "filtered_log.txt"
hit_dict = {}
with open(log_path,'r') as log:
	log_lines = log.readlines()
	for line in log_lines:
		if "Table" in line:
			for match in table_names_list.finditer(line):
				if tables.get(match.group(0)) != "":
					if line.strip('\n')[-4:] == "miss" :
						hit_dict.update({tables.get(match.group(0)):"miss"})
					else: 
						hit_dict.update({tables.get(match.group(0)):"hit"})
					

hit_miss_list = re.compile('|'.join(re.escape(key) for key in hit_dict))

def get_table_activity(line):
	activity = "default"
	for match in hit_miss_list.finditer(line):
		activity = hit_dict.get(match.group(0))
	return activity




######## change svg #########################
# write outputs
packet_num = 0
output = "flow_1q_packet_"+str(packet_num)+".svg"
template = "visio/flow_1q.svg"

color_dict = {"hit":"class=\"st301\"","miss":"class=\"st302\"","default":"class=\"st300\""}
replace = False
activity = "default"
with open(template,'r') as t, open (output,'w') as o:
	t_lines = t.readlines()
	for line in t_lines:
		if (re.search("<desc>.*?</desc>",line) > 0) and (not "Metadata" in line):		
#			if True: # TODO decide which color by table name 
			replace = True
			activity = get_table_activity(line)
		elif re.search("class=\"st.*?\"",line) > 0 and replace:		
			line = re.sub("class=\"st.*?\"",color_dict.get(activity,"class=\"st300\""),line)
			replace = False
			activity = "default"
		o.write(line)











