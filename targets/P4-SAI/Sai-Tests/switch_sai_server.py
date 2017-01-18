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

def CreateNewItem(obj_list, obj_class, forbidden_list=[]):
    new_id = GetNewIndex(obj_list.keys() + forbidden_list)
    new_obj = obj_class(id=new_id)
    obj_list[new_id] = new_obj
    # obj_list.update({new_id: new_obj})
    return new_id, new_obj

class Sai_obj():
    def __init__(self, id):
        self.id = id


class Port_obj(Sai_obj):
    def __init__(self, id, hw_port=0, pvid=1, bind_mode=0):
        Sai_obj.__init__(self, id)
        self.hw_port = hw_port
        self.pvid = pvid
        self.bind_mode = bind_mode


class Lag_obj(Sai_obj):
    def __init__(self, id, lag_members=[]):
        Sai_obj.__init__(self, id)
        self.lag_members = lag_members


class LagMember_obj(Sai_obj):
    def __init__(self, id, port_id=0,lag_id=0):
        Sai_obj.__init__(self, id)
        self.port_id = port_id
        self.lag_id = lag_id


class VlanMember_obj(Sai_obj):
    def __init__(self, id, vid=1, bridge_port_id=0,vlan_oid=0,tagging_mode=0):
        Sai_obj.__init__(self, id)
        self.vlan_oid = vlan_oid 
        self.vid = vid
        self.bridge_port_id = bridge_port_id
        self.tagging_mode = tagging_mode


class Vlan_obj(Sai_obj):
    def __init__(self, id, vid=1, vlan_members=None):
        Sai_obj.__init__(self, id)
        self.vid = vid
        self.vlan_members = vlan_members


class BridgePort_obj(Sai_obj):
    def __init__(self, id, port_id=0, vlan_id=1, br_port_type=0):
        Sai_obj.__init__(self, id)
        self.port_id = port_id
        self.vlan_id = vlan_id
        self.br_port_type = br_port_type


class Bridge_obj(Sai_obj):
    def __init__(self, id, bridge_type=0):
        Sai_obj.__init__(self, id)
        self.bridge_type = bridge_type


