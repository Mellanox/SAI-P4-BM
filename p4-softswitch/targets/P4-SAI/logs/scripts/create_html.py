
from collections import OrderedDict
ptf_tests = OrderedDict([
			("L2AccessToAccessVlanTest",1),
			("L2TrunkToTrunkVlanTest",1),
			("L2AccessToTrunkVlanTest",1),
			("L2TrunkToAccessVlanTest",1),
			("L2FloodTest",3),
			("L2LagTest",209),
			("L2VlanBcastUcastTest",8)])

total = 0
with open('ptf.html','w') as h:
	h.write('<!DOCTYPE html>\n<html>\n<head>\n<style>\ntable {\nfont-family: arial, sans-serif;\nborder-collapse: collapse;\nwidth: 100%;\n}')
	h.write('td, th {\n    border: 1px solid #dddddd;\n    text-align: left;\n    padding: 8px;\n}')
	h.write('tr:nth-child(even) {\n    background-color: #dddddd;\n}\n</style>\n</head>\n<body>\n<table>\n  <tr>\n    <th>PTF</th>\n    <th>1Q_flow link</th>\n    <th>Status</th>\n  </tr>\n  <tr>')
	for key in ptf_tests:
		h.write('<tr>\n<td>'+key+'</td>\n<td>')
		for i in xrange(ptf_tests[key]):
			h.write('<a href="flow_1q_packet_'+str(total+i)+'.svg"> test packet #'+str(i)+' </a> <br>')
		total = total + ptf_tests[key]
		h.write('</td><td></td>\n</tr>')

	h.write('</table>\n</body>\n</html>')