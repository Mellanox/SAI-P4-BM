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
    uint32_t pvid;
    uint32_t bind_mode;
    uint32_t mtu;
    uint32_t drop_tagged;
    uint32_t drop_untagged;
    BmEntryHandle handle_lag_if;
    BmEntryHandle handle_port_cfg;
    // Port_obj(sai_id_map_t* sai_id_map_ptr,uint32_t hw_port, uint32_t pvid) : Sai_obj(sai_id_map_ptr) {
    //   //printf("create port object\n");
    //   this->mtu=1512;
    //   this->drop_tagged=0;
    //   this->drop_untagged=0;
    //   this->hw_port=hw_port;
    //   this->pvid=pvid;
    //   this->bind_mode=SAI_PORT_BIND_MODE_PORT;
    // }
    Port_obj(sai_id_map_t* sai_id_map_ptr): Sai_obj(sai_id_map_ptr) {
      //printf("create port object\n");
      this->mtu=1512;
      this->drop_tagged=0;
      this->drop_untagged=0;
      this->hw_port=0;
      this->pvid=1;
      this->bind_mode=SAI_PORT_BIND_MODE_PORT;
    }   
};

class BridgePort_obj : public Sai_obj {
public:
  sai_object_id_t port_id;
  sai_object_id_t vlan_id;
  sai_port_type_t br_port_type;
  BridgePort_obj(sai_id_map_t* sai_id_map_ptr,sai_object_id_t port_id,sai_object_id_t vlan_id, sai_port_type_t br_port_type) : Sai_obj(sai_id_map_ptr) {
    this->port_id=port_id;
    this->vlan_id=vlan_id;
    this->br_port_type=br_port_type;
  }
  BridgePort_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->port_id=0;
    this->vlan_id=1;
    this->br_port_type=SAI_PORT_TYPE_LOGICAL;
  }
};

class Bridge_obj : public Sai_obj {
public:
  uint32_t bridge_type; // sai_bridge_type_t
  Bridge_obj(sai_id_map_t* sai_id_map_ptr,uint32_t bridge_type) : Sai_obj(sai_id_map_ptr) {
    this->bridge_type=bridge_type;
  }
};



typedef std::map<sai_object_id_t, BridgePort_obj*>  bridge_port_id_map_t;
typedef std::map<sai_object_id_t, Port_obj*>        port_id_map_t;
typedef std::map<sai_object_id_t, Bridge_obj*>      bridge_id_map_t;



class Switch_metadata { // this object_id is the switch_id
public:
//  sai_id_map_t sai_id_map; // TODO should come from higher hirarchy (for multiple switch config)
  sai_u32_list_t        hw_port_list;
  port_id_map_t         ports;
  bridge_port_id_map_t  bridge_ports;
  bridge_id_map_t       bridges;
  Switch_metadata(){

    ports.clear();
    bridge_ports.clear();
    bridges.clear();
  }
};
// class Switch_metadata : public Sai_obj { // this object_id is the switch_id
// public:
// //  sai_id_map_t sai_id_map; // TODO should come from higher hirarchy (for multiple switch config)
//   sai_u32_list_t hw_port_list;
//   port_id_map_t ports;
//   bridge_port_id_map_t bridge_ports;
//   bridge_id_map_t bridges;
//   Switch_metadata(sai_id_map_t id_map) : Sai_Obj(id_map) {

//   }
  
// };


#endif
//
//
