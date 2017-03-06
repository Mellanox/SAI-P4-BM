
#include <iostream>
#include <vector>
/// thrift sai server
#include "../../Sai-Tests/sai_thrift_src/gen-cpp/switch_sai_rpc.h"
// #include "../src/sai_object.h"
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>
#include <string>

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
#include <stdlib.h>
#include <saistatus.h>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace  ::switch_sai;


// temp - TODO move to other file
extern "C"{
const char* test_profile_get_value(_In_ sai_switch_profile_id_t profile_id,_In_ const char* variable){return NULL;}
int test_profile_get_next_value(_In_ sai_switch_profile_id_t profile_id,_Out_ const char** variable,_Out_ const char** value){ return -1;}
const service_method_table_t test_services = {test_profile_get_value,test_profile_get_next_value};
sai_status_t sai_api_initialize(_In_ uint64_t flags,_In_ const service_method_table_t* services);
sai_status_t sai_api_query(_In_ sai_api_t sai_api_id,_Out_ void** api_method_table);
sai_status_t sai_api_uninitialize(void);

}
// globals 
const int sai_port = 9092;


class switch_sai_rpcHandler : virtual public switch_sai_rpcIf{

 public:
//  static sai_object sai_obj;

  ~switch_sai_rpcHandler() {
    // deconstructor
    sai_api_uninitialize();
  }
  switch_sai_rpcHandler(){
    // initialization   
    sai_api_initialize(0, &test_services);
  
  }
 
  sai_attribute_t parse_port_atrribute(const sai_thrift_attribute_t &thrift_attr) {
      sai_attribute_t sai_attr;
      sai_attr.id = thrift_attr.id;
      switch (thrift_attr.id) {
          case SAI_PORT_ATTR_UPDATE_DSCP:
              sai_attr.value.booldata = thrift_attr.value.booldata;
              break;
          case SAI_PORT_ATTR_PORT_VLAN_ID:
              sai_attr.value.u16 = thrift_attr.value.u16;
              break;
          case SAI_PORT_ATTR_BIND_MODE:
              sai_attr.value.s32 = thrift_attr.value.s32;
              break;
          case SAI_PORT_ATTR_DROP_UNTAGGED:
              sai_attr.value.booldata = thrift_attr.value.booldata;
              break;
          case SAI_PORT_ATTR_DROP_TAGGED:
              sai_attr.value.booldata = thrift_attr.value.booldata;
              break;
      };
      return sai_attr;
  }

