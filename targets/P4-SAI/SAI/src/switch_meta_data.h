#ifndef SWITCH_MAETA_DATA_H
#define SWITCH_MAETA_DATA_H

#include  <sai.h>
#include <list>
#include <iostream>
#include <vector>
#include <map>


class sai_id_map_t { // object pointer and it's id
  protected:
    std::map<sai_object_id_t,void*> id_map;
    std::vector<sai_object_id_t> unused_id;
  public:
    sai_id_map_t(){
      //printf("creating id map size: %d : %d",id_map.size(),unused_id.empty());
      // init
    }
    void free_id(sai_object_id_t sai_object_id){
      id_map.erase(sai_object_id);
      unused_id.push_back(sai_object_id);
    }
    sai_object_id_t get_new_id(void* obj_ptr){//pointer to object
      sai_object_id_t id;
      if(!unused_id.empty()){
        id = unused_id.back();
        unused_id.pop_back();
      }
      else { id = id_map.size(); }
      id_map[id]=obj_ptr;
      return id;
    }
};


class Sai_obj {
  public:
    sai_object_id_t sai_object_id; // TODO maybe use the map and don't save here
    Sai_obj(sai_id_map_t sai_id_map){
      sai_object_id = sai_id_map.get_new_id(this); // sai_id_map. set map to true.
      printf("sai_object_id is %d\n",sai_object_id);
    }
    ~Sai_obj(){
    //free_id(sai_object_id); TODO: fix this
    }
  
};


class Port_obj : public Sai_obj{
  public:
    uint32_t hw_port;
    uint32_t pvid;
    uint32_t bind_mode;
    // TODO add handle
    Port_obj(sai_id_map_t id_map,uint32_t hw_port, uint32_t pvid) : Sai_obj(id_map) {
      this->hw_port=hw_port;
      this->pvid=pvid;
      this->bind_mode=SAI_PORT_BIND_MODE_PORT;
    }
    Port_obj(sai_id_map_t id_map): Sai_obj(id_map) {
      printf("create port object\n");
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
  BridgePort_obj(sai_id_map_t id_map,sai_object_id_t port_id,sai_object_id_t vlan_id, sai_port_type_t br_port_type) : Sai_obj(id_map) {
    this->port_id=port_id;
    this->vlan_id=vlan_id;
    this->br_port_type=br_port_type;
  }
  BridgePort_obj(sai_id_map_t id_map) : Sai_obj(id_map) {
    this->port_id=0;
    this->vlan_id=1;
    this->br_port_type=SAI_PORT_TYPE_LOGICAL;
  }
};

class Bridge_obj : public Sai_obj {
public:
  sai_bridge_type_t bridge_type;
  Bridge_obj(sai_id_map_t id_map,sai_bridge_type_t bridge_type) : Sai_obj(id_map) {
    this->bridge_type=bridge_type;
  }
};



typedef std::map<sai_object_id_t, BridgePort_obj*>  bridge_port_id_map_t;
typedef std::map<sai_object_id_t, Port_obj*>        port_id_map_t;
typedef std::map<sai_object_id_t, Bridge_obj*>      bridge_id_map_t;


struct switch_metatdata_t{
public:
  //sai_id_map_t sai_id_map; // TODO should come from higher hirarchy (for multiple switch config)
  Sai_obj switch_id(sai_id_map_t sai_id_map);
  sai_u32_list_t hw_port_list;
  port_id_map_t ports;
  bridge_port_id_map_t bridge_ports;
  bridge_id_map_t bridges;
};


#endif
//
//
