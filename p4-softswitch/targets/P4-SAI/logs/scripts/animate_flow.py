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
			'table_vbridge_STP':'vBrid',
			'table_bridge_id_1q':'Bridge_id',
			'table_ingress_vlan_filtering':'Ingress vlan filtering',
			'table_mc_lookup_mode':'',
			'table_xSTP_instance':'xSTP instance',
			'table_xSTP':'Ingress xSTP',
			'table_learn_fdb':'Learning FDB',
			'table_l3_interface':'L3 interface',
			'table_fdb':'FDB',
			'table_mc_fdb':'MC FDB',
			'table_mc_l2_sg_g':'',
			'table_unknown_multicast_nonip':'Unknown multicast',
			'table_unknown_multicast_ipv4':'Unknown multicast',
			'table_broadcast':'',
			'table_flood':'',
			'table_egress_vbridge_STP':'Egress vBridge',
			'table_egress_vlan_tag':'Egress TAG',
			'table_egress_xSTP':'Egress xSTP',
			'table_egress_vlan_filtering':'egress vlan filtering',
			'table_egress_br_port_to_if':'Egress bridge port to interface type table',
			'table_egress_set_vlan':'',
			'table_lag_hash':'',
			'table_egress_lag':'Egress LAG',
			'table_cpu_forward':'',
			'table_ingress_l3_if':'Ingress L3 interface',
			'table_ingress_vrf':'Ingress VRF',
			'table_router':'Router table',
			'table_next_hop_group':'Next hop group table',
			'table_next_hop':'Next hop table',
			'table_neighbor':'Neigh table',
			'table_egress_L3_if':'Egress_L3_interface',
		}




#### GETTING hit/miss per svg table ###################
table_names_list = re.compile('|'.join(re.escape(key) for key in tables))

def new_packet_in_log(log_line):
	return ((log_line[:8]=="[Bridge]") and ("Processing packet received" in log_line))

def parse_filtered_log(packet_num):
	log_path = "filtered_log.txt"
	hit_dict = {}
	log_end = True
	with open(log_path,'r') as log:
		log_lines = log.readlines()
		for line in log_lines:
		# go to desired packet
			if (packet_num != -1):
				if new_packet_in_log(line):
					packet_num-=1
			elif "Table" in line:
				for match in table_names_list.finditer(line):
					if tables.get(match.group(0)) != "":
						if line.strip('\n')[-4:] == "miss" :
							hit_dict.update({tables.get(match.group(0)):"miss"})
						else: 
							hit_dict.update({tables.get(match.group(0)):"hit"})
			elif new_packet_in_log(line):
				log_end = False
				break
	hit_miss_list = re.compile('|'.join(re.escape(key) for key in hit_dict))
	return hit_miss_list,hit_dict,log_end

def get_table_activity(line,hit_miss_list,hit_dict):
	activity = "default"
	for match in hit_miss_list.finditer(line):
		activity = hit_dict.get(match.group(0))
	return activity




######## change svg #########################
# write outputs
def create_svg(template,output,hit_miss_list,hit_dict):
	color_dict = {"hit":"class=\"st301\"","miss":"class=\"st302\"","default":"class=\"st300\""}
	replace = False
	activity = "default"
	with open(template,'r') as t, open (output,'w') as o:
		t_lines = t.readlines()
		for line in t_lines:
			if (re.search("<desc>.*?</desc>",line) > 0) and (not "Metadata" in line):		
	#			if True: # TODO decide which color by table name 
				replace = True
				activity = get_table_activity(line,hit_miss_list,hit_dict)
			elif re.search("class=\"st.*?\"",line) > 0 and replace:		
				line = re.sub("class=\"st.*?\"",color_dict.get(activity,"class=\"st300\""),line)
				replace = False
				activity = "default"
			o.write(line)


def main(): # TODO handle router packets (packet num stays the same for router->bridge)
	log_end = False
	packet_num = 0
	#template = ["visio/flow_1q.svg","visio/flow_router_uni.svg","visio/flow_1d.svg"]
	#template = ["visio/flow_1q.svg","visio/flow_router_uni.svg"]
	template = ["visio/flow_1q.svg"]
	while not log_end:
		# print packet_num
		hit_miss_list, hit_dict, log_end = parse_filtered_log(packet_num)
		output = [i[6:-4]+"_packet_"+str(packet_num)+i[-4:] for i in template]
		for i in xrange(len(template)):
			create_svg(template[i],output[i],hit_miss_list,hit_dict)
		packet_num+=1


if __name__ == "__main__":
	main()











