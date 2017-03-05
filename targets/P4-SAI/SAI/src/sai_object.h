#include <iostream>

//SAI
#ifdef __cplusplus
extern "C" {
#endif
#include <sai.h>
#ifdef __cplusplus
}
#endif

#include <saifdb.h>
#include <saivlan.h>
#include <sairouter.h>
#include <sairouterintf.h>
#include <sairoute.h>
#include <saiswitch.h>
#include <saimirror.h>
#include <saistatus.h>

// INTERNAL
#include "switch_meta_data.h"


// thrift bm clinet
#include "../../../../thrift_src/gen-cpp/bm/Standard.h"
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
	uint32_t list[8]={0,1,2,3,4,5,6,7};
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
	static void 		config_port	(Port_obj* port);
	static void         parse_port_attribute(Port_obj* port, sai_attribute_t attribute);
	//bridge functions
	static sai_status_t create_bridge (sai_object_id_t *bridge_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_bridge (sai_object_id_t bridge_id);
	//bridge_port functions
	static sai_status_t create_bridge_port (sai_object_id_t *bridge_port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_bridge_port (sai_object_id_t bridge_port_id);
	//fdb
	static sai_status_t create_fdb_entry (const sai_fdb_entry_t* fdb_entry,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_fdb_entry (const sai_fdb_entry_t* fdb_entry);	
	//api s
	sai_port_api_t		port_api;
	sai_bridge_api_t	bridge_api;
	sai_fdb_api_t		fdb_api;
	sai_switch_api_t    switch_api;

	sai_object():
	//  constructor pre initializations
	  socket(new TSocket("localhost", bm_port)),
	  transport(new TBufferedTransport(socket)),
	  bprotocol(new TBinaryProtocol(transport)),
	  protocol (new TMultiplexedProtocol(bprotocol, "standard")),
	  bm_client(protocol)
	  {		
  		switch_metadata_ptr = &switch_metadata;
  		switch_metadata.hw_port_list.list=list;
  		switch_metadata.hw_port_list.count=8;
  		bm_client_ptr = &bm_client;
  		sai_id_map_ptr = &sai_id_map;
	  	transport->open();

	  	//api set
	  	switch_api.create_switch = &sai_object::create_switch;
	  	switch_api.get_switch_attribute = &sai_object::get_switch_attribute;
  		port_api.create_port  	= &sai_object::create_port;
  		port_api.remove_port	= &sai_object::remove_port;
  		port_api.set_port_attribute = &sai_object::set_port_attribute;

  		bridge_api.create_bridge = &sai_object::create_bridge;
  		bridge_api.remove_bridge = &sai_object::remove_bridge;
  		bridge_api.create_bridge_port = &sai_object::create_bridge_port;
  		bridge_api.remove_bridge_port = &sai_object::remove_bridge_port;

  		fdb_api.create_fdb_entry = &sai_object::create_fdb_entry;
  		fdb_api.remove_fdb_entry = &sai_object::remove_fdb_entry;
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
         	default:
         		printf("api requested was %d, while sai_api_port is %d\n",sai_api_id,SAI_API_PORT);
         		//*api_method_table=&port_api; ///TODOOOOO remove!!
         		return SAI_STATUS_FAILURE; ///TODOOOOO enable!!
         	}
		return SAI_STATUS_SUCCESS;
	}
};

