#ifndef SWITCH_META_DATA_H
#define SWITCH_META_DATA_H

#define NULL_HANDLE -1
#define NUM_OF_PORTS 32

#include "spdlog/spdlog.h"
#include <iostream>
#include <list>
#include <map>
#include <sai.h>
#include <bm/standard_types.h>
#include <bm/simple_pre_lag_types.h>
#include <vector>

using namespace bm_runtime::standard;
using namespace bm_runtime::simple_pre_lag;

class sai_id_map_t { // object pointer and it's id
protected:
  std::map<sai_object_id_t, void *> id_map;
  std::vector<sai_object_id_t> unused_id;

public:
  sai_id_map_t() {
    // init
    unused_id.clear();
    id_map.clear();
  }

  ~sai_id_map_t() {}

  void free_id(sai_object_id_t sai_object_id) {
    spdlog::get("logger")->debug("freeing object with sai_id {}",
                                 sai_object_id);
    delete id_map[sai_object_id];
    id_map.erase(sai_object_id);
    unused_id.push_back(sai_object_id);
  }

  sai_object_id_t get_new_id(void *obj_ptr) { // pointer to object
    sai_object_id_t id;
    if (!unused_id.empty()) {
      id = unused_id.back();
      unused_id.pop_back();
    } else {
      id = id_map.size() + 1;
    }
    id_map[id] = obj_ptr;
    return id;
  }

  void *get_object(sai_object_id_t sai_object_id) {
    return id_map[sai_object_id];
  }
};

class Sai_obj {
public:
  sai_object_id_t sai_object_id; // TODO maybe use the map and don't save here
  Sai_obj(sai_id_map_t *sai_id_map_ptr) {
    sai_object_id =
        sai_id_map_ptr->get_new_id(this); // sai_id_map. set map to true.
  }
  ~Sai_obj() {
    // free_id(sai_object_id); TODO: fix this
  }
};

class Port_obj : public Sai_obj {
public:
  uint32_t hw_port;
  uint32_t l2_if;
  uint32_t pvid;
  uint32_t bind_mode;
  uint32_t mtu;
  uint32_t drop_tagged;
  uint32_t drop_untagged;
  int ifi_index;
  bool internal;
  bool is_lag;
  bool admin_state;
  sai_port_oper_status_t oper_status;
  BmEntryHandle handle_lag_if;
  BmEntryHandle handle_port_cfg;
  BmEntryHandle handle_ingress_lag;
  Port_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    // printf("create port object");
    this->mtu = 1512;
    this->drop_tagged = 0;
    this->drop_untagged = 0;
    this->hw_port = 0;
    this->l2_if = 0;
    this->pvid = 1;
    this->bind_mode = SAI_PORT_BIND_MODE_PORT;
    this->is_lag = false;
    this->handle_ingress_lag = NULL_HANDLE;
    this->handle_port_cfg = NULL_HANDLE;
    this->handle_lag_if = NULL_HANDLE;
    this->internal = false;
    this->admin_state = true;
    this->oper_status = SAI_PORT_OPER_STATUS_UP;
    this->ifi_index = 0;
  }
};