  sai_thrift_status_t sai_thrift_set_port_attribute(const sai_thrift_object_id_t port_id, const sai_thrift_attribute_t& thrift_attr) {
    printf("sai_thrift_set_port_attribute\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_port_api_t *port_api;
    sai_attribute_t attr = parse_port_atrribute(thrift_attr);
    status = sai_api_query(SAI_API_PORT, (void **) &port_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    status = port_api->set_port_attribute(port_id,&attr);
    return SAI_STATUS_SUCCESS;
  }

  void sai_thrift_get_port_attribute(sai_thrift_attribute_list_t& _return, const sai_thrift_object_id_t port_id) {
    // Your implementation goes here
    printf("sai_thrift_get_port_attribute\n");
  }

  void sai_thrift_get_port_stats(std::vector<int64_t> & _return, const sai_thrift_object_id_t port_id, const std::vector<sai_thrift_port_stat_counter_t> & counter_ids, const int32_t number_of_counters) {
    // Your implementation goes here
    printf("sai_thrift_get_port_stats\n");
  }

  sai_thrift_status_t sai_thrift_clear_port_all_stats(const sai_thrift_object_id_t port_id) {
    // Your implementation goes here
    printf("sai_thrift_clear_port_all_stats\n");
  }

  sai_thrift_status_t sai_thrift_remove_port(const sai_thrift_object_id_t port_id) {
    printf("sai_thrift_remove_port\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_port_api_t *port_api;
    sai_attribute_t attr;
    status = sai_api_query(SAI_API_PORT, (void **) &port_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return status;
    }
    status = port_api->remove_port(port_id);
    return status;
  }

  sai_thrift_object_id_t sai_thrift_create_port(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
      printf("sai_thrift_create_port\n");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_port_api_t *port_api;
      sai_attribute_t *attr = (sai_attribute_t*) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
      status = sai_api_query(SAI_API_PORT, (void **) &port_api);
      if (status != SAI_STATUS_SUCCESS) {
          printf("sai_api_query failed!!!\n");
          return SAI_STATUS_NOT_IMPLEMENTED; 
      }
      uint32_t *lane_list_ptr = sai_thrift_parse_port_attributes(thrift_attr_list, attr );
      //switch_metatdata.switch_id.sai_object_id
      sai_object_id_t s_id=0;
      uint32_t count = thrift_attr_list.size();
      sai_object_id_t port_id =1;
      status = port_api->create_port(&port_id,s_id,count,attr);
      free(lane_list_ptr);
      free(attr);
      return port_id;
  }
  uint32_t* sai_thrift_parse_port_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      uint32_t *lane_list_ptr;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_PORT_ATTR_UPDATE_DSCP:
                  attr_list[i].value.booldata = attribute.value.booldata;
                  break;
              case SAI_PORT_ATTR_PORT_VLAN_ID:
                  attr_list[i].value.u16 = attribute.value.u16;
                  break;
              case SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL:
              case SAI_PORT_ATTR_QOS_DEFAULT_TC:
                  attr_list[i].value.u8 = attribute.value.u8;
                  break;
              case SAI_PORT_ATTR_QOS_INGRESS_BUFFER_PROFILE_LIST:
              case SAI_PORT_ATTR_QOS_EGRESS_BUFFER_PROFILE_LIST:
              case SAI_PORT_ATTR_BIND_MODE:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
              case SAI_PORT_ATTR_HW_LANE_LIST: {
                attr_list[i].value.u32list.count = attribute.value.u32list.count;
                attr_list[i].value.u32list.list = (uint32_t*) malloc(sizeof(uint32_t) * attribute.value.u32list.count);
                int j;
                for (j=0;j<=attribute.value.u32list.count;j++) {
                  attr_list[i].value.u32list.list[j] = attribute.value.u32list.u32list[j];
                }
                lane_list_ptr = attr_list[i].value.u32list.list;
                break;
              }
              default:
                  break;
          }
      }
      return lane_list_ptr;
  }
  void sai_thrift_parse_bridge_port_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      int l=thrift_attr_list.size();
      for(uint32_t i = 0; i < l; i++, it++) {
          //printf("thrift_attr_list_size_before switch=%d\n",thrift_attr_list.size());
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          //printf("attr id %d\n",attribute.id);
          switch (attribute.id) {
            case SAI_BRIDGE_PORT_ATTR_VLAN_ID:
          //  printf("br_vlan_id=%d,i=%d\n",attribute.value.s32,i);
              attr_list[i].value.u16 = attribute.value.u16;
              break;
            case SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
          //    printf("br_port_br_id=%d, i=%d\n",attribute.value.s32,i);
              attr_list[i].value.oid = attribute.value.oid;
              printf("sai_thrift_parse_bridge_port_attributes. bridge_id = %d\n", attr_list[i].value.oid);
              break; 
            case SAI_BRIDGE_PORT_ATTR_TYPE:
          //    printf("br_port_type=%d,i=%d\n",attribute.value.s32,i);
              attr_list[i].value.s32 = attribute.value.s32;
              break; 
            case SAI_BRIDGE_PORT_ATTR_PORT_ID:
          //    printf("br_port_p_id=%d,i=%d\n",attribute.value.s32,i);
              attr_list[i].value.oid = attribute.value.oid;
              break;
            default:
              break; 
          }
          //printf("thrift_attr_list_size aft switch=%d\n",thrift_attr_list.size());
      }
    //  printf("br_port thrift parsing done\n");
  }

  void sai_thrift_parse_bridge_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_BRIDGE_ATTR_TYPE:
                attr_list[i].value.s32 = attribute.value.s32;
                break;
              default:
                std::cout << "--> while parsing bridge_attr: attribute.id = " << attribute.id << " was dumped in sai_cpp_server" << endl; 
                break;
          }
      }
  }
  void sai_thrift_parse_fdb_entry_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
        attribute = (sai_thrift_attribute_t)*it;
        attr_list[i].id = attribute.id;
        switch (attribute.id) {
            case SAI_FDB_ENTRY_ATTR_TYPE:
                attr_list[i].value.s32 = attribute.value.s32;
                //td::cout << "--> attr packet type="<<attribute.value.s32<<endl;
                //std::cout << "--> attr packet_static" << SAI_FDB_ENTRY_TYPE_STATIC <<endl;
                break;
            case SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID:
                attr_list[i].value.oid = attribute.value.oid;
                break;
            case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
                attr_list[i].value.s32 = attribute.value.s32;
                //std::cout << "--> attr packet_action="<<attribute.value.s32<<endl;
                //std::cout << "--> attr packet_action_fwd=" << SAI_PACKET_ACTION_FORWARD <<endl;
                break;
            default:
                std::cout << "--> while parsing fdb_attr: attribute.id = " << attribute.id << " was dumped in sai_cpp_server" << endl; 
                break;
        }
    }
}


  sai_thrift_object_id_t sai_thrift_create_bridge(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    printf("sai_thrift_create_bridge\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *bridge_api;
    sai_attribute_t *attr = (sai_attribute_t*) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    sai_thrift_parse_bridge_attributes(thrift_attr_list, attr );
    sai_object_id_t s_id=0;
    uint32_t count = thrift_attr_list.size();
    sai_object_id_t bridge_id =1;
    status = bridge_api->create_bridge(&bridge_id,s_id,count,attr);
    free(attr);
    return bridge_id;
  }

  sai_thrift_status_t sai_thrift_remove_bridge(const sai_thrift_object_id_t bridge_id) {
    printf("sai_thrift_remove_bridge\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *bridge_api;
    status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    status = bridge_api->remove_bridge(bridge_id);
    return status;
  }

  sai_thrift_object_id_t sai_thrift_create_bridge_port(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    printf("sai_thrift_create_bridge_port\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *bridge_api;
    sai_attribute_t *attr= (sai_attribute_t*) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        //return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    uint32_t count = thrift_attr_list.size();
    sai_thrift_parse_bridge_port_attributes(thrift_attr_list, attr );
    sai_object_id_t s_id=0;
    sai_object_id_t bridge_port_id =1;
    bridge_api->create_bridge_port(&bridge_port_id,s_id,count,attr);
    printf("%d %d\n",bridge_port_id,(sai_thrift_object_id_t)bridge_port_id);
    free(attr);
    return (sai_thrift_object_id_t)bridge_port_id;
  }

  sai_thrift_status_t sai_thrift_remove_bridge_port(const sai_thrift_object_id_t bridge_port_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_bridge_port\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *bridge_api;
    status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    return bridge_api->remove_bridge_port(bridge_port_id);
  }

  void sai_thirft_get_bridge_attribute(sai_thrift_attribute_list_t& _return, const sai_thrift_object_id_t bridge_id, const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thirft_get_bridge_attribute\n");
  }


sai_fdb_entry_t  parse_thrift_fdb_entry(const sai_thrift_fdb_entry_t thrift_fdb_entry){
  sai_fdb_entry_t sai_fdb_entry;
  sai_fdb_entry.switch_id=0;
  printf("mac string size:%d\n",thrift_fdb_entry.mac_address.length());
  std::cout << "mac_addr thrift: " << thrift_fdb_entry.mac_address << endl;
  parse_mac_str(thrift_fdb_entry.mac_address, sai_fdb_entry.mac_address);  
  std::cout << "mac addr parsed: " << endl;
  for (int i=0; i<6; i++) {
    printf("mac[%d] = %d |",i, sai_fdb_entry.mac_address[i]);
  }
  std::cout << endl;
  sai_fdb_entry.vlan_id       =thrift_fdb_entry.vlan_id;
  sai_fdb_entry.bridge_type   =(sai_fdb_entry_bridge_type_t)thrift_fdb_entry.bridge_type;
  sai_fdb_entry.bridge_id     =thrift_fdb_entry.bridge_id;
  return sai_fdb_entry;
}

  sai_thrift_status_t sai_thrift_create_fdb_entry(const sai_thrift_fdb_entry_t& thrift_fdb_entry, const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    printf("sai_thrift_create_fdb_entry\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_fdb_api_t *fdb_api;
    sai_attribute_t *attr= (sai_attribute_t*) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    status = sai_api_query(SAI_API_FDB, (void **) &fdb_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    sai_thrift_parse_fdb_entry_attributes(thrift_attr_list, attr );
    uint32_t count = thrift_attr_list.size();
    std::cout << "--> create fdb attr count = "<< count << endl;
    sai_fdb_entry_t sai_fdb_entry;
    sai_fdb_entry = parse_thrift_fdb_entry(thrift_fdb_entry);
    return fdb_api->create_fdb_entry(&sai_fdb_entry,count,attr);
  }

  sai_thrift_status_t sai_thrift_delete_fdb_entry(const sai_thrift_fdb_entry_t& thrift_fdb_entry) {
    printf("sai_thrift_delete_fdb_entry\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_fdb_api_t *fdb_api;
    status = sai_api_query(SAI_API_FDB, (void **) &fdb_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    sai_fdb_entry_t sai_fdb_entry;
    sai_fdb_entry = parse_thrift_fdb_entry(thrift_fdb_entry);
    return fdb_api->remove_fdb_entry(&sai_fdb_entry);
  }

  sai_thrift_status_t sai_thrift_flush_fdb_entries(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_flush_fdb_entries\n");
  }



  void sai_thrift_parse_vlan_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_VLAN_ATTR_VLAN_ID:
                attr_list[i].value.u16 = attribute.value.u16;
                break;
              default:
                std::cout << "--> while parsing vlan_attr: attribute.id = " << attribute.id << " was dumped in sai_cpp_server" << endl; 
                break;
          }
      }
  }
  sai_thrift_object_id_t sai_thrift_create_vlan(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    printf("sai_thrift_create_vlan\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_vlan_api_t *vlan_api;
    sai_attribute_t *attr= (sai_attribute_t*) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    status = sai_api_query(SAI_API_VLAN, (void **) &vlan_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        //return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    sai_thrift_parse_vlan_attributes(thrift_attr_list, attr );
    uint32_t count = thrift_attr_list.size();
    std::cout << "--> create vlan attr count = "<< count << endl;
    sai_object_id_t s_id=0;
    sai_object_id_t vlan_id =1;
    bridge_api->create_vlan(&vlan_id,s_id,count,attr);
    free(attr);
    return (sai_thrift_object_id_t)vlan_id;
  }

  sai_thrift_status_t sai_thrift_delete_vlan(const sai_thrift_object_id_t vlan_id) {
    printf("sai_thrift_delete_vlan\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *vlan_api;
    status = sai_api_query(SAI_API_VLAN, (void **) &vlan_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    status = bridge_api->remove_vlan(vlan_id);
    return status;
  }

  void sai_thrift_get_vlan_stats(std::vector<int64_t> & _return, const sai_thrift_vlan_id_t vlan_id, const std::vector<sai_thrift_vlan_stat_counter_t> & counter_ids, const int32_t number_of_counters) {
    // Your implementation goes here
    printf("sai_thrift_get_vlan_stats\n");
  }

  void sai_thrift_parse_vlan_member_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_VLAN_MEMBER_ATTR_VLAN_ID:
                attr_list[i].value.oid = attribute.value.oid;
                break;
              case SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID:
                attr_list[i].value.oid = attribute.value.oid;
                break;
              case SAI_VLAN_MEMBER_ATTR_VLAN_TAGGING_MODE:
                attr_list[i].value.s32 = attribute.value.s32;
                break; 
              default:
                std::cout << "--> while parsing vlan_member_attr: attribute.id = " << attribute.id << " was dumped in sai_cpp_server" << endl; 
                break;
          }
      }
  }
  sai_thrift_object_id_t sai_thrift_create_vlan_member(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    printf("sai_thrift_create_vlan_member\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_vlan_api_t *vlan_api;
    sai_attribute_t *attr= (sai_attribute_t*) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    status = sai_api_query(SAI_API_VLAN, (void **) &vlan_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        //return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    sai_thrift_parse_vlan_member_attributes(thrift_attr_list, attr );
    uint32_t count = thrift_attr_list.size();
    std::cout << "--> create vlan_member attr count = "<< count << endl;
    sai_object_id_t s_id=0;
    sai_object_id_t vlan_member_id =1;
    bridge_api->create_create_vlan_member(&vlan_member_id,s_id,count,attr);
    free(attr);
    return (sai_thrift_object_id_t)vlan_member_id;
  }

  sai_thrift_status_t sai_thrift_remove_vlan_member(const sai_thrift_object_id_t vlan_member_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_vlan_member\n");
  }

  void sai_thrift_get_vlan_attribute(sai_thrift_attribute_list_t& _return, const sai_thrift_object_id_t vlan_id) {
    // Your implementation goes here
    printf("sai_thrift_get_vlan_attribute\n");
  }

  sai_thrift_object_id_t sai_thrift_create_virtual_router(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_virtual_router\n");
  }

  sai_thrift_status_t sai_thrift_remove_virtual_router(const sai_thrift_object_id_t vr_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_virtual_router\n");
  }

  sai_thrift_status_t sai_thrift_create_route(const sai_thrift_unicast_route_entry_t& thrift_unicast_route_entry, const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_route\n");
  }

  sai_thrift_status_t sai_thrift_remove_route(const sai_thrift_unicast_route_entry_t& thrift_unicast_route_entry) {
    // Your implementation goes here
    printf("sai_thrift_remove_route\n");
  }

  sai_thrift_object_id_t sai_thrift_create_router_interface(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_router_interface\n");
  }

  sai_thrift_status_t sai_thrift_remove_router_interface(const sai_thrift_object_id_t rif_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_router_interface\n");
  }

  sai_thrift_object_id_t sai_thrift_create_next_hop(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_next_hop\n");
  }

  sai_thrift_status_t sai_thrift_remove_next_hop(const sai_thrift_object_id_t next_hop_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_next_hop\n");
  }

  sai_thrift_object_id_t sai_thrift_create_next_hop_group(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_next_hop_group\n");
  }

  sai_thrift_status_t sai_thrift_remove_next_hop_group(const sai_thrift_object_id_t next_hop_group_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_next_hop_group\n");
  }

  sai_thrift_status_t sai_thrift_add_next_hop_to_group(const sai_thrift_object_id_t next_hop_group_id, const std::vector<sai_thrift_object_id_t> & thrift_nexthops) {
    // Your implementation goes here
    printf("sai_thrift_add_next_hop_to_group\n");
  }

  sai_thrift_status_t sai_thrift_remove_next_hop_from_group(const sai_thrift_object_id_t next_hop_group_id, const std::vector<sai_thrift_object_id_t> & thrift_nexthops) {
    // Your implementation goes here
    printf("sai_thrift_remove_next_hop_from_group\n");
  }

  sai_thrift_object_id_t sai_thrift_create_lag(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_lag\n");
  }

  sai_thrift_status_t sai_thrift_remove_lag(const sai_thrift_object_id_t lag_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_lag\n");
  }

  sai_thrift_object_id_t sai_thrift_create_lag_member(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_lag_member\n");
  }

  sai_thrift_status_t sai_thrift_remove_lag_member(const sai_thrift_object_id_t lag_member_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_lag_member\n");
  }

  sai_thrift_object_id_t sai_thrift_create_stp_entry(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_stp_entry\n");
  }

  sai_thrift_status_t sai_thrift_remove_stp_entry(const sai_thrift_object_id_t stp_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_stp_entry\n");
  }

  sai_thrift_status_t sai_thrift_set_stp_port_state(const sai_thrift_object_id_t stp_id, const sai_thrift_object_id_t port_id, const sai_thrift_port_stp_port_state_t stp_port_state) {
    // Your implementation goes here
    printf("sai_thrift_set_stp_port_state\n");
  }

  sai_thrift_port_stp_port_state_t sai_thrift_get_stp_port_state(const sai_thrift_object_id_t stp_id, const sai_thrift_object_id_t port_id) {
    // Your implementation goes here
    printf("sai_thrift_get_stp_port_state\n");
  }

  sai_thrift_status_t sai_thrift_create_neighbor_entry(const sai_thrift_neighbor_entry_t& thrift_neighbor_entry, const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_neighbor_entry\n");
  }

  sai_thrift_status_t sai_thrift_remove_neighbor_entry(const sai_thrift_neighbor_entry_t& thrift_neighbor_entry) {
    // Your implementation goes here
    printf("sai_thrift_remove_neighbor_entry\n");
  }

  sai_thrift_object_id_t sai_thrift_create_switch(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_port\n");
    //TODO: Currently not caring about attr_list.
    // sai_attribute_t *attr = (sai_attribute_t*) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_switch_api_t *switch_api;
    status = sai_api_query(SAI_API_SWITCH, (void **) &switch_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return SAI_STATUS_NOT_IMPLEMENTED; 
    }
    printf("sai_thrift_create_switch\n");
    // uint32_t count = thrift_attr_list.size();
    uint32_t count =0;
    sai_object_id_t s_id =1;
    status = switch_api->create_switch(&s_id,count,NULL);
    return s_id;
  }

  void sai_thrift_get_switch_attribute(sai_thrift_attribute_list_t& _return, const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    // for attr in thrift_attr_list:
    //   if attr.id == SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID:
    //     attr.value.oid = self.bridges[0].sai_object_id
    // for attr in thrift_attr_list:
    //   if attr.id == SAI_SWITCH_ATTR_PORT_LIST:
    //     attr.value.objlist = sai_thrift_object_list_t(count=len(self.ports.keys()), object_id_list=self.ports.keys())

    printf("sai_thrift_get_switch_attribute\n");
  }

  void sai_thrift_get_port_list_by_front_port(sai_thrift_attribute_t& _return) {
    // Your implementation goes here
    printf("sai_thrift_get_port_list_by_front_port\n");
  }

  sai_thrift_object_id_t sai_thrift_get_cpu_port_id() {
    // Your implementation goes here
    printf("sai_thrift_get_cpu_port_id\n");
  }

  sai_thrift_object_id_t sai_thrift_get_default_trap_group() {
    // Your implementation goes here
    printf("sai_thrift_get_default_trap_group\n");
  }

  sai_thrift_object_id_t sai_thrift_get_default_router_id() {
    // Your implementation goes here
    printf("sai_thrift_get_default_router_id\n");
  }

  sai_thrift_object_id_t sai_thrift_get_port_id_by_front_port(const std::string& port_name) {
    // Your implementation goes here
    printf("sai_thrift_get_port_id_by_front_port\n");
  }

  sai_thrift_status_t sai_thrift_set_switch_attribute(const sai_thrift_attribute_t& attribute) {
    // Your implementation goes here
    printf("sai_thrift_set_switch_attribute\n");
  }

  sai_thrift_object_id_t sai_thrift_create_hostif(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_hostif\n");
  }

  sai_thrift_status_t sai_thrift_remove_hostif(const sai_thrift_object_id_t hif_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_hostif\n");
  }

  sai_thrift_object_id_t sai_thrift_create_hostif_trap_group(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_hostif_trap_group\n");
  }

  sai_thrift_status_t sai_thrift_remove_hostif_trap_group(const sai_thrift_object_id_t trap_group_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_hostif_trap_group\n");
  }

  sai_thrift_status_t sai_thrift_create_hostif_trap(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_hostif_trap\n");
  }

  sai_thrift_status_t sai_thrift_remove_hostif_trap(const sai_thrift_hostif_trap_id_t trap_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_hostif_trap\n");
  }

  sai_thrift_status_t sai_thrift_set_hostif_trap(const sai_thrift_hostif_trap_id_t trap_id, const sai_thrift_attribute_t& thrift_attr) {
    // Your implementation goes here
    printf("sai_thrift_set_hostif_trap\n");
  }

  sai_thrift_status_t sai_thrift_set_hostif_trap_group(const sai_thrift_object_id_t trap_group_id, const sai_thrift_attribute_t& thrift_attr) {
    // Your implementation goes here
    printf("sai_thrift_set_hostif_trap_group\n");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_table(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_acl_table\n");
  }

  sai_thrift_status_t sai_thrift_delete_acl_table(const sai_thrift_object_id_t acl_table_id) {
    // Your implementation goes here
    printf("sai_thrift_delete_acl_table\n");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_entry(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_acl_entry\n");
  }

  sai_thrift_status_t sai_thrift_delete_acl_entry(const sai_thrift_object_id_t acl_entry) {
    // Your implementation goes here
    printf("sai_thrift_delete_acl_entry\n");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_counter(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_acl_counter\n");
  }

  sai_thrift_status_t sai_thrift_delete_acl_counter(const sai_thrift_object_id_t acl_counter_id) {
    // Your implementation goes here
    printf("sai_thrift_delete_acl_counter\n");
  }

  void sai_thrift_get_acl_counter_attribute(std::vector<sai_thrift_attribute_value_t> & _return, const sai_thrift_object_id_t acl_counter_id, const std::vector<int32_t> & thrift_attr_ids) {
    // Your implementation goes here
    printf("sai_thrift_get_acl_counter_attribute\n");
  }

  sai_thrift_object_id_t sai_thrift_create_mirror_session(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_mirror_session\n");
  }

  sai_thrift_status_t sai_thrift_remove_mirror_session(const sai_thrift_object_id_t session_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_mirror_session\n");
  }

  sai_thrift_object_id_t sai_thrift_create_policer(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_policer\n");
  }

  sai_thrift_status_t sai_thrift_remove_policer(const sai_thrift_object_id_t policer_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_policer\n");
  }

  void sai_thrift_get_policer_stats(std::vector<int64_t> & _return, const sai_thrift_object_id_t policer_id, const std::vector<sai_thrift_policer_stat_counter_t> & counter_ids) {
    // Your implementation goes here
    printf("sai_thrift_get_policer_stats\n");
  }

  sai_thrift_object_id_t sai_thrift_create_scheduler_profile(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_scheduler_profile\n");
  }

  sai_thrift_status_t sai_thrift_remove_scheduler_profile(const sai_thrift_object_id_t scheduler_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_scheduler_profile\n");
  }

  void sai_thrift_get_queue_stats(std::vector<int64_t> & _return, const sai_thrift_object_id_t queue_id, const std::vector<sai_thrift_queue_stat_counter_t> & counter_ids, const int32_t number_of_counters) {
    // Your implementation goes here
    printf("sai_thrift_get_queue_stats\n");
  }

  sai_thrift_status_t sai_thrift_clear_queue_stats(const sai_thrift_object_id_t queue_id, const std::vector<sai_thrift_queue_stat_counter_t> & counter_ids, const int32_t number_of_counters) {
    // Your implementation goes here
    printf("sai_thrift_clear_queue_stats\n");
  }

  sai_thrift_status_t sai_thrift_set_queue_attribute(const sai_thrift_object_id_t queue_id, const sai_thrift_attribute_t& thrift_attr) {
    // Your implementation goes here
    printf("sai_thrift_set_queue_attribute\n");
  }

  sai_thrift_object_id_t sai_thrift_create_buffer_profile(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_buffer_profile\n");
  }

  sai_thrift_object_id_t sai_thrift_create_pool_profile(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_pool_profile\n");
  }

  sai_thrift_status_t sai_thrift_set_priority_group_attribute(const sai_thrift_object_id_t pg_id, const sai_thrift_attribute_t& thrift_attr) {
    // Your implementation goes here
    printf("sai_thrift_set_priority_group_attribute\n");
  }

  void sai_thrift_get_pg_stats(std::vector<int64_t> & _return, const sai_thrift_object_id_t pg_id, const std::vector<sai_thrift_pg_stat_counter_t> & counter_ids, const int32_t number_of_counters) {
    // Your implementation goes here
    printf("sai_thrift_get_pg_stats\n");
  }

  sai_thrift_object_id_t sai_thrift_create_wred_profile(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_wred_profile\n");
  }

  sai_thrift_status_t sai_thrift_remove_wred_profile(const sai_thrift_object_id_t wred_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_wred_profile\n");
  }

  sai_thrift_object_id_t sai_thrift_create_qos_map(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    printf("sai_thrift_create_qos_map\n");
  }

  sai_thrift_status_t sai_thrift_remove_qos_map(const sai_thrift_object_id_t qos_map_id) {
    // Your implementation goes here
    printf("sai_thrift_remove_qos_map\n");
  }

  // GENERAL
  void parse_mac_str(const std::string& mac_str, uint8_t mac[6])
  {
    int l=mac_str.length();
    int j=0;
    for (int i=0; i<l ; i+=3){
      mac[j] = (uint8_t) std::stoi(mac_str.substr(i,2),NULL,16);j++;
    }
  }
};


int main(int argc, char **argv) {
  // open server to sai functions
  printf("creating server for SAI on port %d \n", sai_port);
  boost::shared_ptr<switch_sai_rpcHandler> handler(new switch_sai_rpcHandler());
  boost::shared_ptr<TProcessor> processor(new switch_sai_rpcProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(sai_port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  printf("sai server started \n");
  server.serve();
  printf("thrift done\n");
  return 0;
}

