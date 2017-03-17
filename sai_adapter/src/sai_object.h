#include <iostream>

//SAI
#ifdef __cplusplus
extern "C" {
#endif
#include <sai.h>
#ifdef __cplusplus
}
#endif

// #include <saifdb.h>
// #include <saivlan.h>
// #include <sairouter.h>
// #include <sairouterintf.h>
// #include <sairoute.h>
// #include <saiswitch.h>
// #include <saimirror.h>
// #include <saistatus.h>

// INTERNAL
#include "switch_meta_data.h"


// thrift bm clinet
#include <Standard.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TMultiplexedProtocol.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TSocket.h>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace bm_runtime::standard;

const int bm_port = 9090;
const int32_t cxt_id =0;

// static StandardClient* bm_client_ptr;
// static sai_id_map_t* sai_id_map_ptr;

class sai_object {
public:
	//thrift
	boost::shared_ptr<TTransport> socket;
  	boost::shared_ptr<TTransport> transport;
  	boost::shared_ptr<TProtocol>  bprotocol;
  	boost::shared_ptr<TProtocol>  protocol;
  	StandardClient bm_client;
	// generals
  	sai_id_map_t sai_id_map;
	Switch_metadata switch_metadata;
	std::vector<sai_object_id_t> switch_list;
	uint32_t list[8]={0,1,2,3,4,5,6,7};
	static std::vector<sai_object_id_t> * switch_list_ptr;
	static sai_id_map_t* sai_id_map_ptr;
	static StandardClient* bm_client_ptr;
    static Switch_metadata* switch_metadata_ptr;

