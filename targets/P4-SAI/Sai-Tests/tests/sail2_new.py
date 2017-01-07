# Copyright 2013-present Barefoot Networks, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Thrift SAI interface L2 tests
"""
import socket
from switch import *
import sai_base_test
import sys
sys.path.append('../sai_thrift_src/gen-py/')
from switch_sai.ttypes import *
import random
# from sai_enums import *
 
@group('l2')
class L21DBridgeBasicTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet port 0 -> port 1"
        vlan_id = 10
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        # Get HW ports
        attr_list = [sai_thrift_attribute_t(id=sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST)]
        attr_list = self.client.sai_thrift_get_switch_attribute(attr_list)
        for attr in attr_list.attr_list:
            if attr.id == sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST:
                hw_port_list = attr.value.u32list.u32list
            # sai_switch_attr.SAI_SWITCH_ATTR_PORT_NUMBER
        hw_port1 = hw_port_list[0]
        hw_port2 = hw_port_list[1]

        # Create Ports
        bind_mode = sai_port_bind_mode.SAI_PORT_BIND_MODE_SUB_PORT
        port1 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port1)
        port2 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port2)

        # Create 1D Bridge
        bridge_type = sai_bridge_type.SAI_BRIDGE_TYPE_1D
        bridge_attr_value = sai_thrift_attribute_value_t(s32= bridge_type)
        bridge_attr = sai_thrift_attribute_t(id=sai_bridge_attr.SAI_BRIDGE_ATTR_TYPE, value=bridge_attr_value)
        bridge = self.client.sai_thrift_create_bridge([bridge_attr])

        # Create Bridge ports
        bridge_port_type = sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_SUB_PORT
        bridge_port1 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port1, vlan_id, bridge)
        bridge_port2 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port2, vlan_id, bridge)

        # Create FDB Entries:
        mac_action = sai_packet_action.SAI_PACKET_ACTION_FORWARD
        fdb_entry_type = sai_fdb_entry_type.SAI_FDB_ENTRY_TYPE_STATIC
        sai_thrift_create_fdb(self.client, mac1, bridge_type, vlan_id, bridge, bridge_port1, mac_action, fdb_entry_type)
        sai_thrift_create_fdb(self.client, mac2, bridge_type, vlan_id, bridge, bridge_port2, mac_action, fdb_entry_type)

        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64)

        try:
            send_packet(self, hw_port1, str(pkt))
            verify_packets(self, pkt, [hw_port2])
        finally:
            sai_thrift_delete_fdb(self.client, mac1, bridge)
            sai_thrift_delete_fdb(self.client, mac2, bridge)
            self.client.sai_thrift_remove_port(port1)
            self.client.sai_thrift_remove_port(port2)

@group('l2')
class L21QBridgeAccess2AccessTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet port 0 -> port 1"
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        vlan_id = 10

        # Get HW ports
        attr_list = [sai_thrift_attribute_t(id=sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST)]
        attr_list = self.client.sai_thrift_get_switch_attribute(attr_list)
        for attr in attr_list.attr_list:
            if attr.id == sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST:
                hw_port_list = attr.value.u32list.u32list
            # sai_switch_attr.SAI_SWITCH_ATTR_PORT_NUMBER
        hw_port1 = hw_port_list[0]
        hw_port2 = hw_port_list[1]

        # Create Ports
        bind_mode = sai_port_bind_mode.SAI_PORT_BIND_MODE_SUB_PORT
        port1 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port1)
        port2 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port2)

        # Create 1Q Bridge
        bridge_type = sai_bridge_type.SAI_BRIDGE_TYPE_1Q
        bridge_attr_value = sai_thrift_attribute_value_t(s32= bridge_type)
        bridge_attr = sai_thrift_attribute_t(id=sai_bridge_attr.SAI_BRIDGE_ATTR_TYPE, value=bridge_attr_value)
        bridge = self.client.sai_thrift_create_bridge([bridge_attr])

        # Create Bridge ports
        bridge_port_type = sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_PORT
        bridge_port1 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port1, vlan_id, bridge)
        bridge_port2 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port2, vlan_id, bridge)

        # Create VLAN
        vlan_attr_value = sai_thrift_attribute_value_t(u16= vlan_id)
        vlan_attr = sai_thrift_attribute_t(id=sai_vlan_attr.SAI_VLAN_ATTR_VLAN_ID, value=vlan_attr_value)
        vlan_oid = self.client.sai_thrift_create_vlan([vlan_attr])

        # tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED
        tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_UNTAGGED
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port1, tagging_mode)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port2, tagging_mode)
        # SAI_VLAN_ATTR_MEMBER_LIST

        # Create FDB Entries:
        mac_action = sai_packet_action.SAI_PACKET_ACTION_FORWARD
        fdb_entry_type = sai_fdb_entry_type.SAI_FDB_ENTRY_TYPE_STATIC
        sai_thrift_create_fdb(self.client, mac1, bridge_type, vlan_id, bridge, bridge_port1, mac_action, fdb_entry_type)
        sai_thrift_create_fdb(self.client, mac2, bridge_type, vlan_id, bridge, bridge_port2, mac_action, fdb_entry_type)
        
        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64)

        try:
            send_packet(self, hw_port1, str(pkt))
            verify_packets(self, pkt, [hw_port2])
        finally:
            sai_thrift_delete_fdb(self.client, mac1, bridge)
            sai_thrift_delete_fdb(self.client, mac2, bridge)
            self.client.sai_thrift_remove_port(port1)
            self.client.sai_thrift_remove_port(port2)
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_delete_vlan(vlan_oid)

@group('l2')
class L21QBridgeAccess2TrunkTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        # switch_init(self.client)
        vlan_id = 11
        trunk_pvid = 20
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        # Get HW ports
        attr_list = [sai_thrift_attribute_t(id=sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST)]
        attr_list = self.client.sai_thrift_get_switch_attribute(attr_list)
        for attr in attr_list.attr_list:
            if attr.id == sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST:
                hw_port_list = attr.value.u32list.u32list
            # sai_switch_attr.SAI_SWITCH_ATTR_PORT_NUMBER
        # chosen_hw_ports = random.sample(hw_port_list, 2)
        # hw_port1 = chosen_hw_ports[0]
        # hw_port2 = chosen_hw_ports[1]
        hw_port1 = hw_port_list[0]
        hw_port2 = hw_port_list[1]
        print "Sending L2 packet Access(%d) -> Trunk(%d) (trunk vlan=%d)" % (hw_port1, hw_port2, vlan_id)

        # Create Ports
        bind_mode = sai_port_bind_mode.SAI_PORT_BIND_MODE_SUB_PORT
        port1 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port1)
        port2 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port2)

        # Create 1Q Bridge
        bridge_type = sai_bridge_type.SAI_BRIDGE_TYPE_1Q
        bridge_attr_value = sai_thrift_attribute_value_t(s32= bridge_type)
        bridge_attr = sai_thrift_attribute_t(id=sai_bridge_attr.SAI_BRIDGE_ATTR_TYPE, value=bridge_attr_value)
        bridge = self.client.sai_thrift_create_bridge([bridge_attr])

        # Create Bridge ports
        bridge_port_type = sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_PORT
        bridge_port1 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port1, vlan_id, bridge)
        bridge_port2 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port2, trunk_pvid, bridge)

        # Create VLAN
        vlan_attr_value = sai_thrift_attribute_value_t(u16= vlan_id)
        vlan_attr = sai_thrift_attribute_t(id=sai_vlan_attr.SAI_VLAN_ATTR_VLAN_ID, value=vlan_attr_value)
        vlan_oid = self.client.sai_thrift_create_vlan([vlan_attr])

        # Set Port attr
        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=sai_port_attr.SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        # tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED
        tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_UNTAGGED
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port1, tagging_mode)
        tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port2, tagging_mode)
        # SAI_VLAN_ATTR_MEMBER_LIST

        # Create FDB Entries:
        mac_action = sai_packet_action.SAI_PACKET_ACTION_FORWARD
        fdb_entry_type = sai_fdb_entry_type.SAI_FDB_ENTRY_TYPE_STATIC
        sai_thrift_create_fdb(self.client, mac1, bridge_type, vlan_id, bridge, bridge_port1, mac_action, fdb_entry_type)
        sai_thrift_create_fdb(self.client, mac2, bridge_type, vlan_id, bridge, bridge_port2, mac_action, fdb_entry_type)
        
        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                dl_vlan_enable=True,
                                vlan_vid=vlan_id,
                                ip_ttl=64,
                                pktlen=104)
        try:
            send_packet(self, hw_port1, str(pkt))
            verify_packets(self, exp_pkt, [hw_port2])
        finally:
            sai_thrift_delete_fdb(self.client, mac1, bridge)
            sai_thrift_delete_fdb(self.client, mac2, bridge)
            self.client.sai_thrift_remove_port(port1)
            self.client.sai_thrift_remove_port(port2)
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_delete_vlan(vlan_oid)


@group('l2')
class L21QBridgeTrunk2TrunkTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        # switch_init(self.client)
        vlan_id = 12
        trunk_pvid = 20
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        # Get HW ports
        attr_list = [sai_thrift_attribute_t(id=sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST)]
        attr_list = self.client.sai_thrift_get_switch_attribute(attr_list)
        for attr in attr_list.attr_list:
            if attr.id == sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST:
                hw_port_list = attr.value.u32list.u32list
            # sai_switch_attr.SAI_SWITCH_ATTR_PORT_NUMBER
        # chosen_hw_ports = random.sample(hw_port_list, 2)
        # hw_port1 = chosen_hw_ports[0]
        # hw_port2 = chosen_hw_ports[1]
        hw_port1 = hw_port_list[0]
        hw_port2 = hw_port_list[1]
        print "Sending L2 packet Trunk(%d) -> Trunk(%d) (trunk vlan=%d)" % (hw_port1, hw_port2, vlan_id)

        # Create Ports
        bind_mode = sai_port_bind_mode.SAI_PORT_BIND_MODE_SUB_PORT
        port1 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port1)
        port2 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port2)

        # Create 1Q Bridge
        bridge_type = sai_bridge_type.SAI_BRIDGE_TYPE_1Q
        bridge_attr_value = sai_thrift_attribute_value_t(s32= bridge_type)
        bridge_attr = sai_thrift_attribute_t(id=sai_bridge_attr.SAI_BRIDGE_ATTR_TYPE, value=bridge_attr_value)
        bridge = self.client.sai_thrift_create_bridge([bridge_attr])

        # Create Bridge ports
        bridge_port_type = sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_PORT
        bridge_port1 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port1, vlan_id, bridge)
        bridge_port2 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port2, trunk_pvid, bridge)

        # Create VLAN
        vlan_attr_value = sai_thrift_attribute_value_t(u16= vlan_id)
        vlan_attr = sai_thrift_attribute_t(id=sai_vlan_attr.SAI_VLAN_ATTR_VLAN_ID, value=vlan_attr_value)
        vlan_oid = self.client.sai_thrift_create_vlan([vlan_attr])

        # Set Port attr
        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=sai_port_attr.SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port1, tagging_mode)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port2, tagging_mode)
        # SAI_VLAN_ATTR_MEMBER_LIST

        # Create FDB Entries:
        mac_action = sai_packet_action.SAI_PACKET_ACTION_FORWARD
        fdb_entry_type = sai_fdb_entry_type.SAI_FDB_ENTRY_TYPE_STATIC
        sai_thrift_create_fdb(self.client, mac1, bridge_type, vlan_id, bridge, bridge_port1, mac_action, fdb_entry_type)
        sai_thrift_create_fdb(self.client, mac2, bridge_type, vlan_id, bridge, bridge_port2, mac_action, fdb_entry_type)
        
        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                dl_vlan_enable=True,
                                vlan_vid=vlan_id,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                dl_vlan_enable=True,
                                vlan_vid=vlan_id,
                                ip_ttl=64)
        try:
            send_packet(self, hw_port1, str(pkt))
            verify_packets(self, exp_pkt, [hw_port2])
        finally:
            sai_thrift_delete_fdb(self.client, mac1, bridge)
            sai_thrift_delete_fdb(self.client, mac2, bridge)
            self.client.sai_thrift_remove_port(port1)
            self.client.sai_thrift_remove_port(port2)
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_delete_vlan(vlan_oid)

@group('l2')
class L21QBridgeTrunk2AccessTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        # switch_init(self.client)
        vlan_id = 13
        trunk_pvid = 20
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        # Get HW ports
        attr_list = [sai_thrift_attribute_t(id=sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST)]
        attr_list = self.client.sai_thrift_get_switch_attribute(attr_list)
        for attr in attr_list.attr_list:
            if attr.id == sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST:
                hw_port_list = attr.value.u32list.u32list
            # sai_switch_attr.SAI_SWITCH_ATTR_PORT_NUMBER
        # chosen_hw_ports = random.sample(hw_port_list, 2)
        # hw_port1 = chosen_hw_ports[0]
        # hw_port2 = chosen_hw_ports[1]
        hw_port1 = hw_port_list[0]
        hw_port2 = hw_port_list[1]
        print "Sending L2 packet Trunk(%d) -> Access(%d) (trunk vlan=%d)" % (hw_port1, hw_port2, vlan_id)

        # Create Ports
        bind_mode = sai_port_bind_mode.SAI_PORT_BIND_MODE_SUB_PORT #  .1Q
        port1 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port1)
        port2 = sai_thrift_create_port(self.client, vlan_id, bind_mode, hw_port2)

        # Create 1Q Bridge
        bridge_type = sai_bridge_type.SAI_BRIDGE_TYPE_1Q
        bridge_attr_value = sai_thrift_attribute_value_t(s32= bridge_type)
        bridge_attr = sai_thrift_attribute_t(id=sai_bridge_attr.SAI_BRIDGE_ATTR_TYPE, value=bridge_attr_value)
        bridge = self.client.sai_thrift_create_bridge([bridge_attr])

        # Create Bridge ports
        bridge_port_type = sai_bridge_port_type.SAI_BRIDGE_PORT_TYPE_PORT
        bridge_port1 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port1, vlan_id, bridge)
        bridge_port2 = sai_thrift_create_bridge_port(self.client, bridge_port_type, port2, trunk_pvid, bridge)

        # Create VLAN
        vlan_attr_value = sai_thrift_attribute_value_t(u16= vlan_id)
        vlan_attr = sai_thrift_attribute_t(id=sai_vlan_attr.SAI_VLAN_ATTR_VLAN_ID, value=vlan_attr_value)
        vlan_oid = self.client.sai_thrift_create_vlan([vlan_attr])

        # Set Port attr
        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=sai_port_attr.SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_TAGGED
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port1, tagging_mode)
        tagging_mode = sai_vlan_tagging_mode.SAI_VLAN_TAGGING_MODE_UNTAGGED
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, bridge_port2, tagging_mode)
        # SAI_VLAN_ATTR_MEMBER_LIST

        # Create FDB Entries:
        mac_action = sai_packet_action.SAI_PACKET_ACTION_FORWARD
        fdb_entry_type = sai_fdb_entry_type.SAI_FDB_ENTRY_TYPE_STATIC
        sai_thrift_create_fdb(self.client, mac1, bridge_type, vlan_id, bridge, bridge_port1, mac_action, fdb_entry_type)
        sai_thrift_create_fdb(self.client, mac2, bridge_type, vlan_id, bridge, bridge_port2, mac_action, fdb_entry_type)
        
        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                dl_vlan_enable=True,
                                vlan_vid=vlan_id,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64,
                                pktlen=96)
        try:
            send_packet(self, hw_port1, str(pkt))
            verify_packets(self, exp_pkt, [hw_port2])
        finally:
            sai_thrift_delete_fdb(self.client, mac1, bridge)
            sai_thrift_delete_fdb(self.client, mac2, bridge)
            self.client.sai_thrift_remove_port(port1)
            self.client.sai_thrift_remove_port(port2)
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_delete_vlan(vlan_oid)

@group('l2')
@group('lag')
class L2LagTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        switch_init(self.client)
        default_vlan = 1
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        self.client.sai_thrift_create_vlan(vlan_id)

        lag_id1 = self.client.sai_thrift_create_lag([])

        sai_thrift_vlan_remove_all_ports(self.client, default_vlan)

        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag_id1, port1)
        lag_member_id2 = sai_thrift_create_lag_member(self.client, lag_id1, port2)
        lag_member_id3 = sai_thrift_create_lag_member(self.client, lag_id1, port3)

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_id, lag_id1, SAI_VLAN_PORT_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_id, port4, SAI_VLAN_PORT_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(lag_id1, attr)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        sai_thrift_create_fdb(self.client, vlan_id, mac1, lag_id1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_id, mac2, port4, mac_action)

        try:
            count = [0, 0, 0]
            dst_ip = int(socket.inet_aton('10.10.10.1').encode('hex'),16)
            max_itrs = 200
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(hex(dst_ip)[2:].zfill(8).decode('hex'))
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_dst=dst_ip_addr,
                                        ip_src='192.168.8.1',
                                        ip_id=109,
                                        ip_ttl=64)

                exp_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_dst=dst_ip_addr,
                                            ip_src='192.168.8.1',
                                            ip_id=109,
                                            ip_ttl=64)

                send_packet(self, 3, str(pkt))
                rcv_idx = verify_any_packet_any_port(self, [exp_pkt], [0, 1, 2])
                count[rcv_idx] += 1
                dst_ip += 1

            print count
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                        "Not all paths are equally balanced")

            pkt = simple_tcp_packet(eth_src='00:11:11:11:11:11',
                                    eth_dst='00:22:22:22:22:22',
                                    ip_dst='10.0.0.1',
                                    ip_id=109,
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_src='00:11:11:11:11:11',
                                    eth_dst='00:22:22:22:22:22',
                                    ip_dst='10.0.0.1',
                                    ip_id=109,
                                    ip_ttl=64)
            print "Sending packet port 1 (lag member) -> port 4"
            send_packet(self, 0, str(pkt))
            verify_packets(self, exp_pkt, [3])
            print "Sending packet port 2 (lag member) -> port 4"
            send_packet(self, 1, str(pkt))
            verify_packets(self, exp_pkt, [3])
            print "Sending packet port 3 (lag member) -> port 4"
            send_packet(self, 2, str(pkt))
            verify_packets(self, exp_pkt, [3])
        finally:

            sai_thrift_delete_fdb(self.client, vlan_id, mac1, lag_id1)
            sai_thrift_delete_fdb(self.client, vlan_id, mac2, port4)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)

            self.client.sai_thrift_remove_lag_member(lag_member_id1)
            self.client.sai_thrift_remove_lag_member(lag_member_id2)
            self.client.sai_thrift_remove_lag_member(lag_member_id3)
            self.client.sai_thrift_remove_lag(lag_id1)
            self.client.sai_thrift_delete_vlan(vlan_id)

            for port in sai_port_list:
                sai_thrift_create_vlan_member(self.client, default_vlan, port, SAI_VLAN_PORT_UNTAGGED)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port4, attr)
