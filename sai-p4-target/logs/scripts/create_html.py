
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
	# h.write('<!DOCTYPE html>\n<html>\n<head>')
	# h.write('<meta name="viewport" content="width=device-width, initial-scale=1">')
	# h.write('<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">')#
	h.write('\n<style>\ntable {\nfont-family: arial, sans-serif;\nborder-collapse: collapse;\nwidth: 100%;\n}')
	# h.write('<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>')
	# h.write('<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>')
	h.write('td, th {\n    border: 1px solid #dddddd;\n    text-align: left;\n    padding: 8px;\n}')
	h.write('tr:nth-child(even) {\n    background-color: #dddddd;\n}\n</style>\n</head>\n<body>\n')
# 	h.write('<div class="container">\n<h2>Dropdowns</h2>\n<p>The .dropdown-header class is used to add headers inside the dropdown menu:</p>')
# 	h.write('<div class="dropdown"><button class="btn btn-default dropdown-toggle" type="button" data-toggle="dropdown">Tutorials')
# 	h.write('<span class="caret"></span></button><ul class="dropdown-menu"><li class="dropdown-header">Dropdown header 1</li>')
# 	h.write('')
# 	h.write('')


  
  
    
#       <li><a href="#">HTML</a></li>
#       <li><a href="#">CSS</a></li>
#       <li><a href="#">JavaScript</a></li>
#       <li class="divider"></li>
#       <li class="dropdown-header">Dropdown header 2</li>
#       <li><a href="#">About Us</a></li>
#     </ul>
#   </div>
# </div>
	h.write('<table>\n  <tr>\n    <th>PTF</th>\n    <th>1Q_flow link</th>\n    <th>Status</th>\n  </tr>\n  <tr>')
	for key in ptf_tests:
		h.write('<tr>\n<td>'+key+'</td>\n<td>')
		for i in xrange(ptf_tests[key]):
			h.write('<a href="flow_1q_packet_'+str(total+i)+'.svg"> test packet #'+str(i)+' </a> <br>')
		total = total + ptf_tests[key]
		h.write('</td><td></td>\n</tr>')

	h.write('</table>\n</body>\n</html>')

  
  
  

