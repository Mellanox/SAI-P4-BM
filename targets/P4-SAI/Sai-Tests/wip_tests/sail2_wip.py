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
from sai_types import *
 
@group('l2_wip')
class L2WIP(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet port 0 -> port 1"
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        vlan_id = 10

        attr_list = [SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID]
        attr_list = self.client.sai_thrift_get_switch_attribute(attr_list)
        bridge = attr_list[0].value.oid
        attr_list = [SAI_BRIDGE_ATTR_PORT_LIST]
        attr_list = self.client.sai_thirft_get_bridge_attribute(bridge, attr_list)
        bridge_port_list = attr_list[0].value.objlist
        bridge_port1 = bridge_port_list[0]
        bridge_port2 = bridge_port_list[1]
        attr_list = [SAI_BRIDGE_PORT_ATTR_PORT_ID]
        attr_list = self.client.sai_thirft_get_bridge_port_attribute(bridge_port1, attr_list)
        port1 = attr_list[0].value.oid
        attr_list = [SAI_BRIDGE_PORT_ATTR_PORT_ID]
        attr_list = self.client.sai_thirft_get_bridge_port_attribute(bridge_port2, attr_list)
        port2 = attr_list[0].value.oid

        # TODO:   port1 -> 2 passes tagged drops untagged  port2 -> 1 drops tagged passed untagged
        
        # Create FDB Entries:
        mac_action = SAI_PACKET_ACTION_FORWARD
        fdb_entry_type = SAI_FDB_ENTRY_TYPE_STATIC
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
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_delete_vlan(vlan_oid)
            self.client.sai_thrift_remove_bridge_port(bridge_port1)
            self.client.sai_thrift_remove_bridge_port(bridge_port2)
            self.client.sai_thrift_remove_bridge(bridge)
            self.client.sai_thrift_remove_port(port1)
            self.client.sai_thrift_remove_port(port2)