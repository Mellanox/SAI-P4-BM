#include "sai_object_interface.h"
#include "sai_object.h"
#include "switch_meta_data.h"

extern "C"
{
  S_O_Handle create_sai_object() { return new sai_object(); }
    void    free_sai_object(S_O_Handle p) { 
    	sai_object* q = (sai_object*) p;
    	q->~sai_object();
    	delete p; 
    }


// API
  sai_status_t sai_obj_api_query(S_O_Handle p,sai_api_t sai_api_id,void** api_method_table){
    sai_object* q = (sai_object*) p;
    return q->sai_api_query(sai_api_id,api_method_table); 
  }

// PORT
  sai_status_t sai_obj_create_port (S_O_Handle p,sai_object_id_t *port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list) {
      sai_object* q = (sai_object*) p;
      return q->create_port(port_id,switch_id,attr_count,attr_list); 
    }

  sai_status_t sai_obj_remove_port (S_O_Handle p, sai_object_id_t port_id) {
      sai_object* q = (sai_object*) p;
      return q->remove_port(port_id); 
    }

// BRIDGE
  sai_status_t sai_obj_create_bridge(S_O_Handle p,sai_object_id_t *bridge_id, sai_object_id_t switch_id, uint32_t attr_count, const sai_attribute_t *attr_list) {
    sai_object* q = (sai_object*) p;
    return q->create_bridge(bridge_id,switch_id,attr_count,attr_list); 
  }
  sai_status_t sai_obj_remove_bridge(S_O_Handle p,sai_object_id_t bridge_id) {
    sai_object* q = (sai_object*) p;
    return q->remove_bridge(bridge_id); 
  }


}