class BridgePort_obj : public Sai_obj {
public:
  uint32_t port_id;
  uint32_t vlan_id;
  uint32_t bridge_port;
  sai_bridge_port_type_t bridge_port_type;
  sai_object_id_t bridge_id;
  sai_bridge_port_fdb_learning_mode_t learning_mode;
  BmEntryHandle handle_id_1d;
  BmEntryHandle handle_egress_set_vlan;
  BmEntryHandle handle_egress_br_port_to_if;
  BmEntryHandle handle_subport_ingress_interface_type;
  BmEntryHandle handle_port_ingress_interface_type;
  std::map<uint32_t, BmEntryHandle>
      handle_fdb_port; // per bridge_id, valid for .1Q
  std::map<uint32_t, BmEntryHandle>
      handle_fdb_learn_port;                     // per bridge_id, valid for .1Q
  std::map<uint32_t, sai_fdb_entry_type_t>
      fdb_entry_type_port;                       // per bridge_id, valid for .1Q
  BmEntryHandle handle_fdb_sub_port;             // valid for .1D
  BmEntryHandle handle_fdb_learn_sub_port;       // valid for .1D
  sai_fdb_entry_type_t fdb_entry_type_sub_port;  // per bridge_id, valid for .1Q
  // BmEntryHandle handle_cfg; // TODO
  BridgePort_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->port_id = 0;
    this->vlan_id = 1;
    this->bridge_port = NULL;
    this->bridge_id = NULL;
    this->learning_mode = SAI_BRIDGE_PORT_FDB_LEARNING_MODE_HW;
    this->bridge_port_type = SAI_BRIDGE_PORT_TYPE_PORT;
    // TODO NULL_HANDLE is inavlid. consider other notation
    this->handle_id_1d = NULL_HANDLE;
    this->handle_egress_set_vlan = NULL_HANDLE;
    this->handle_egress_br_port_to_if = NULL_HANDLE;
    this->handle_subport_ingress_interface_type = NULL_HANDLE;
    this->handle_port_ingress_interface_type = NULL_HANDLE;
    this->handle_fdb_sub_port = NULL_HANDLE;
    this->handle_fdb_learn_sub_port = NULL_HANDLE;
  }

  bool does_fdb_exist(uint32_t bridge_id) {
    if (bridge_port_type == SAI_BRIDGE_PORT_TYPE_SUB_PORT) {
      return (handle_fdb_sub_port != NULL_HANDLE);
    } else {
      return (handle_fdb_port.count(bridge_id) == 1);
    }
  }

  void set_fdb_handle(BmEntryHandle handle_fdb, BmEntryHandle handle_learn_fdb,
                      uint32_t bridge_id, sai_fdb_entry_type_t entry_type) {
    if (bridge_port_type == SAI_BRIDGE_PORT_TYPE_SUB_PORT) {
      handle_fdb_sub_port = handle_fdb;
      handle_fdb_learn_sub_port = handle_learn_fdb;
      fdb_entry_type_sub_port = entry_type;
    } else {
      handle_fdb_port[bridge_id] = handle_fdb;
      handle_fdb_learn_port[bridge_id] = handle_learn_fdb;
      fdb_entry_type_port[bridge_id] = entry_type;
    }
  }

  void remove_fdb_handle(uint32_t bridge_id) {
    if (bridge_port_type == SAI_BRIDGE_PORT_TYPE_SUB_PORT) {
      handle_fdb_sub_port = NULL_HANDLE;
      handle_fdb_learn_sub_port = NULL_HANDLE;
    } else {
      handle_fdb_port.erase(bridge_id);
      handle_fdb_learn_port.erase(bridge_id);
    }
  }
};

class Bridge_obj : public Sai_obj {
public:
  sai_bridge_type_t bridge_type;
  std::vector<sai_object_id_t> bridge_port_list;
  uint32_t bridge_id; // Valid for .1D bridges.
  BmMcMgrpHandle handle_mc_mgrp; // Valid for .1D bridge
  BmMcL1Handle handle_mc_l1;     // Valid for .1D bridge
  Bridge_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->bridge_type = SAI_BRIDGE_TYPE_1Q;
    this->bridge_port_list.clear();
    this->bridge_id = 0;
    this->handle_mc_mgrp = NULL_HANDLE;
    this->handle_mc_l1 = NULL_HANDLE;
  }
};

class Vlan_obj : public Sai_obj {
public:
  uint16_t vid;
  std::vector<sai_object_id_t> vlan_members;
  BmEntryHandle handle_id_1q;
  BmEntryHandle handle_router_ingress_vlan_filtering;
  BmEntryHandle handle_router_egress_vlan_filtering;
  uint32_t bridge_id;             // Valid for .1Q bridge
  BmEntryHandle handle_broadcast; // Valid for .1Q bridge
  BmEntryHandle handle_flood;     // Valid for .1Q bridge
  BmMcMgrpHandle handle_mc_mgrp;  // Valid for .1Q bridge
  BmMcL1Handle handle_mc_l1;      // Valid for .1Q bridge
  Vlan_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->vlan_members.clear();
    this->vid = 0;
    this->handle_id_1q = NULL_HANDLE;
    this->handle_router_ingress_vlan_filtering = NULL_HANDLE;
    this->handle_router_egress_vlan_filtering = NULL_HANDLE;
    this->handle_mc_mgrp = NULL_HANDLE;
    this->handle_mc_l1 = NULL_HANDLE;
    this->handle_flood = NULL_HANDLE;
    this->handle_broadcast = NULL_HANDLE;
  }
};

