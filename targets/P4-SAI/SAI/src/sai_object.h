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
  	sai_id_map_t sai_id_map;
	// generals
	static sai_id_map_t* sai_id_map_ptr;
	//sai_id_map_t* sai_id_map_ptr;
	static StandardClient* bm_client_ptr;
    static Switch_metadata* switch_metadata_ptr;

	//port functions
	static sai_status_t create_port (sai_object_id_t *port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	static sai_status_t remove_port (sai_object_id_t port_id);
	static void 		config_port	(Port_obj port);
	//bridge functions
	static sai_status_t create_bridge (sai_object_id_t *bridge_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list);
	
	//api s
	sai_port_api_t port_api;
	sai_bridge_api_t bridge_api;

	sai_object():
	//  constructor pre initializations
	  socket(new TSocket("localhost", bm_port)),
	  transport(new TBufferedTransport(socket)),
	  bprotocol(new TBinaryProtocol(transport)),
	  protocol (new TMultiplexedProtocol(bprotocol, "standard")),
	  // sai_id_map_ptr(new sai_id_map_t()),
	  //sai_id_map(),
	  bm_client(protocol)
	  {
	  	printf("sai_object constructor started\n");
	  	//sai_id_map_t* 
	  	//sai_id_map_ptr = new sai_id_map_t;
	  	//sai_id_map = sai_id_map_t();
	  	Switch_metadata* switch_metadata_ptr = new Switch_metadata;
	  	//printf("sai_id_map addr:%d\n",&sai_id_map);

  		uint32_t list[]={0,1,2,3,4,5,6,7};
  		switch_metadata_ptr->hw_port_list.list=list;
  		switch_metadata_ptr->hw_port_list.count=8;
  		bm_client_ptr = &bm_client;
  		sai_id_map_ptr = &sai_id_map;
	  	transport->open();

	  	//api set
  		port_api.create_port  	= &sai_object::create_port;
  		port_api.remove_port	= &sai_object::remove_port;

  		bridge_api.create_bridge = &sai_object::create_bridge;
    	printf("BM connection started on port %d\n",bm_port); 
	  }
	~sai_object(){
	 	  //deconstructor
  		transport->close();
  		delete bm_client_ptr;
  		// delete sai_id_map;
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
         	default:
         		printf("api requested was %d, while sai_api_port is %d\n",sai_api_id,SAI_API_PORT);
         		//*api_method_table=&port_api; ///TODOOOOO remove!!
         		return SAI_STATUS_FAILURE; ///TODOOOOO enable!!
         	}
		return SAI_STATUS_SUCCESS;
	}
};