class SaiHandler():
  def __init__(self):
    self.switch_id = 0
    self.log = {}
    print "connecting to cli thrift"
    self.cli_client = SwitchThriftClient(json='../sai.json')
    self.hw_port_list = [0, 1, 2, 3, 4, 5, 6, 7]
    self.ports = {}
    self.vlans = {}
    self.vlan_members = {}
    self.bridge_ports = {}
    self.bridge_ids = {}
    self.lag_members = {}
    self.lags = {}

  def sai_thrift_create_switch(self, thrift_attr_list):
    return self.switch_id

  def sai_thrift_get_switch_attribute(self, thrift_attr_list):
    hw_port_list = sai_thrift_u32_list_t(u32list=self.hw_port_list, count=len(self.hw_port_list))
    for attr in thrift_attr_list:
      if attr.id == sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST:
        attr.value = sai_thrift_attribute_value_t(u32list=hw_port_list)
        new_attr = sai_thrift_attribute_t(id=attr.id, value=attr.value)
    new_attr_list = sai_thrift_attribute_list_t(attr_list = [new_attr], attr_count=1)
    return new_attr_list


  # FDB API
  def sai_thrift_create_fdb_entry(self, thrift_fdb_entry, thrift_attr_list):
    # fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, vlan_id=vlan_id)
    for attr in thrift_attr_list:
      if attr.id == sai_fdb_entry_attr.SAI_FDB_ENTRY_ATTR_TYPE:
        entry_type = attr.value.s32
      elif attr.id == sai_fdb_entry_attr.SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID:
        bridge_port = attr.value.oid
      elif attr.id == sai_fdb_entry_attr.SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
        packet_action = attr.value.s32

    bridge_type = thrift_fdb_entry.bridge_type
    bridge_id = thrift_fdb_entry.bridge_id
    mac = thrift_fdb_entry.mac_address
    vlan_id = thrift_fdb_entry.vlan_id
    out_if_type = 0 # port_type (not lag or router). TODO: check how to do it with SAI

    match_str = thrift_fdb_entry.mac_address + ' ' + str(bridge_id)
    action_str = str(bridge_port)
    if packet_action == sai_packet_action.SAI_PACKET_ACTION_FORWARD:
      if entry_type == sai_fdb_entry_type.SAI_FDB_ENTRY_TYPE_STATIC:
        self.cli_client.AddTable('table_fdb', 'action_set_egress_br_port', match_str, action_str)
  	return 0

  def sai_thrift_delete_fdb_entry(self, thrift_fdb_entry):
    match_str = thrift_fdb_entry.mac_address + ' ' + str(thrift_fdb_entry.bridge_id)
    self.cli_client.RemoveTableEntry('table_fdb', match_str)
    return 0

  # VLAN API
  def sai_thrift_create_vlan(self, thrift_attr_list):
    for attr in thrift_attr_list:
      if attr.id == sai_vlan_attr.SAI_VLAN_ATTR_VLAN_ID:
        vid = attr.value.u16
    if vid in [x.vid for x in self.vlans]:
      print "vlan id %d already exists" % vid
      return -1
    else:
      print "vlan id %d created" % vid
      vlan_oid, vlan_obj = CreateNewItem(self.vlans, Vlan_obj)
      vlan_obj.vid = vid
      vlan_obj.vlan_members = []
      return vlan_oid

  def sai_thrift_delete_vlan(self, vlan_oid):
    self.vlans.pop(vlan_oid, None)
    return 0

  def sai_thrift_remove_vlan_member(self, vlan_member_id):
    vlan_member = self.vlan_members[vlan_member_id]
    bridge_port_id = vlan_member.bridge_port_id
    vid = vlan_member.vid
    self.cli_client.RemoveTableEntry('table_egress_vlan_filtering', list_to_str([bridge_port_id, vid]))
    self.cli_client.RemoveTableEntry('table_ingress_vlan_filtering', list_to_str([bridge_port_id, vid]))
    out_if = self.ports[self.bridge_ports[bridge_port_id].port_id].hw_port
    if vlan_member.tagging_mode == sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_UNTAGGED:
      self.cli_client.RemoveTableEntry('table_egress_vlan_tag', list_to_str([out_if, vid, 1]))
    else:
      self.cli_client.RemoveTableEntry('table_egress_vlan_tag', list_to_str([out_if, vid, 0]))
    self.vlans[vlan_member.vlan_oid].vlan_members.remove(vlan_member_id)
    self.vlan_members.pop(vlan_member_id, None)
    return 0

  def sai_thrift_create_vlan_member(self, vlan_member_attr_list):
  	# sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED
  	# sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_UNTAGGED
    for attr in vlan_member_attr_list:
      if attr.id == sai_vlan_member_attr.SAI_VLAN_MEMBER_ATTR_VLAN_ID:
        vlan_oid = attr.value.oid
      elif attr.id == sai_vlan_member_attr.SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID:
        bridge_port_id = attr.value.oid
      elif attr.id == sai_vlan_member_attr.SAI_VLAN_MEMBER_ATTR_VLAN_TAGGING_MODE:
        tagging_mode = attr.value.s32
    vlan_obj = self.vlans[vlan_oid]
    vlan_id = vlan_obj.vid
    vlan_member_id, vlan_member_obj = CreateNewItem(self.vlan_members, VlanMember_obj)
    vlan_member_obj.bridge_port_id = bridge_port_id
    vlan_member_obj.vid = vlan_id
    vlan_member_obj.vlan_oid = vlan_oid
    vlan_member_obj.tagging_mode = tagging_mode
    vlan_obj.vlan_members.append(vlan_member_id)
    port_id = self.bridge_ports[bridge_port_id].port_id
    if port_id in self.lags.keys():
      out_if = port_id
    else:
      out_if = self.ports[port_id].hw_port
    if tagging_mode == sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED:
      vlan_pcp = 0 
      vlan_cfi = 0
      self.cli_client.AddTable('table_egress_vlan_tag','action_forward_vlan_tag',
                               list_to_str([out_if, vlan_id, 0]), list_to_str([vlan_pcp, vlan_cfi, vlan_id]))
    elif tagging_mode == sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_PRIORITY_TAGGED:
      vlan_pcp = 0 
      vlan_cfi = 0
      self.cli_client.AddTable('table_egress_vlan_tag','action_forward_vlan_tag',
                               list_to_str([out_if, vlan_id, 0]), list_to_str([vlan_pcp, vlan_cfi, vlan_id]))
    else:
      self.cli_client.AddTable('table_egress_vlan_tag','action_forward_vlan_untag',
                              list_to_str([out_if, vlan_id, 1]),'')

    self.cli_client.AddTable('table_egress_vlan_filtering','_nop',
                              list_to_str([bridge_port_id, vlan_id]),'')
    self.cli_client.AddTable('table_ingress_vlan_filtering','_nop',
                              list_to_str([bridge_port_id, vlan_id]),'')

    return vlan_member_id

  # Port API
  def sai_thrift_create_port(self, thrift_attr_list):
    port, port_obj = CreateNewItem(self.ports, Port_obj, forbidden_list=self.lags.keys())
    for attr in thrift_attr_list:
      if attr.id == sai_port_attr.SAI_PORT_ATTR_PORT_VLAN_ID:
        vlan_id = attr.value.u16
        port_obj.pvid = vlan_id
        self.cli_client.AddTable('table_port_PVID', 'action_set_pvid', str(port), str(vlan_id))
      elif attr.id == sai_port_attr.SAI_PORT_ATTR_BIND_MODE:
        bind_mode = attr.value.s32
        port_obj.bind_mode = bind_mode
        self.cli_client.AddTable('table_port_mode', 'action_set_port_mode', str(port), str(bind_mode))
      elif attr.id == sai_port_attr.SAI_PORT_ATTR_HW_LANE_LIST:
        hw_port_list = attr.value.u32list.u32list
    hw_port = hw_port_list[0]
    port_obj.hw_port = hw_port
    self.cli_client.AddTable('table_ingress_lag', 'action_set_lag_l2if', str(hw_port), list_to_str([0,port]))
    return port

  def sai_thrift_remove_port(self, port_id):
    hw_port = self.ports[port_id].hw_port
    self.cli_client.RemoveTableEntry('table_ingress_lag', str(hw_port))
    self.cli_client.RemoveTableEntry('table_port_PVID', str(port_id))
    self.ports.pop(port_id, None)
    return 0

  def sai_thrift_set_port_attribute(self, port, attr):
    if attr.id == sai_port_attr.SAI_PORT_ATTR_PORT_VLAN_ID:
      vlan_id = attr.value.u16
    #self.cli_client.AddTable('table_ingress_lag', 'action_set_lag_l2if', str(port), list_to_str([0, 0,port]))  # TODO - this needs to be somehwere else
    self.cli_client.AddTable('table_accepted_frame_type_default_internal', 'action_set_pvid', str(port), str(vlan_id))
    return 0

  # LAG Api
  def sai_thrift_create_lag(self, thrift_attr_list):
    lag_id, lag_obj = CreateNewItem(self.lags, Lag_obj, forbidden_list=self.ports.keys())
    return lag_id

  def sai_thrift_remove_lag(self, lag_id):
    self.lags.pop(lag_id, None)
    return 0

  def sai_thrift_create_lag_member(self, thrift_attr_list):
    lag_member_id, lag_member_obj = CreateNewItem(self.lag_members, LagMember_obj)
    for attr in thrift_attr_list:
      if attr.id == sai_lag_member_attr.SAI_LAG_MEMBER_ATTR_PORT_ID:
        port_id = attr.value.oid
        lag_member_obj.port_id = port_id
      if attr.id == sai_lag_member_attr.SAI_LAG_MEMBER_ATTR_LAG_ID:
        lag_id = attr.value.oid
        lag_member_obj.lag_id = lag_id
        self.lags[lag_id].lag_members.append(lag_member_id)
    port = self.ports[port_id]
    hw_port = port.hw_port
    pvid = port.pvid
    bind_mode = port.bind_mode 
    self.cli_client.RemoveTableEntry('table_ingress_lag', str(hw_port))
    self.cli_client.AddTable('table_ingress_lag', 'action_set_lag_l2if', str(hw_port), list_to_str([1, lag_id]))
    self.cli_client.RemoveTableEntry('table_lag_hash',str(lag_id))
    self.cli_client.AddTable('table_lag_hash', 'action_set_lag_hash_size', str(lag_id), str(len(self.lags[lag_id].lag_members)))
    self.cli_client.AddTable('table_egress_lag', 'action_set_out_port', list_to_str([lag_id, lag_member_id]), str(hw_port))
    self.cli_client.RemoveTableEntry('table_port_PVID', str(lag_id))
    self.cli_client.RemoveTableEntry('table_port_mode', str(lag_id))
    self.cli_client.AddTable('table_port_PVID', 'action_set_pvid', str(lag_id), str(pvid))
    self.cli_client.AddTable('table_port_mode', 'action_set_port_mode', str(lag_id), str(bind_mode))
    return lag_member_id

  def sai_thrift_remove_lag_member(self, lag_member_id):
    print "removing lag member %d" % lag_member_id
    print "members"
    print self.lag_members
    print "lags"
    print self.lags
    lag_member = self.lag_members.pop(lag_member_id, None)
    if not lag_member:
      return 0
    print "lag_id %d" % lag_member.lag_id
    print self.lags[lag_member.lag_id].lag_members
    self.lags[lag_member.lag_id].lag_members.remove(lag_member_id)
    return 0


  # Bridge API
  def sai_thrift_create_bridge(self, thrift_attr_list):
    bridge_id, bridge_obj = CreateNewItem(self.bridge_ids, Bridge_obj)
    for attr in thrift_attr_list:
      if attr.id == sai_bridge_attr.SAI_BRIDGE_ATTR_TYPE:
        bridge_type = attr.value.s32
        bridge_obj.bridge_type = bridge_type
    return bridge_id

  def sai_thrift_remove_bridge(self, bridge_id):
    self.bridge_ids.pop(bridge_id, None)
    return 0

  def sai_thrift_create_bridge_port(self, thrift_attr_list):
    for attr in thrift_attr_list:
      if attr.id == sai_bridge_port_attr.SAI_BRIDGE_PORT_ATTR_VLAN_ID:
        vlan_id = attr.value.s32
      elif attr.id == sai_bridge_port_attr.SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
        bridge_id = attr.value.s32
      elif attr.id == sai_bridge_port_attr.SAI_BRIDGE_PORT_ATTR_TYPE:
        bridge_port_type = attr.value.s32
      elif attr.id == sai_bridge_port_attr.SAI_BRIDGE_PORT_ATTR_PORT_ID:
        port_id = attr.value.s32
    br_port, br_port_obj = CreateNewItem(self.bridge_ports, BridgePort_obj)
    br_port_obj.port_id = port_id
    br_port_obj.vlan_id = vlan_id
    br_port_obj.type = bridge_port_type
    #TODO: Connect Thrift constants to P4 ?
    if bridge_port_type == sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_SUB_PORT: #.1D
      self.cli_client.RemoveTableEntry('table_bridge_id_1d', str(br_port))
      self.cli_client.RemoveTableEntry('table_egress_set_vlan', str(br_port))
      self.cli_client.AddTable('table_bridge_id_1d', 'action_set_bridge_id', str(br_port), str(bridge_id))
      self.cli_client.AddTable('table_egress_set_vlan', 'action_set_vlan', str(br_port), str(vlan_id))
      l2_if_type = 2
    elif bridge_port_type == sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_PORT: #.1Q
      self.cli_client.RemoveTableEntry('table_bridge_id_1q', str(br_port))
      self.cli_client.AddTable('table_bridge_id_1q', 'action_set_bridge_id', str(vlan_id), str(bridge_id))
      l2_if_type = 3 

    self.cli_client.RemoveTableEntry('table_egress_br_port_to_if', str(br_port))  
    if port_id in self.lags.keys(): # LAG
      self.cli_client.AddTable('table_egress_br_port_to_if', 'action_forward_set_outIfType', str(br_port), list_to_str([port_id, 1]))
      bind_mode = self.ports[self.lag_members[self.lags[port_id].lag_members[0]].port_id].bind_mode
    else: # port
      self.cli_client.AddTable('table_egress_br_port_to_if', 'action_forward_set_outIfType', str(br_port), list_to_str([self.ports[port_id].hw_port, 0]))
      bind_mode = self.ports[port_id].bind_mode

    if bind_mode == sai_port_bind_mode.SAI_PORT_BIND_MODE_SUB_PORT:
      self.cli_client.AddTable('table_subport_ingress_interface_type', 'action_set_l2_if_type', list_to_str([port_id, vlan_id]), list_to_str([l2_if_type, br_port]))
    else:
      self.cli_client.AddTable('table_port_ingress_interface_type', 'action_set_l2_if_type', str(port_id), str(br_port))
    return br_port

  def sai_thrift_remove_bridge_port(self, bridge_port_id):
    br_port = self.bridge_ports.pop(bridge_port_id, None)
    if br_port.type == sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_SUB_PORT: #.1D 
      self.cli_client.RemoveTableEntry('table_bridge_id_1d', str(br_port.id))
      self.cli_client.RemoveTableEntry('table_egress_set_vlan', str(br_port.id))
    else:
      self.cli_client.RemoveTableEntry('table_bridge_id_1d', str(br_port.vlan_id))
    self.cli_client.RemoveTableEntry('table_ingress_l2_interface_type', list_to_str([br_port.port_id, br_port.vlan_id]))
    self.cli_client.RemoveTableEntry('table_egress_br_port_to_if', str(br_port.id))
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