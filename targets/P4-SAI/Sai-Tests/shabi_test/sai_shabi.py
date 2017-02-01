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
 
class ShabiTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet port 0 -> port 1"
        vlan_id = 10
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        bind_mode = SAI_PORT_BIND_MODE_PORT
        hw_port0 = 0
        port0 = sai_thrift_create_port(self.client, bind_mode, hw_port0, vlan_id)
        hw_port1 = 1
        port1 = sai_thrift_create_port(self.client, bind_mode, hw_port1, vlan_id)
        print "port0 %d. port 1 %d" % (port0, port1)
        
        # bridge_type = SAI_BRIDGE_TYPE_1D
        # bridge_attr_value = sai_thrift_attribute_value_t(s32= bridge_type)
        # bridge_attr = sai_thrift_attribute_t(id=SAI_BRIDGE_ATTR_TYPE, value=bridge_attr_value)
        # bridge = self.client.sai_thrift_create_bridge([bridge_attr])