class Vlan_member_obj : public Sai_obj {
public:
  sai_object_id_t bridge_port_id;
  sai_object_id_t vlan_oid;
  uint32_t tagging_mode;
  uint16_t vid;
  BmEntryHandle handle_egress_vlan_tag;
  BmEntryHandle handle_egress_vlan_filtering;
  BmEntryHandle handle_ingress_vlan_filtering;
  Vlan_member_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->vid = NULL_HANDLE;
    this->vlan_oid = NULL_HANDLE; // TODO needed? consider remove.
    this->tagging_mode = SAI_VLAN_TAGGING_MODE_UNTAGGED;
    this->bridge_port_id = NULL_HANDLE;
  }
};

class Lag_obj : public Sai_obj {
public:
  uint32_t l2_if;
  Port_obj *port_obj;
  std::vector<sai_object_id_t> lag_members;
  BmEntryHandle handle_lag_hash;
  BmEntryHandle handle_port_configurations;
  Lag_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->lag_members.clear();
    this->l2_if = 0;
    this->handle_lag_hash = NULL_HANDLE;
    this->handle_port_configurations = NULL_HANDLE;
    this->port_obj = nullptr;
  }
};

class Lag_member_obj : public Sai_obj {
public:
  Port_obj *port;
  Lag_obj *lag;
  BmEntryHandle handle_egress_lag;
  Lag_member_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->port = NULL;
    this->lag = NULL;
    this->handle_egress_lag = NULL_HANDLE;
  }
};

class HostIF_obj : public Sai_obj {
public:
  union netdev_obj {
    Port_obj *port;
    Lag_obj  *lag;
    Vlan_obj *vlan;
  } netdev_obj;
  sai_object_type_t netdev_obj_type;
  sai_hostif_type_t hostif_type;
  std::string netdev_name;
  int netdev_fd;
  bool oper_status;
  // std::thread netdev_thread;
  // netdev_fd_t netdev;
  HostIF_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->netdev_obj.port = nullptr;
    this->hostif_type = SAI_HOSTIF_TYPE_NETDEV;
    this->netdev_name = "";
    this->oper_status = true;
  }
};

class HostIF_Table_Entry_obj : public Sai_obj {
public:
  sai_hostif_table_entry_type_t entry_type;
  sai_hostif_table_entry_channel_type_t channel_type;
  uint16_t trap_id;
  HostIF_Table_Entry_obj(sai_id_map_t *sai_id_map_ptr)
      : Sai_obj(sai_id_map_ptr) {
    this->trap_id = 0;
    this->entry_type = SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID;
    this->channel_type =
        SAI_HOSTIF_TABLE_ENTRY_CHANNEL_TYPE_NETDEV_PHYSICAL_PORT;
  }
};

class HostIF_Trap_obj : public Sai_obj {
public:
  sai_hostif_trap_type_t trap_type;
  sai_packet_action_t trap_action;
  uint16_t trap_id;
  BmEntryHandle handle_trap;
  BmEntryHandle handle_trap_id;
  HostIF_Trap_obj(sai_id_map_t *sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->trap_id = 0;
  }
};

class HostIF_Trap_Group_obj : public Sai_obj {
public:
  HostIF_Trap_Group_obj(sai_id_map_t *sai_id_map_ptr)
      : Sai_obj(sai_id_map_ptr) {}
};

class VirtualRouter_obj : public Sai_obj {
public:
  bool v4_state;
  bool v6_state;
  uint32_t vrf;
  VirtualRouter_obj(sai_id_map_t *sai_id_map_ptr)
      : Sai_obj(sai_id_map_ptr) {
        this->vrf = 0;
        this->v4_state = true;
        this->v6_state = true;
      }
};