    //switch
	static sai_status_t create_switch(sai_object_id_t* switch_id, uint32_t attr_count, const sai_attribute_t *attr_list);
	static sai_status_t get_switch_attribute(sai_object_id_t switch_id, sai_uint32_t attr_count, sai_attribute_t *attr_list);
	//port functions
	static sai_status_t create_port (sai_object_id_t *port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_port (sai_object_id_t port_id);
	static sai_status_t set_port_attribute(sai_object_id_t port_id, const sai_attribute_t *attr);
	static sai_status_t get_port_attribute(sai_object_id_t port_id, uint32_t attr_count, sai_attribute_t *attr_list);
	static void 		config_port	(Port_obj* port);
	static void         set_parsed_port_attribute(Port_obj* port, sai_attribute_t attribute);
	static void         get_parsed_port_attribute(Port_obj* port, sai_attribute_t *attribute);
	//bridge functions
	static sai_status_t create_bridge (sai_object_id_t *bridge_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_bridge (sai_object_id_t bridge_id);
	static sai_status_t get_bridge_attribute(sai_object_id_t bridge_id, uint32_t attr_count, sai_attribute_t *attr_list);
	
	//bridge_port functions
	static sai_status_t create_bridge_port (sai_object_id_t *bridge_port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_bridge_port (sai_object_id_t bridge_port_id);
	static sai_status_t get_bridge_port_attribute(sai_object_id_t bridge_port_id, uint32_t attr_count, sai_attribute_t *attr_list);
	//fdb
	static sai_status_t create_fdb_entry (const sai_fdb_entry_t* fdb_entry,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_fdb_entry (const sai_fdb_entry_t* fdb_entry);	
	//vlan
	static sai_status_t create_vlan (sai_object_id_t *vlan_id , sai_object_id_t switch_id, uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_vlan (sai_object_id_t vlan_id);
	static sai_status_t set_vlan_attribute (sai_object_id_t vlan_id, const sai_attribute_t *attr);
	static sai_status_t get_vlan_attribute (sai_object_id_t vlan_id, const uint32_t attr_count, sai_attribute_t *attr_list);
	static sai_status_t create_vlan_member (sai_object_id_t *vlan_member_id, sai_object_id_t switch_id, uint32_t attr_count, const sai_attribute_t *attr_list);
	static sai_status_t remove_vlan_member (sai_object_id_t vlan_member_id);
	static sai_status_t set_vlan_member_attribute (sai_object_id_t vlan_member_id, const sai_attribute_t *attr);
	static sai_status_t get_vlan_member_attribute (sai_object_id_t vlan_member_id, const uint32_t attr_count, sai_attribute_t *attr_list);
	static sai_status_t get_vlan_stats (sai_object_id_t vlan_id, const sai_vlan_stat_t *counter_ids, uint32_t number_of_counters, uint64_t *counters);
	static sai_status_t clear_vlan_stats (sai_object_id_t vlan_id, const sai_vlan_stat_t *counter_ids, uint32_t number_of_counters);
	//lag
	static sai_status_t create_lag(sai_object_id_t *lag_id,sai_object_id_t switch_id, uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_lag(sai_object_id_t lag_id);
	static sai_status_t create_lag_member(sai_object_id_t *lag_member_id,sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_lag_member(sai_object_id_t lag_member_id);

	//api s
	sai_port_api_t		port_api;
	sai_bridge_api_t	bridge_api;
	sai_fdb_api_t		fdb_api;
	sai_switch_api_t    switch_api;
	sai_vlan_api_t		vlan_api;
	sai_lag_api_t		lag_api;
	sai_hostif_api_t    hostif_api;
	sai_object():
	//  constructor pre initializations
	  socket(new TSocket("localhost", bm_port)),
	  transport(new TBufferedTransport(socket)),
	  bprotocol(new TBinaryProtocol(transport)),
	  protocol (new TMultiplexedProtocol(bprotocol, "standard")),
	  bm_client(protocol)
	  {	
	  	switch_list_ptr = &switch_list;
  		switch_metadata_ptr = &switch_metadata;
  		switch_metadata.hw_port_list.list=list;
  		switch_metadata.hw_port_list.count=8;
  		bm_client_ptr = &bm_client;
  		sai_id_map_ptr = &sai_id_map;
	  	transport->open();

	  	//api set
	  	switch_api.create_switch 		= &sai_object::create_switch;
	  	switch_api.get_switch_attribute = &sai_object::get_switch_attribute;
  		
  		port_api.create_port  			= &sai_object::create_port;
  		port_api.remove_port			= &sai_object::remove_port;
  		port_api.set_port_attribute 	= &sai_object::set_port_attribute;
  		port_api.get_port_attribute 	= &sai_object::get_port_attribute;

  		bridge_api.create_bridge 		= &sai_object::create_bridge;
  		bridge_api.remove_bridge 		= &sai_object::remove_bridge;
  		bridge_api.get_bridge_attribute = &sai_object::get_bridge_attribute;
  		bridge_api.create_bridge_port 	= &sai_object::create_bridge_port;
  		bridge_api.remove_bridge_port 	= &sai_object::remove_bridge_port;
  		bridge_api.get_bridge_port_attribute = &sai_object::get_bridge_port_attribute;

  		fdb_api.create_fdb_entry 		= &sai_object::create_fdb_entry;
  		fdb_api.remove_fdb_entry 		= &sai_object::remove_fdb_entry;
  		
  		vlan_api.create_vlan 			= &sai_object::create_vlan;
  		vlan_api.remove_vlan 			= &sai_object::remove_vlan;
  		vlan_api.set_vlan_attribute 	= &sai_object::set_vlan_attribute;
  		vlan_api.get_vlan_attribute 	= &sai_object::get_vlan_attribute;
  		vlan_api.create_vlan_member 	= &sai_object::create_vlan_member;
  		vlan_api.remove_vlan_member 	= &sai_object::remove_vlan_member;
  		vlan_api.set_vlan_member_attribute = &sai_object::set_vlan_member_attribute;
  		vlan_api.get_vlan_member_attribute = &sai_object::get_vlan_member_attribute;
  		vlan_api.get_vlan_stats 		= &sai_object::get_vlan_stats;
  		vlan_api.clear_vlan_stats 		= &sai_object::clear_vlan_stats;
  		
  		lag_api.create_lag 				= &sai_object::create_lag;
  		lag_api.remove_lag 				= &sai_object::remove_lag;
  		lag_api.create_lag_member 		= &sai_object::create_lag_member;
  		lag_api.remove_lag_member 		= &sai_object::remove_lag_member;
  		//
    	printf("BM connection started on port %d\n",bm_port); 
	  }
	~sai_object(){
	 	  //deconstructor
  		transport->close();
  		delete bm_client_ptr;
  		delete sai_id_map_ptr;
  		delete switch_metadata_ptr;
    	printf("BM clients closed\n");
	 }

	sai_status_t sai_api_query(sai_api_t sai_api_id,void** api_method_table){
		switch (sai_api_id) {
            case SAI_API_PORT:
              *api_method_table=&port_api;
              break;
            case SAI_API_BRIDGE:
              *api_method_table=&bridge_api;
              break;
            case SAI_API_FDB:
              *api_method_table=&fdb_api;
              break;
            case SAI_API_SWITCH:
              *api_method_table=&switch_api;
              break;
            case SAI_API_VLAN:
              *api_method_table=&vlan_api;
              break;
            case SAI_API_LAG:
              *api_method_table =&lag_api;
              break;
            case SAI_API_HOSTIF:
              *api_method_table =&hostif_api;
              break;
         	default:
         		printf("api requested was %d, while sai_api_port is %d\n",sai_api_id,SAI_API_PORT);
         		return SAI_STATUS_FAILURE;
         	}
		return SAI_STATUS_SUCCESS;
	}
};

