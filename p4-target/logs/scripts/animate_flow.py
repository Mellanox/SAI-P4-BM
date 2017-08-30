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
			'table_vbridge_STP':'Ingress vBridge STP Table',
			'table_bridge_id_1q':'Bridge_id',
			'table_ingress_vlan_filtering':'Ingress vlan filtering',
			'table_mc_lookup_mode':'',
			'table_xSTP_instance':'xSTP instance Table',
			'table_xSTP':'Ingress xSTP Table',
			'table_learn_fdb':'Learning FDB',
			'table_l3_interface':'L3 interface Table',
			'table_fdb':'FDB Table',
			'table_mc_fdb':'MC FDB',
			'table_mc_l2_sg_g':'',
			'table_unknown_multicast_nonip':'Unknown multicast',
			'table_unknown_multicast_ipv4':'Unknown multicast',
			'table_broadcast':'',
			'table_flood':'',
			'table_egress_vbridge_STP':'Egress vBridge STP Table',
			'table_egress_vlan_tag':'Egress TAG',
			'table_egress_xSTP':'Egress xSTP',
			'table_egress_vlan_filtering':'egress vlan filtering Table',
			'table_egress_br_port_to_if':'Egress bridge port to interface type table',
			'table_egress_set_vlan':'',
			'table_lag_hash':'',
			'table_egress_lag':'Egress LAG Table',
			'table_cpu_forward':'',
			'table_ingress_l3_if':'Ingress L3 interface table',
			'table_ingress_vrf':'Ingress VRF table',
			'table_router':'Router table',
			'table_next_hop_group':'Next hop group table',
			'table_next_hop':'Next hop table',
			'table_neighbor':'Neigh table',
			'table_egress_L3_if':'Egress_L3_interface table',
		}

#### GETTING hit/miss per svg table ###################
table_names_list = re.compile('|'.join(re.escape(key) for key in tables))

def new_packet_in_log(log_line):
	return ((log_line[:8]=="[Bridge]") and ("Processing packet received" in log_line))

def parse_filtered_log(packet_num):
	log_path = "filtered_log.txt"
	hit_dict = {}
	log_end = True
	get_action = False
	with open(log_path,'r') as log:
		log_lines = log.readlines()
		for line in log_lines:
		# go to desired packet
			if (packet_num != -1):
				if new_packet_in_log(line):
					packet_num-=1
			elif "Table" in line:
				table_status = ""
				for match in table_names_list.finditer(line):
					if len(match.group(0))>0:
						match_group = match.group(0)
						get_action = True
						if tables.get(match_group) != "":
							table_status = "Miss" if (line.strip('\n')[-4:] == "miss") else "Hit"
								 
			# after table always comes action
			elif get_action:
				get_action = False
				key = tables.get(match_group)
				if len(key)>0:
					action = line.split("Action entry is ")[-1].rstrip()
					hit_dict.update({key:[table_status,action]})
			elif new_packet_in_log(line):
				log_end = False
				break
	hit_miss_list = re.compile('|'.join(re.escape(key) for key in hit_dict))
	return hit_miss_list,hit_dict,log_end

def get_table_info(line,hit_miss_list,hit_dict):
	activity = "default"
	table_hash = ""
	action = ""
	#print hit_dict.keys()
	#hit_miss_list = re.compile('|'.join(re.escape(key) for key in hit_dict))
	for match in hit_miss_list.finditer(line):
		if len(match.group(0))>0:
			#print match.group(0)
			table_hash = match.group(0)
			value = hit_dict.get(table_hash,["",""])
			activity = value[0]
			action = value[1]
	return activity, action ,table_hash





################ helper functions for nice text lyout ###########################

def new_line(end=False,x=0,style='st500'):
	if (end): return'</tspan>'
	else:	return '<tspan x="'+str(x) +'" dy="1.2em" class = "' + style+ '">'


def fix_lines(text,line_len = 12,style ='st500',x=0):
	words = text.split(' ')
	if len(words)>1 and len(text)>line_len:
		text_out = words[0]
		n=len(text_out) ; i=1 ;  close_last_line = "" ; space =True
		while i<len(words):
			if n+len(words[i]) >line_len:
				text_out += close_last_line + new_line(x=x,style=style)	
				close_last_line = new_line(end=True)	
				n=0; space= False
			text_out += (' ' if space else '')+ words[i]
			n+=len(words[i]) ; space = True ; i+=1
		return text_out +close_last_line
	else: return text

def replace_table_text(text,activity,action,table_hash,y):	
	pre_text = text.split('<text')[0]
	post_text=text.split('</text>')[-1]
	# if "NULL" in action: print action - TODO 
	action = action.split('-')
	if "action_" in action[0]: action[0] =  action[0][7:]

	main_text = fix_lines(table_hash,style ='st501') + new_line(x=12) + "Table " + activity + new_line(end=True)
	main_text += new_line() + "Action :" + new_line(end=True) + new_line(x=12) + action[0] + new_line(end=True) 
	if len(action[-1])>0 and len(action)>1:
		main_text += new_line() + "Params :" + new_line(end=True) + new_line(x=12) + action[1] + new_line(end=True)
	
	final_text = pre_text+ "<text x=\"0\" y=\""+y+"\" class=\"st501\" v:langID=\"1033\">"
	final_text +=  main_text + '</text>' + post_text
	return final_text




################### change svg #########################
# write outputs
def create_svg(template,output,hit_miss_list,hit_dict,y):
	color_dict = {"Hit":"class=\"st301\"","Miss":"class=\"st302\"","default":"class=\"st300\""}
	replace_style = False
	replace_text = False
	in_text = False
	text = ""
	table_hash = ""
	activity = "default"
	action = ""
	with open(template,'r') as t, open (output,'w') as o:
		t_lines = t.readlines()
		for line in t_lines:

			# detect table
			if (re.search("<desc>.*?</desc>",line) > 0) and (not "Metadata" in line):		
				replace_style = True
				activity,action,table_hash = get_table_info(line,hit_miss_list,hit_dict)
				replace_text = activity != 'default'

			# replace table color
			elif re.search("class=\"st.*?\"",line) > 0 and replace_style:		
				line = re.sub("class=\"st.*?\"",color_dict.get(activity,"class=\"st300\""),line)
				replace_style = False

			# Assume no <text> and </text> in same line
			elif re.search("<text.*?>",line) > 0 and replace_text:		
				in_text = True
				text += '\n'+line
				#print "started text"
			#elif re.search("</text>",line) > 0 and replace_text and in_text:
			elif in_text:
				text += line
				#print "in text"
			
			if re.search("</text>",line) > 0 and in_text:
					replace_text = False
					in_text = False
					o.write(replace_table_text(text,activity,action,table_hash,y))
					text = ""
					activity = "default"
			elif not in_text:
				o.write(line)

def main(): # TODO handle router packets (packet num stays the same for router->bridge)
	log_end = False
	packet_num = 0
	#template = ["visio/flow_1q.svg","visio/flow_router_uni.svg","visio/flow_1d.svg"]
	template = ["visio/flow_1q.svg","visio/flow_router_uni.svg"]
	y_map = ['1045.37','1616.37']
	#template = ["visio/flow_1q.svg"]
	while not log_end:
		# print packet_num
		hit_miss_list, hit_dict, log_end = parse_filtered_log(packet_num)
		output = [i[6:-4]+"_packet_"+str(packet_num)+i[-4:] for i in template]
		for i in xrange(len(template)):
			y = y_map[i]
			create_svg(template[i],output[i],hit_miss_list,hit_dict,y)
		packet_num+=1


if __name__ == "__main__":
	main()