class RouterInterface_obj : public Sai_obj {
public:
  sai_mac_t mac;
  bool mac_valid;
  uint16_t vid;
  uint32_t rif_id;
  VirtualRouter_obj* vr;
  sai_router_interface_type_t type;
  BmEntryHandle handle_l3_interface;
  BmEntryHandle handle_egress_vlan_tag;
  BmEntryHandle handle_egress_l3;
  BmEntryHandle handle_ingress_l3;
  BmEntryHandle handle_ingress_vrf;
  RouterInterface_obj(sai_id_map_t *sai_id_map_ptr)
      : Sai_obj(sai_id_map_ptr) {
        this->vid = 1;
        this->mac_valid = false;
        this->rif_id = 0;
        this->type = SAI_ROUTER_INTERFACE_TYPE_VLAN;
        this->vr = nullptr;
        this->handle_l3_interface = NULL_HANDLE;
        this->handle_egress_vlan_tag = NULL_HANDLE;
        this->handle_egress_l3 = NULL_HANDLE;
        this->handle_ingress_l3 = NULL_HANDLE;
        this->handle_ingress_vrf = NULL_HANDLE;
      }
};

class NextHop_obj : public Sai_obj {
public:
  uint32_t nhop_id;
  sai_next_hop_type_t type;
  sai_ip_address_t ip;
  RouterInterface_obj *rif;
  BmEntryHandle hanlde_table_nhop;
  NextHop_obj(sai_id_map_t *sai_id_map_ptr)
      : Sai_obj(sai_id_map_ptr) {
        this->nhop_id = 0;
        this->type = SAI_NEXT_HOP_TYPE_IP;
        this->rif = NULL;
        this->hanlde_table_nhop = NULL_HANDLE;
      }
};



typedef std::map<sai_object_id_t, BridgePort_obj *> bridge_port_id_map_t;
typedef std::map<sai_object_id_t, Port_obj *> port_id_map_t;
typedef std::map<sai_object_id_t, Bridge_obj *> bridge_id_map_t;
typedef std::map<sai_object_id_t, Vlan_obj *> vlan_id_map_t;
typedef std::map<sai_object_id_t, Vlan_member_obj *> vlan_member_id_map_t;
typedef std::map<sai_object_id_t, Lag_obj *> lag_id_map_t;
typedef std::map<sai_object_id_t, uint32_t> l2_if_map_t;
typedef std::map<sai_object_id_t, Lag_member_obj *> lag_member_id_map_t;
typedef std::map<sai_object_id_t, HostIF_obj *> hostif_id_map_t;
typedef std::map<sai_object_id_t, HostIF_Table_Entry_obj *>
    hostif_table_entry_id_map_t;
typedef std::map<sai_object_id_t, HostIF_Trap_obj *> hostif_trap_id_map_t;
typedef std::map<sai_object_id_t, HostIF_Trap_Group_obj *>
    hostif_trap_group_id_map_t;
typedef std::map<sai_object_id_t, VirtualRouter_obj *> vr_id_map_t;
typedef std::map<sai_object_id_t, RouterInterface_obj *> rif_id_map_t;
typedef std::map<sai_object_id_t, NextHop_obj *> nhop_id_map_t;
class Switch_metadata { 
public:
  sai_object_id_t switch_id;
  std::vector<int> hw_port_list;
  port_id_map_t ports;
  bridge_port_id_map_t bridge_ports;
  bridge_id_map_t bridges;
  vlan_id_map_t vlans;
  vlan_member_id_map_t vlan_members;
  lag_id_map_t lags;
  lag_member_id_map_t lag_members;
  hostif_id_map_t hostifs;
  hostif_table_entry_id_map_t hostif_table_entries;
  hostif_trap_id_map_t hostif_traps;
  hostif_trap_group_id_map_t hostif_trap_groups;
  vr_id_map_t vrs;
  rif_id_map_t rifs;
  nhop_id_map_t nhops;
  sai_object_id_t default_bridge_id;
  sai_object_id_t default_vlan_oid;
  sai_object_id_t cpu_port_id;
  sai_object_id_t default_vr_id;
  sai_object_id_t default_trap_group;
  BridgePort_obj *router_bridge_port;
  sai_mac_t default_switch_mac;
  sai_fdb_event_notification_fn fdb_event_notification_fn;
  sai_port_state_change_notification_fn port_state_change_notification_fn;
  Switch_metadata() {
    for (int i = 0; i < NUM_OF_PORTS; i++){
      hw_port_list.push_back(i);
    }
    fdb_event_notification_fn = NULL;
    port_state_change_notification_fn = NULL;
    memset(default_switch_mac, 0, 6);
  }

