import sys
sys.path.append('sai_thrift_src/gen-py')
sys.path.append('../')
sys.path.append('../../../tools/')
from cli_driver import SwitchThriftClient
from switch_sai import switch_sai_rpc
from switch_sai.ttypes import *

from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer

import socket

def list_to_str(num_list):
  st = ''
  for num in num_list:
    st = st + str(num) + ' '
  return st


def GetNewIndex(num_list):
  return min(set(xrange(len(num_list)+1)) - set(num_list))

class SaiHandler:
  def __init__(self):
    self.log = {}
    print "connecting to cli thrift"
    self.cli_client = SwitchThriftClient(json='../sai.json')
    self.active_vlans = {}
    self.bridge_ports = []
    self.bridge_id = 0

  def sai_thrift_create_vlan(self, vid):
    if vid in self.active_vlans:
      print "vlan id %d already exists" % vid
    else:
      print "vlan id %d created" % vid
      self.active_vlans.update({vid: []})
    return 0

  def sai_thrift_create_fdb_entry(self, thrift_fdb_entry, thrift_attr_list):
    # fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, vlan_id=vlan_id)
    for attr in thrift_attr_list:
      if attr.id == sai_fdb_entry_attr.SAI_FDB_ENTRY_ATTR_TYPE:
        entry_type = attr.value.s32
      elif attr.id == sai_fdb_entry_attr.SAI_FDB_ENTRY_ATTR_PORT_ID:
        port = attr.value.oid
      elif attr.id == sai_fdb_entry_attr.SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
        packet_action = attr.value.s32
    out_if = port
    out_if_type = 0 # port_type (not lag or router). TODO: check how to do it with SAI
    match_str = thrift_fdb_entry.mac_address + ' ' + str(self.bridge_id)
    action_str = list_to_str([out_if, out_if_type])
    if packet_action == sai_packet_action.SAI_PACKET_ACTION_FORWARD:
      if entry_type == sai_fdb_entry_type.SAI_FDB_ENTRY_TYPE_STATIC:
        self.cli_client.AddTable('table_fdb', 'action_forward_set_outIfType', match_str, action_str)
  	return 0

  def sai_thrift_delete_fdb_entry(self, thrift_fdb_entry):
    match_str = thrift_fdb_entry.mac_address + ' ' + str(self.bridge_id)
    print "RemoveTableEntry: %s" % match_str
    self.cli_client.RemoveTableEntry('table_fdb', match_str)
    return 0

  def sai_thrift_delete_vlan(self, vlan_id):
    self.active_vlans.pop(vlan_id, None)
    return 0

  def sai_thrift_remove_vlan_member(self, vlan_member_id):
    for vlan_id, vlan_members in self.active_vlans.iteritems():
      if vlan_member_id in vlan_members:
        self.active_vlans[vlan_id].remove(vlan_member_id)
    # TODO, update tables as well?
    return 0

  def sai_thrift_create_vlan_member(self, vlan_member_attr_list):
  	# sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED
  	# sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_UNTAGGED
    for attr in vlan_member_attr_list:
      if attr.id == sai_vlan_member_attr.SAI_VLAN_MEMBER_ATTR_VLAN_ID:
        vlan_id = attr.value.s32
      elif attr.id == sai_vlan_member_attr.SAI_VLAN_MEMBER_ATTR_PORT_ID:
        port_id = attr.value.oid
      elif attr.id == sai_vlan_member_attr.SAI_VLAN_MEMBER_ATTR_VLAN_TAGGING_MODE:
        tagging_mode = attr.value.s32
    all_vlan_members = [item for sublist in self.active_vlans.values() for item in sublist]
    vlan_member_id = GetNewIndex(all_vlan_members)

    self.active_vlans[vlan_id].append(vlan_member_id)
    self.cli_client.AddTable('table_ingress_vlan_filtering','_nop',list_to_str([port_id, vlan_id]),'')

    if tagging_mode == sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED:
      vlan_pcp = 0 
      vlan_cfi = 0
      self.cli_client.AddTable('table_egress_vlan_filtering','action_set_vlan_tag_mode',
                               list_to_str([port_id, vlan_id, 0]), list_to_str([vlan_pcp, vlan_cfi]))
    else:
      self.cli_client.AddTable('table_egress_vlan_filtering','_nop',
                              list_to_str([port_id, vlan_id, 0]),'')

    self.cli_client.AddTable('table_egress_vlan_filtering','_nop',
                              list_to_str([port_id, vlan_id, 1]),'')


    # TODO - this needs to be somewhere else.
    l2_if_type = 3   # 1Q Bridge, TODO: not part of SAI api??
    br_port = GetNewIndex(self.bridge_ports)
    self.bridge_ports.append(br_port)
    self.cli_client.AddTable('table_ingress_l2_interface_type', 'action_set_l2_if_type',
                             list_to_str([port_id, vlan_id]), list_to_str([l2_if_type, br_port]))
    self.cli_client.AddTable('table_vbridge', 'action_set_bridge_id', str(br_port), str(self.bridge_id))

    

    return vlan_member_id

  def sai_thrift_set_port_attribute(self, port, attr):
    if attr.id == sai_port_attr.SAI_PORT_ATTR_PORT_VLAN_ID:
      vlan_id = attr.value.u16
    #self.cli_client.AddTable('table_ingress_lag', 'action_set_lag_l2if', str(port), list_to_str([0, 0,port]))  # TODO - this needs to be somehwere else
    self.cli_client.AddTable('table_accepted_frame_type', 'action_set_pvid', str(port), str(vlan_id))
    return 0


handler = SaiHandler()
processor = switch_sai_rpc.Processor(handler)
transport = TSocket.TServerSocket(port=9092)
tfactory = TTransport.TBufferedTransportFactory()
pfactory = TBinaryProtocol.TBinaryProtocolFactory()

server = TServer.TSimpleServer(processor, transport, tfactory, pfactory)

print "Starting python server..."
server.serve()
print "done!"