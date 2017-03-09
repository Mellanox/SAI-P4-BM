#ifndef SWITCH_META_DATA_H
#define SWITCH_META_DATA_H

#include  <sai.h>
#include <list>
#include <iostream>
#include <vector>
#include <map>
#include "../../../../thrift_src/gen-cpp/bm/standard_types.h"
using namespace bm_runtime::standard;

class sai_id_map_t { // object pointer and it's id
  protected:
    std::map<sai_object_id_t,void*> id_map;
    std::vector<sai_object_id_t> unused_id;
  public:
    sai_id_map_t(){
      //printf("sai_id_map_constructor \n",id_map.size(),unused_id.size());
      // init
      unused_id.clear();
      id_map.clear();
    }

    ~sai_id_map_t(){
      printf("sai_id_map_destructor, id_map addr: %d \n",&id_map);
    }

    void free_id(sai_object_id_t sai_object_id){
      printf("freeing object with sai_id %d\n",sai_object_id);
      delete id_map[sai_object_id];
      id_map.erase(sai_object_id);
      unused_id.push_back(sai_object_id);
    }

    sai_object_id_t get_new_id(void* obj_ptr){//pointer to object
      //printf("get_new_id\n");
      sai_object_id_t id;
      if(!unused_id.empty()){
        id = unused_id.back();
        unused_id.pop_back();
      }
      else { 
        id = id_map.size(); }
      //printf("id_map.size = %d \n", id_map.size());
      id_map[id]=obj_ptr;
      //printf("after insertion : id_map.size = %d \n", id_map.size());
      return id;
    }

    void *get_object(sai_object_id_t sai_object_id) {
      return id_map[sai_object_id];
    }
};


class Sai_obj {
  public:
    sai_object_id_t sai_object_id; // TODO maybe use the map and don't save here
    Sai_obj(sai_id_map_t* sai_id_map_ptr){
      sai_object_id = sai_id_map_ptr->get_new_id(this); // sai_id_map. set map to true.
      printf("sai_object_id is %d\n",sai_object_id);
    }
    ~Sai_obj(){
    printf("sai_object_destructor %d \n",this->sai_object_id);
    //free_id(sai_object_id); TODO: fix this
    }
  
};


class Port_obj : public Sai_obj{
  public:
    uint32_t hw_port;
    uint32_t l2_if;
    uint32_t pvid;
    uint32_t bind_mode;
    uint32_t mtu;
    uint32_t drop_tagged;
    uint32_t drop_untagged;
    BmEntryHandle handle_lag_if;
    BmEntryHandle handle_port_cfg;
    Port_obj(sai_id_map_t* sai_id_map_ptr): Sai_obj(sai_id_map_ptr) {
      //printf("create port object\n");
      this->mtu=1512;
      this->drop_tagged=0;
      this->drop_untagged=0;
      this->hw_port=0;
      this->l2_if=0;
      this->pvid=1;
      this->bind_mode=SAI_PORT_BIND_MODE_PORT;
    }   
};

class BridgePort_obj : public Sai_obj {
public:
  uint32_t port_id;
  uint32_t vlan_id;
  uint32_t bridge_port;
  sai_bridge_port_type_t bridge_port_type;
  BmEntryHandle handle_id_1d;
  BmEntryHandle handle_id_1q;
  BmEntryHandle handle_egress_set_vlan;
  BmEntryHandle handle_egress_br_port_to_if;
  BmEntryHandle handle_subport_ingress_interface_type;
  BmEntryHandle handle_port_ingress_interface_type;
  //BmEntryHandle handle_cfg; // TODO
  BridgePort_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->port_id=0;
    this->vlan_id=1;
    this->bridge_port=0;
    this->bridge_port_type=SAI_BRIDGE_PORT_TYPE_PORT;
    // TODO 999 is inavlid. consider other notation
    this->handle_id_1d =999;
    this->handle_id_1q =999;
    this->handle_egress_set_vlan =999;
    this->handle_egress_br_port_to_if =999;
    this->handle_subport_ingress_interface_type =999;
    this->handle_port_ingress_interface_type =999;

  }
};

class Bridge_obj : public Sai_obj {
public:
  sai_bridge_type_t bridge_type;
  std::vector<sai_object_id_t> bridge_port_list;
  Bridge_obj(sai_id_map_t* sai_id_map_ptr,sai_bridge_type_t bridge_type) : Sai_obj(sai_id_map_ptr) {
    this->bridge_type=bridge_type;
    this->bridge_port_list.clear();
  }
};

class Vlan_obj : public Sai_obj {
  public:
    uint16_t vid;
    std::vector<sai_object_id_t, Vlan_member*> vlan_members;
    Vlan_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
      this->vlan_members.clear();
      this->vid = 0;
    }
}

class Vlan_member_obj : public Sai_obj {
  public:
    sai_object_id_t bridge_port_id;
    sai_object_id_t vlan_oid;
    uint32_t tagging_mode;
    uint16_t vid;
    BmEntryHandle handle_egress_vlan_tag;
    BmEntryHandle handle_egress_vlan_filtering;
    BmEntryHandle handle_ingress_vlan_filtering;
    Vlan_member_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr){
      this->vid = 999;
      this->vlan_oid = 999;// TODO needed? consider remove.
      this->tagging_mode = SAI_VLAN_TAGGING_MODE_UNTAGGED;
      this->bridge_port_id=999;
    }
}

class Lag_obj : public Sai_obj {
public:
  l2_if;
  std::vector<sai_object_id_t, Lag_member*> lag_members;
  Port_obj* port_obj;
  Lag_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr){
    this->lag_members.clear();
    this->l2_if=0;
    this->port_obj=NULL;
  }
}

class Lag_member_obj : public Sai_obj{
  uint32_t port_id;
  uint32_t lag_id;
  uint32_t hw_port;
  Lag_member_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr){
    this->port_id=0;
    this->lag_id=0;
    this->hw_port=0;
  }
}

typedef std::map<sai_object_id_t, BridgePort_obj*>  bridge_port_id_map_t;
typedef std::map<sai_object_id_t, Port_obj*>        port_id_map_t;
typedef std::map<sai_object_id_t, Bridge_obj*>      bridge_id_map_t;
typedef std::map<sai_object_id_t, Vlan_obj*>        vlan_id_map_t;
typedef std::map<sai_object_id_t, Lag_obj*>         lag_id_map_t;

class Switch_metadata { // TODO:  add default.. // this object_id is the switch_id
public:
  sai_u32_list_t        hw_port_list;
  port_id_map_t         ports;
  bridge_port_id_map_t  bridge_ports;
  bridge_id_map_t       bridges;
  vlan_id_map_t         vlans;
  lag_id_map_t          lags;

  Switch_metadata(){
    ports.clear();
    bridge_ports.clear();
    bridges.clear();
    vlans.clear();
    lags.clear();
  }
};

#endif