  HostIF_Table_Entry_obj *GetTableEntryFromTrapID(uint16_t trap_id, sai_hostif_table_entry_type_t entry_type) {
    for (hostif_table_entry_id_map_t::iterator it =
             hostif_table_entries.begin();
         it != hostif_table_entries.end(); ++it) {
      if ((it->second->trap_id == trap_id) && (it->second->entry_type == entry_type)) {
        return it->second;
      }
    }
    spdlog::get("logger")->warn("hostif_table_entry not found for trap_id {} ",
                                 trap_id);
    return nullptr;
  }

  BridgePort_obj *GetBrPortObjFromBrPort(uint16_t bridge_port) {
    for (bridge_port_id_map_t::iterator it = bridge_ports.begin();
         it != bridge_ports.end(); ++it) {
      if (it->second->bridge_port == bridge_port) {
        return it->second;
      }
    }
    spdlog::get("logger")->error(
        "bridge_port object not found for bridge_port {} ", bridge_port);
    return nullptr;
  }

  Port_obj *GetPortObjFromHwPort(uint16_t hw_port) {
    for (port_id_map_t::iterator it = ports.begin();
         it != ports.end(); ++it) {
      if (it->second->hw_port == hw_port) {
        return it->second;
      }
    }
    spdlog::get("logger")->error(
        "port object not found for hw_port {} ", hw_port);
    return nullptr;
  }

  sai_object_id_t GetVlanObjIdFromVid(uint16_t vid) {
    for (vlan_id_map_t::iterator it = vlans.begin(); it != vlans.end(); ++it) {
      if (it->second->vid == vid) {
        return it->first;
      }
    }
    spdlog::get("logger")->error("vlan object not found for vid {} ", vid);
    return SAI_NULL_OBJECT_ID;
  }

  HostIF_obj *GetHostIFFromPhysicalPort(int port_num) {
    for (hostif_id_map_t::iterator it = hostifs.begin(); it != hostifs.end();
         ++it) {
      spdlog::get("logger")->debug("hostif hw_port {} ",
                                   it->second->netdev_obj.port->hw_port);
      if (it->second->netdev_obj.port->hw_port == port_num) {
        return it->second;
      }
    }
    spdlog::get("logger")->error("hostif not found for physical port {} ",
                                 port_num);
    return nullptr;
  }

  HostIF_obj *GetHostIFFromVlanId(int vid) {
    for (hostif_id_map_t::iterator it = hostifs.begin(); it != hostifs.end();
         ++it) {
      spdlog::get("logger")->debug("hostif vlan_id {} ",
                                   it->second->netdev_obj.vlan->vid);
      if (it->second->netdev_obj.vlan->vid == vid) {
        return it->second;
      }
    }
    spdlog::get("logger")->error("hostif not found for vlan id {} ", vid);
    return nullptr;
  }

  uint32_t GetNewVrf() {
    std::vector<uint32_t> vrfs;
    for (vr_id_map_t::iterator it = vrs.begin();
         it != vrs.end(); ++it) {
      vrfs.push_back(it->second->vrf);
      spdlog::get("logger")->debug("{} ", it->second->vrf);
    }
    for (int i = 0; i < vrfs.size(); ++i) {
      if (std::find(vrfs.begin(), vrfs.end(), i) ==
          vrfs.end()) {
        spdlog::get("logger")->debug("-->GetNewVrf: vrf is: {} ",
                                     i);
        return i;
      }
    }
    spdlog::get("logger")->debug("--> GetNewVrf: vrf is: {} ",
                                 vrfs.size());
    return vrfs.size();
  }

  uint32_t GetNewRifId() {
    std::vector<uint32_t> rif_ids;
    for (rif_id_map_t::iterator it = rifs.begin();
         it != rifs.end(); ++it) {
      rif_ids.push_back(it->second->rif_id);
      spdlog::get("logger")->debug("{} ", it->second->rif_id);
    }
    for (int i = 0; i < rif_ids.size(); ++i) {
      if (std::find(rif_ids.begin(), rif_ids.end(), i) ==
          rif_ids.end()) {
        spdlog::get("logger")->debug("-->GetNewNextHopID: rif_id is: {} ",
                                     i);
        return i;
      }
    }
    spdlog::get("logger")->debug("--> GetNewNextHopID: rif_id is: {} ",
                                 rif_ids.size());
    return rif_ids.size();
  }

