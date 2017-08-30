#!/usr/bin/env python
# aggregate all P4 target logs to one time ordeed log.
from datetime import datetime
import datetime as dt
import os
import shutil
import time


def get_time(time,str):
	t = time
	if len(str)<14:
		return t
	try:
	 	time = datetime.strptime(str[1:13], '%H:%M:%S.%f')
	 	if time<t:
	 		time = time+dt.timedelta(days=1) # prevent midnight wraparound
	except ValueError:
	     return t
	return time

def meld_logs(log1,log2,log1_initial_list,log2_initial,out):
	with open(log1,'r') as lg1, open(log2,'r') as lg2, open (out,'w') as o:
	 	l1_lines = lg1.readlines()
	 	l2_lines = lg2.readlines()
		index = [0,0]
		time=[datetime.strptime('00:00:00.000', '%H:%M:%S.%f'),datetime.strptime('00:00:00.000', '%H:%M:%S.%f')]
		chk_time=[True,True]
		length = [len(l1_lines),len(l2_lines)]
		initial_list=[]
		while (index[0]<length[0]) or (index[1]<length[1]):
			if chk_time[0]: time[0] = get_time(time[0],l1_lines[index[0]])
			if chk_time[1]: time[1] = get_time(time[1],l2_lines[index[1]])
			if (time[0]<time[1]) and (index[0]<length[0]) or (index[1]==length[1]): # TODO
				o.write(l1_lines[index[0]])
		 		initial_list.append(log1_initial_list[index[0]])
		 		index[0]+=1
		 		chk_time=[index[0]<length[0],False]
			elif (index[1]<length[1]) or (index[0]==length[0]):
				o.write(l2_lines[index[1]])
				initial_list.append(log2_initial)
				index[1]+=1
				chk_time=[False,index[1]<length[1]]

	print('melded logs: ({},{}) => {}'.format(log1,log2,out))
	return initial_list

# add diffrent title for each log, e.g. [bridge] . [router] ...
def add_initials(log,initials,initials_index_vec,out):
	with open(log,'r') as lg, open (out,'w') as o:
		lines = lg.readlines()
		for i in xrange(len(lines)): 
			o.write('['+initials[initials_index_vec[i]]+'] '+lines[i])

# Main
def main():
	log_path_list =[]
	log_path_list.append('bridge_log.txt')
	log_path_list.append('router_log.txt')
	#log_path_list.append('c.txt')
	#log_path_list.append('d.txt')

	add_initials_bool = True

	log_initials =[]
	log_initials.append('Bridge')
	log_initials.append('Router')
	#log_initials.append('c')
	#log_initials.append('d')

	output_path = 'log.txt' # dir only

	#iniitialize:
	prev_temp_file = log_path_list[0]
	with open(log_path_list[0],'r') as l0:
			prev_initial = [0]*(len(l0.readlines()))

	#loop files:
	for i in xrange(1,len(log_path_list)):
		temp_file = output_path[:-4] + '_temp_' + str(i) + output_path[-4:]
		prev_initial=meld_logs(prev_temp_file,log_path_list[i],prev_initial,i,temp_file)
		if i>1 : 
			os.remove(prev_temp_file)
			print('deleted {}'.format(prev_temp_file))
		prev_temp_file = temp_file
	if add_initials_bool:
		add_initials(temp_file,log_initials,prev_initial,output_path)
		print('copying with initials from {} to {}'.format(temp_file,output_path))
		os.remove(temp_file)
	else:
		print('copying {} to {}'.format(temp_file,output_path))
		shutil.move(temp_file,output_path)

if __name__ == "__main__":
	main()