  uint32_t GetNewNextHopID() {
    std::vector<uint32_t> nexthop_ids;
    for (nhop_id_map_t::iterator it = nhops.begin();
         it != nhops.end(); ++it) {
      nexthop_ids.push_back(it->second->nhop_id);
      spdlog::get("logger")->debug("{} ", it->second->nhop_id);
    }
    for (int i = 0; i < nexthop_ids.size(); ++i) {
      if (std::find(nexthop_ids.begin(), nexthop_ids.end(), i) ==
          nexthop_ids.end()) {
        spdlog::get("logger")->debug("-->GetNewNextHopID: nhop_id is: {} ",
                                     i);
        return i;
      }
    }
    spdlog::get("logger")->debug("--> GetNewNextHopID: nhop_id is: {} ",
                                 nexthop_ids.size());
    return nexthop_ids.size();
  }

  uint32_t GetNewBridgePort() {
    std::vector<uint32_t> bridge_port_nums;
    for (bridge_port_id_map_t::iterator it = bridge_ports.begin();
         it != bridge_ports.end(); ++it) {
      bridge_port_nums.push_back(it->second->bridge_port);
      spdlog::get("logger")->debug("{} ", it->second->bridge_port);
    }
    for (int i = 0; i < bridge_port_nums.size(); ++i) {
      if (std::find(bridge_port_nums.begin(), bridge_port_nums.end(), i) ==
          bridge_port_nums.end()) {
        spdlog::get("logger")->debug("-->GetNewBridgePort: bridge_port is: {} ",
                                     i);
        return i;
      }
    }
    spdlog::get("logger")->debug("--> GetNewBridgePort: bridge_port is: {} ",
                                 bridge_port_nums.size());
    return bridge_port_nums.size();
  }

  uint32_t GetNewBridgeID(uint32_t prefered_id) {
    std::vector<uint32_t> bridge_ids;
    for (bridge_id_map_t::iterator it = bridges.begin(); it != bridges.end();
         ++it) {
      bridge_ids.push_back(it->second->bridge_id);
    }
    for (vlan_id_map_t::iterator it = vlans.begin(); it != vlans.end(); ++it) {
      bridge_ids.push_back(it->second->bridge_id);
    }

    if (std::find(bridge_ids.begin(), bridge_ids.end(), prefered_id) ==
        bridge_ids.end()) {
      return prefered_id;
    }

    for (int i = 0; i < bridge_ids.size(); ++i) {
      if (std::find(bridge_ids.begin(), bridge_ids.end(), i) ==
          bridge_ids.end()) {
        return i;
      }
    }
    return bridge_ids.size();
  }

  uint32_t GetNewL2IF() {
    std::vector<uint32_t> l2_ifs_nums;
    for (port_id_map_t::iterator it = ports.begin(); it != ports.end(); ++it) {
      l2_ifs_nums.push_back(it->second->l2_if);
    }
    for (lag_id_map_t::iterator it = lags.begin(); it != lags.end(); ++it) {
      l2_ifs_nums.push_back(it->second->l2_if);
    }
    for (int i = 0; i < l2_ifs_nums.size(); ++i) {
      if (std::find(l2_ifs_nums.begin(), l2_ifs_nums.end(), i) ==
          l2_ifs_nums.end()) {
        spdlog::get("logger")->debug("--> Get_New_L2_if: new if is: {} ", i);
        return i;
      }
    }
    spdlog::get("logger")->debug("--> Get_New_L2_if: new if is: {} ",
                                 l2_ifs_nums.size());
    return l2_ifs_nums.size();
  }

  uint16_t GetNewTrapID() {
    std::vector<uint16_t> trap_ids;
    for (hostif_trap_id_map_t::iterator it = hostif_traps.begin();
         it != hostif_traps.end(); ++it) {
      trap_ids.push_back(it->second->trap_id);
    }
    for (int i = 0; i < trap_ids.size(); ++i) {
      if (std::find(trap_ids.begin(), trap_ids.end(), i) == trap_ids.end()) {
        return i;
      }
    }
    return trap_ids.size();
  }
};

#endif
