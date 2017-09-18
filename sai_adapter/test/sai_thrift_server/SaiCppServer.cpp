#include <iostream>
#include <vector>
/// thrift sai server
#include <spdlog/spdlog.h>
#include <string>
#include <switch_sai_rpc.h>
#include <arpa/inet.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>

// SAI
#ifdef __cplusplus
extern "C" {
#endif
#include <sai.h>
#ifdef __cplusplus
}
#endif

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace ::switch_sai;

// temp - TODO move to other file
extern "C" {
const char *test_profile_get_value(_In_ sai_switch_profile_id_t profile_id,
                                   _In_ const char *variable) {
  return NULL;
}
int test_profile_get_next_value(_In_ sai_switch_profile_id_t profile_id,
                                _Out_ const char **variable,
                                _Out_ const char **value) {
  return -1;
}
const service_method_table_t test_services = {test_profile_get_value,
                                              test_profile_get_next_value};
sai_status_t sai_api_initialize(_In_ uint64_t flags,
                                _In_ const service_method_table_t *services);
sai_status_t sai_api_query(_In_ sai_api_t sai_api_id,
                           _Out_ void **api_method_table);
sai_status_t sai_api_uninitialize(void);
}
// globals
const int sai_port = 9092;

class switch_sai_rpcHandler : virtual public switch_sai_rpcIf {
public:
  //  static sai_object sai_obj;
  std::shared_ptr<spdlog::logger> logger;
  ~switch_sai_rpcHandler() {
    // deconstructor
    logger->info("switch_sai_rpcHandler destructor called");
    spdlog::drop_all();
    sai_api_uninitialize();
  }
  switch_sai_rpcHandler() {
    // initialization
    logger = spdlog::get("logger");
    sai_api_initialize(0, &test_services);
    sai_switch_api_t *switch_api;
    if (sai_api_query(SAI_API_SWITCH, (void **) &switch_api) != SAI_STATUS_SUCCESS) {
      logger->error("Failed to query sai_switch_api");
      exit(1);
    }

    sai_object_id_t switch_id;
    sai_attribute_t switch_attr;
    switch_attr.id = SAI_SWITCH_ATTR_INIT_SWITCH;
    switch_attr.value.booldata = true;
    logger->info("Switch init with default configurations");
    switch_api->create_switch(&switch_id, 1, &switch_attr);
    logger->info("Switch init with default configurations done");
    // server_internal_init_switch();
  }

  sai_attribute_t
  parse_port_atrribute(const sai_thrift_attribute_t &thrift_attr) {
    sai_attribute_t sai_attr;
    sai_attr.id = thrift_attr.id;
    switch (thrift_attr.id) {
    case SAI_PORT_ATTR_PORT_VLAN_ID:
      sai_attr.value.u16 = thrift_attr.value.u16;
      break;
    case SAI_PORT_ATTR_BIND_MODE:
    case SAI_PORT_OPER_STATUS_UP:
      sai_attr.value.s32 = thrift_attr.value.s32;
      break;
    case SAI_PORT_ATTR_UPDATE_DSCP:
    case SAI_PORT_ATTR_DROP_UNTAGGED:
    case SAI_PORT_ATTR_DROP_TAGGED:
    case SAI_PORT_ATTR_ADMIN_STATE:
      sai_attr.value.booldata = thrift_attr.value.booldata;
      break;
    };
    return sai_attr;
  }

  sai_thrift_status_t
  sai_thrift_set_port_attribute(const sai_thrift_object_id_t port_id,
                                const sai_thrift_attribute_t &thrift_attr) {
    logger->info("sai_thrift_set_port_attribute");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_port_api_t *port_api;
    sai_attribute_t attr = parse_port_atrribute(thrift_attr);
    status = sai_api_query(SAI_API_PORT, (void **)&port_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    status = port_api->set_port_attribute(port_id, &attr);
    return SAI_STATUS_SUCCESS;
  }

void sai_thrift_get_port_attribute(sai_thrift_attribute_list_t& thrift_attr_list, const sai_thrift_object_id_t port_id) {
    // Your implementation goes here
    logger->info("sai_thrift_get_port_attribute");
    sai_status_t status = SAI_STATUS_SUCCESS;
      sai_port_api_t *port_api;
      sai_attribute_t max_queue_attribute;
      sai_attribute_t queue_list_object_attribute;
      sai_thrift_attribute_t thrift_queue_list_attribute;
      sai_object_list_t *queue_list_object;
      int max_queues = 0;
      status = sai_api_query(SAI_API_PORT, (void **) &port_api);
      if (status != SAI_STATUS_SUCCESS) {
          return;
      }

      std::vector<sai_thrift_attribute_t>& attr_list = thrift_attr_list.attr_list;
      // TODO = currently no support in PG
      
      // max_queue_attribute.id = SAI_PORT_ATTR_QOS_NUMBER_OF_QUEUES;
      // port_api->get_port_attribute(port_id, 1, &max_queue_attribute);
      // max_queues = max_queue_attribute.value.u32;
      // queue_list_object_attribute.id = SAI_PORT_ATTR_QOS_QUEUE_LIST;
      // queue_list_object_attribute.value.objlist.list = (sai_object_id_t *) malloc(sizeof(sai_object_id_t) * max_queues);
      // queue_list_object_attribute.value.objlist.count = max_queues;
      // port_api->get_port_attribute(port_id, 1, &queue_list_object_attribute);

      // thrift_attr_list.attr_count = 2;
      // thrift_queue_list_attribute.id = SAI_PORT_ATTR_QOS_QUEUE_LIST;
      // thrift_queue_list_attribute.value.objlist.count = max_queues;
      // std::vector<sai_thrift_object_id_t>& queue_list = thrift_queue_list_attribute.value.objlist.object_id_list;
      // queue_list_object = &queue_list_object_attribute.value.objlist;
      // for (int index = 0; index < max_queues; index++) {
      //     queue_list.push_back((sai_thrift_object_id_t) queue_list_object->list[index]);
      // }
      // attr_list.push_back(thrift_queue_list_attribute);
      // free(queue_list_object_attribute.value.objlist.list);

      // sai_attribute_t max_pg_attribute;
      // sai_attribute_t pg_list_object_attribute;
      // sai_thrift_attribute_t thrift_pg_list_attribute;
      // sai_object_list_t *pg_list_object;
      // int max_pg = 0;

      // max_pg_attribute.id = SAI_PORT_ATTR_NUMBER_OF_INGRESS_PRIORITY_GROUPS;
      // port_api->get_port_attribute(port_id, 1, &max_pg_attribute);
      // max_pg = max_pg_attribute.value.u32;
      // pg_list_object_attribute.id = SAI_PORT_ATTR_INGRESS_PRIORITY_GROUP_LIST;
      // pg_list_object_attribute.value.objlist.list = (sai_object_id_t *) malloc(sizeof(sai_object_id_t) * max_pg);
      // pg_list_object_attribute.value.objlist.count = max_pg;
      // port_api->get_port_attribute(port_id, 1, &pg_list_object_attribute);

      // thrift_attr_list.attr_count = 3;
      // thrift_pg_list_attribute.id = SAI_PORT_ATTR_INGRESS_PRIORITY_GROUP_LIST;
      // thrift_pg_list_attribute.value.objlist.count = max_pg;
      // std::vector<sai_thrift_object_id_t>& pg_list = thrift_pg_list_attribute.value.objlist.object_id_list;
      // pg_list_object = &pg_list_object_attribute.value.objlist;
      // for (int index = 0; index < max_pg; index++) {
      //     pg_list.push_back((sai_thrift_object_id_t) pg_list_object->list[index]);
      // }
      // attr_list.push_back(thrift_pg_list_attribute);
      // free(pg_list_object_attribute.value.objlist.list);
 
      sai_attribute_t port_hw_lane;
      sai_thrift_attribute_t thrift_port_hw_lane;
      sai_u32_list_t *lane_list_num;

      port_hw_lane.id = SAI_PORT_ATTR_HW_LANE_LIST;
      port_hw_lane.value.u32list.list = (uint32_t *) malloc(sizeof(uint32_t) * 4);
      port_hw_lane.value.u32list.count = 4;
      port_api->get_port_attribute(port_id, 1, &port_hw_lane);

      thrift_attr_list.attr_count = 1;
      thrift_port_hw_lane.id = SAI_PORT_ATTR_HW_LANE_LIST;
      thrift_port_hw_lane.value.u32list.count = port_hw_lane.value.u32list.count;
      std::vector<int32_t>& lane_list = thrift_port_hw_lane.value.u32list.u32list;
      lane_list_num = &port_hw_lane.value.u32list;
      for (int index = 0; index < port_hw_lane.value.u32list.count ; index++) {
          lane_list.push_back((uint32_t) lane_list_num->list[index]);
      }
      attr_list.push_back(thrift_port_hw_lane);
      free(port_hw_lane.value.u32list.list); 

      sai_attribute_t port_oper_status_attribute;
      sai_thrift_attribute_t thrift_port_status;
      port_oper_status_attribute.id = SAI_PORT_ATTR_OPER_STATUS;
      port_api->get_port_attribute(port_id, 1, &port_oper_status_attribute);

      thrift_attr_list.attr_count = 2;
      thrift_port_status.id = SAI_PORT_ATTR_OPER_STATUS;
      thrift_port_status.value.s32 =  port_oper_status_attribute.value.s32;
      attr_list.push_back(thrift_port_status);
  }

  void sai_thrift_get_port_stats(
      std::vector<int64_t> &_return, const sai_thrift_object_id_t port_id,
      const std::vector<sai_thrift_port_stat_counter_t> &counter_ids,
      const int32_t number_of_counters) {
    // Your implementation goes here
    logger->info("sai_thrift_get_port_stats");
  }

  sai_thrift_status_t
  sai_thrift_clear_port_all_stats(const sai_thrift_object_id_t port_id) {
    // Your implementation goes here
    logger->info("sai_thrift_clear_port_all_stats");
  }

  sai_thrift_status_t
  sai_thrift_remove_port(const sai_thrift_object_id_t port_id) {
    logger->info("sai_thrift_remove_port");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_port_api_t *port_api;
    sai_attribute_t attr;
    status = sai_api_query(SAI_API_PORT, (void **)&port_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return status;
    }
    status = port_api->remove_port(port_id);
    return status;
  }

  sai_thrift_object_id_t sai_thrift_create_port(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_port");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_port_api_t *port_api;
    sai_attribute_t *attr = (sai_attribute_t *)malloc(sizeof(sai_attribute_t) *
                                                      thrift_attr_list.size());
    status = sai_api_query(SAI_API_PORT, (void **)&port_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    uint32_t *lane_list_ptr =
        sai_thrift_parse_port_attributes(thrift_attr_list, attr);
    // switch_metatdata.switch_id.sai_object_id
    sai_object_id_t s_id = 0;
    uint32_t count = thrift_attr_list.size();
    sai_object_id_t port_id = 1;
    status = port_api->create_port(&port_id, s_id, count, attr);
    free(lane_list_ptr);
    free(attr);
    return port_id;
  }

  uint32_t *sai_thrift_parse_port_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    uint32_t *lane_list_ptr;
    for (uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
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
        attr_list[i].value.u32list.list = (uint32_t *)malloc(
            sizeof(uint32_t) * attribute.value.u32list.count);
        int j;
        for (j = 0; j <= attribute.value.u32list.count; j++) {
          attr_list[i].value.u32list.list[j] =
              attribute.value.u32list.u32list[j];
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
  void sai_thrift_parse_bridge_port_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    int l = thrift_attr_list.size();
    for (uint32_t i = 0; i < l; i++, it++) {
      attribute = (sai_thrift_attribute_t)*it;
      attr_list[i].id = attribute.id;
      switch (attribute.id) {
      case SAI_BRIDGE_PORT_ATTR_VLAN_ID:
        attr_list[i].value.u16 = attribute.value.u16;
        break;
      case SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
        attr_list[i].value.oid = attribute.value.oid;
        break;
      case SAI_BRIDGE_PORT_ATTR_TYPE:
        attr_list[i].value.s32 = attribute.value.s32;
        break;
      case SAI_BRIDGE_PORT_ATTR_PORT_ID:
        attr_list[i].value.oid = attribute.value.oid;
        break;
      default:
        break;
      }
    }
  }

  sai_thrift_attribute_t
  parse_bridge_port_thrift_attribute(sai_attribute_t *sai_attr) {
    sai_thrift_attribute_t thrift_attr;
    thrift_attr.id = sai_attr->id;
    switch (sai_attr->id) {
    case SAI_BRIDGE_PORT_ATTR_PORT_ID:
      thrift_attr.value.oid = sai_attr->value.oid;
      break;
    case SAI_BRIDGE_PORT_ATTR_VLAN_ID:
      thrift_attr.value.u16 = sai_attr->value.u16;
      break;
    case SAI_BRIDGE_PORT_ATTR_TYPE:
      thrift_attr.value.s32 = sai_attr->value.s32;
      break;
    case SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
      thrift_attr.value.oid = sai_attr->value.oid;
      break;
    }
    return thrift_attr;
  }

  void sai_thrift_parse_bridge_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    for (uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
      attribute = (sai_thrift_attribute_t)*it;
      attr_list[i].id = attribute.id;
      switch (attribute.id) {
      case SAI_BRIDGE_ATTR_TYPE:
        attr_list[i].value.s32 = attribute.value.s32;
        break;
      case SAI_BRIDGE_ATTR_PORT_LIST:
        attr_list[i].value.objlist.count = 0;
        break;
      default:
        break;
      }
    }
    return;
  }

  void sai_thrift_parse_fdb_entry_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    for (uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
      attribute = (sai_thrift_attribute_t)*it;
      attr_list[i].id = attribute.id;
      switch (attribute.id) {
      case SAI_FDB_ENTRY_ATTR_TYPE:
        attr_list[i].value.s32 = attribute.value.s32;
        break;
      case SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID:
        attr_list[i].value.oid = attribute.value.oid;
        break;
      case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
        attr_list[i].value.s32 = attribute.value.s32;
        break;
      default:
        logger->error("--> while parsing fdb_attr: attribute.id = {} was "
                      "dumped in sai_cpp_server",
                      attribute.id);
        break;
      }
    }
  }

    sai_attribute_t *sai_thrift_attribute_list_to_sai(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_attribute_t *sai_attrs;

      sai_attrs = (sai_attribute_t *) calloc(thrift_attr_list.size(), sizeof(sai_attribute_t));
      if (!sai_attrs) {
          // SAI_THRIFT_LOG_ERR("failed to allocate sai attibutes list");
        logger->error("failed to allocate sai attibutes list");
          return NULL;
      }

      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          sai_thrift_attribute_t & thrift_attr = (sai_thrift_attribute_t &) *it;
          sai_attrs[i].id = thrift_attr.id;
          sai_attrs[i].value.oid = thrift_attr.value.oid;
      }

      return sai_attrs;
  }

  void sai_attributes_to_sai_thrift_list(sai_attribute_t *sai_attrs, uint32_t count, std::vector<sai_thrift_attribute_t> & thrift_attr_list) {

      for (uint32_t i = 0; i < count; i++) {
          sai_thrift_attribute_t thrift_attr;

          thrift_attr.id        = sai_attrs[i].id;
          thrift_attr.value.oid = sai_attrs[i].value.oid;

          thrift_attr_list.push_back(thrift_attr);
      }
  }

  sai_thrift_object_id_t sai_thrift_create_bridge(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_bridge");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *bridge_api;
    sai_attribute_t *attr = (sai_attribute_t *)malloc(sizeof(sai_attribute_t) *
                                                      thrift_attr_list.size());
    status = sai_api_query(SAI_API_BRIDGE, (void **)&bridge_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    sai_thrift_parse_bridge_attributes(thrift_attr_list, attr);
    sai_object_id_t s_id = 0;
    uint32_t count = thrift_attr_list.size();
    sai_object_id_t bridge_id = 1;
    status = bridge_api->create_bridge(&bridge_id, s_id, count, attr);
    free(attr);
    return bridge_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_bridge(const sai_thrift_object_id_t bridge_id) {
    logger->info("sai_thrift_remove_bridge");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *bridge_api;
    status = sai_api_query(SAI_API_BRIDGE, (void **)&bridge_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    status = bridge_api->remove_bridge(bridge_id);
    return status;
  }

  sai_thrift_attribute_t
  parse_bridge_thrift_attribute(sai_attribute_t *sai_attr) {
    sai_thrift_attribute_t thrift_attr;
    thrift_attr.id = sai_attr->id;
    switch (sai_attr->id) {
    case SAI_BRIDGE_ATTR_TYPE:
      thrift_attr.value.s32 = sai_attr->value.s32;
      break;
    case SAI_BRIDGE_ATTR_PORT_LIST:
      thrift_attr.value.objlist.count = sai_attr->value.objlist.count;
      for (int i = 0; i < thrift_attr.value.objlist.count; i++) {
        thrift_attr.value.objlist.object_id_list.push_back(
            sai_attr->value.objlist.list[i]);
      }
      break;
    }
    return thrift_attr;
  }

  void sai_thirft_get_bridge_attribute(
      sai_thrift_attribute_list_t &_return,
      const sai_thrift_object_id_t bridge_id,
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thirft_get_bridge_attribute");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_bridge_api_t *bridge_api;
    status = sai_api_query(SAI_API_BRIDGE, (void **)&bridge_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      // return SAI_STATUS_NOT_IMPLEMENTED;
    }

    uint32_t count = thrift_attr_list.size();
    sai_attribute_t *attr =
        (sai_attribute_t *)malloc(sizeof(sai_attribute_t) * count);
    sai_thrift_parse_bridge_attributes(thrift_attr_list, attr);
    status = bridge_api->get_bridge_attribute(bridge_id, count, attr);
    _return.attr_count = count;
    for (int i = 0; i < count; i++) {
      _return.attr_list.push_back(parse_bridge_thrift_attribute(attr + i));
    }
    free(attr);
    return;
  }

  void sai_thrift_create_bridge_port(sai_thrift_result_t &ret, const std::vector<sai_thrift_attribute_t> & thrift_attr_list)
  {
      sai_bridge_api_t *bridge_api;
      sai_attribute_t *sai_attrs;

      // SAI_THRIFT_FUNC_LOG();

      ret.status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
      if (ret.status != SAI_STATUS_SUCCESS) {
          // SAI_THRIFT_LOG_ERR("failed to obtain bridge_api, status:%d", ret.status);
        logger->error("failed to obtain bridge_api, status:{}", ret.status);
          return;
      }

      sai_attrs = sai_thrift_attribute_list_to_sai(thrift_attr_list);
      if (!sai_attrs) {
          ret.status = SAI_STATUS_NO_MEMORY;
          return;
      }

      ret.status = bridge_api->create_bridge_port((sai_object_id_t *) &ret.data.oid, 0, thrift_attr_list.size(), sai_attrs);
      if (ret.status != SAI_STATUS_SUCCESS) {
          // SAI_THRIFT_LOG_ERR("failed to create bridge port, status:%d", ret.status);
        logger->error("failed to create bridge port, status:{}", ret.status);
      }
      free(sai_attrs);
  }

  sai_thrift_status_t sai_thrift_remove_bridge_port(const sai_thrift_object_id_t bridge_port_id)
  {
      sai_bridge_api_t *bridge_api;
      sai_status_t status;

      // SAI_THRIFT_FUNC_LOG();

      status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
      if (status != SAI_STATUS_SUCCESS) {
          // SAI_THRIFT_LOG_ERR("failed to obtain bridge_api, status:%d", status);
        logger->error("failed to obtain bridge_api, status:{}", status);
          return status;
      }

      return bridge_api->remove_bridge_port((sai_object_id_t) bridge_port_id);
  }


  void sai_thrift_get_bridge_port_list(sai_thrift_result_t &ret, sai_thrift_object_id_t bridge_id)
  {
      logger->info("sai_thrift_get_bridge_port_list");
      std::vector<sai_thrift_object_id_t>& port_list = ret.data.objlist.object_id_list;
      sai_bridge_api_t *bridge_api;
      uint32_t max_ports = 128;
      sai_attribute_t attr;

      // SAI_THRIFT_FUNC_LOG();

      ret.status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
      if (ret.status != SAI_STATUS_SUCCESS) {
          logger->error("failed to obtain bridge_api, status:{}", ret.status);
          return;
      }

      attr.id = SAI_BRIDGE_ATTR_PORT_LIST;
      attr.value.objlist.list = (sai_object_id_t *) calloc(max_ports, sizeof(sai_object_id_t));
      attr.value.objlist.count = max_ports;

      ret.status = bridge_api->get_bridge_attribute(bridge_id, 1, &attr);
      if (ret.status != SAI_STATUS_SUCCESS && attr.value.objlist.count > max_ports) {
          /* retry one more time with a bigger list */
          max_ports = attr.value.objlist.count;
          attr.value.objlist.list = (sai_object_id_t *) realloc(attr.value.objlist.list, max_ports * sizeof(sai_object_id_t));

          ret.status = bridge_api->get_bridge_attribute(bridge_id, 1, &attr);
      }

      if (ret.status != SAI_STATUS_SUCCESS) {
          logger->error("Failed to obtain bridge ports list, status:{}", ret.status);
          free(attr.value.objlist.list);
          return;
      }

      for (int index = 0; index < attr.value.objlist.count; index++) {
          port_list.push_back((sai_thrift_object_id_t) attr.value.objlist.list[index]);
      }
      free(attr.value.objlist.list);
  }

   void sai_thrift_get_bridge_port_attribute(sai_thrift_attribute_list_t& thrift_attr_list, const sai_thrift_object_id_t bridge_port_id) {
      logger->info("sai_thrift_get_bridge_port_attribute");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_bridge_api_t *bridge_api;
      uint32_t attr_count = 0;
      sai_attribute_t attr[3];

      // SAI_THRIFT_FUNC_LOG();

      thrift_attr_list.attr_count = 0;

      status = sai_api_query(SAI_API_BRIDGE, (void **) &bridge_api);
      if (status != SAI_STATUS_SUCCESS) {
          // SAI_THRIFT_LOG_ERR("failed to obtain bridge_api, status:%d", status);
          logger->error("failed to obtain bridge_api, status:{}", status);
          return;
      }

      attr[0].id = SAI_BRIDGE_PORT_ATTR_TYPE;
      attr[1].id = SAI_BRIDGE_PORT_ATTR_BRIDGE_ID;
      attr_count = 2;

      status = bridge_api->get_bridge_port_attribute(bridge_port_id, attr_count, attr);
      if (status != SAI_STATUS_SUCCESS) {
          // SAI_THRIFT_LOG_ERR("failed to obtain bridge port type, status:%d", status);
          logger->error("failed to obtain bridge port type, status:{}", status);
          return;
      }

      sai_attributes_to_sai_thrift_list(attr, attr_count, thrift_attr_list.attr_list);

      switch (attr[0].value.s32) {
      case SAI_BRIDGE_PORT_TYPE_PORT:
          attr[0].id = SAI_BRIDGE_PORT_ATTR_FDB_LEARNING_MODE;
          attr[1].id = SAI_BRIDGE_PORT_ATTR_PORT_ID;
          attr_count = 2;
          break;

      case SAI_BRIDGE_PORT_TYPE_TUNNEL:
          attr[0].id = SAI_BRIDGE_PORT_ATTR_TUNNEL_ID;
          attr_count = 1;
          break;

      default:
          return;
      }

      status = bridge_api->get_bridge_port_attribute(bridge_port_id, attr_count, attr);
      if (status != SAI_STATUS_SUCCESS) {
          // SAI_THRIFT_LOG_ERR("failed to obtain bridge port attributes, status:%d", status);
        logger->error("failed to obtain bridge port attributes, status:{}", status);
          return;
      }

      sai_attributes_to_sai_thrift_list(attr, attr_count, thrift_attr_list.attr_list);
  }

  sai_thrift_status_t sai_thrift_set_bridge_port_attribute(
      const sai_thrift_object_id_t bridge_port_id,
      const sai_thrift_attribute_t &thrift_attr) {
    // Your implementation goes here
    logger->info("sai_thrift_set_bridge_port_attribute");
  }

  // GENERAL
  void parse_mac_str(const std::string &mac_str, uint8_t mac[6]) {
    int l = mac_str.length();
    int j = 5;
    for (int i = 0; i < l; i += 3) {
      mac[j] = (uint8_t)std::stoi(mac_str.substr(i, 2), NULL, 16);
      j--;
    }
  }

  sai_fdb_entry_t
  parse_thrift_fdb_entry(const sai_thrift_fdb_entry_t thrift_fdb_entry) {
    sai_fdb_entry_t sai_fdb_entry;
    sai_fdb_entry.switch_id = 0;
    parse_mac_str(thrift_fdb_entry.mac_address, sai_fdb_entry.mac_address);
    sai_fdb_entry.vlan_id = thrift_fdb_entry.vlan_id;
    sai_fdb_entry.bridge_type =
        (sai_fdb_entry_bridge_type_t)thrift_fdb_entry.bridge_type;
    sai_fdb_entry.bridge_id = thrift_fdb_entry.bridge_id;
    return sai_fdb_entry;
  }

  sai_thrift_status_t sai_thrift_create_fdb_entry(
      const sai_thrift_fdb_entry_t &thrift_fdb_entry,
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_fdb_entry");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_fdb_api_t *fdb_api;
    sai_attribute_t *attr = (sai_attribute_t *)malloc(sizeof(sai_attribute_t) *
                                                      thrift_attr_list.size());
    status = sai_api_query(SAI_API_FDB, (void **)&fdb_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    sai_thrift_parse_fdb_entry_attributes(thrift_attr_list, attr);
    uint32_t count = thrift_attr_list.size();
    sai_fdb_entry_t sai_fdb_entry;
    sai_fdb_entry = parse_thrift_fdb_entry(thrift_fdb_entry);
    return fdb_api->create_fdb_entry(&sai_fdb_entry, count, attr);
  }

  sai_thrift_status_t
  sai_thrift_delete_fdb_entry(const sai_thrift_fdb_entry_t &thrift_fdb_entry) {
    logger->info("sai_thrift_delete_fdb_entry");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_fdb_api_t *fdb_api;
    status = sai_api_query(SAI_API_FDB, (void **)&fdb_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    sai_fdb_entry_t sai_fdb_entry;
    sai_fdb_entry = parse_thrift_fdb_entry(thrift_fdb_entry);
    return fdb_api->remove_fdb_entry(&sai_fdb_entry);
  }

  void sai_thrift_parse_fdb_flush_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID:
                  attr_list[i].value.oid = (sai_object_id_t) attribute.value.oid;
                  break;
              case SAI_FDB_FLUSH_ATTR_VLAN_ID:
                  attr_list[i].value.u16 = attribute.value.u16;
                  break;
              case SAI_FDB_FLUSH_ATTR_ENTRY_TYPE:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
          }
      }
  }

  sai_thrift_status_t sai_thrift_flush_fdb_entries(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
      logger->info("sai_thrift_flush_fdb_entries");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_fdb_api_t *fdb_api;
      status = sai_api_query(SAI_API_FDB, (void **) &fdb_api);
      if (status != SAI_STATUS_SUCCESS) {
         return status;
      }
      sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
      sai_thrift_parse_fdb_flush_attributes(thrift_attr_list, attr_list);
      uint32_t attr_count = thrift_attr_list.size();
      status = fdb_api->flush_fdb_entries(0, attr_count, attr_list);
      free(attr_list);
      return status;
  }

  void sai_thrift_parse_vlan_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    for (uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
      attribute = (sai_thrift_attribute_t)*it;
      attr_list[i].id = attribute.id;
      switch (attribute.id) {
      case SAI_VLAN_ATTR_VLAN_ID:
        attr_list[i].value.u16 = attribute.value.u16;
        break;
      default:
        logger->error(
            "--> while parsing vlan_attr: {} was dumped in sai_cpp_server",
            attribute.id);
        break;
      }
    }
  }
  sai_thrift_object_id_t sai_thrift_create_vlan(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_vlan");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_vlan_api_t *vlan_api;
    sai_attribute_t *attr = (sai_attribute_t *)malloc(sizeof(sai_attribute_t) *
                                                      thrift_attr_list.size());
    status = sai_api_query(SAI_API_VLAN, (void **)&vlan_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      // return SAI_STATUS_NOT_IMPLEMENTED;
    }
    sai_thrift_parse_vlan_attributes(thrift_attr_list, attr);
    uint32_t count = thrift_attr_list.size();
    sai_object_id_t s_id = 0;
    sai_object_id_t vlan_id = 1;
    vlan_api->create_vlan(&vlan_id, s_id, count, attr);
    free(attr);
    return (sai_thrift_object_id_t)vlan_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_vlan(const sai_thrift_object_id_t vlan_id) {
    logger->info("sai_thrift_delete_vlan");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_vlan_api_t *vlan_api;
    status = sai_api_query(SAI_API_VLAN, (void **)&vlan_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    status = vlan_api->remove_vlan(vlan_id);
    return status;
  }

  void sai_thrift_get_vlan_stats(
      std::vector<int64_t> &_return, const sai_thrift_vlan_id_t vlan_id,
      const std::vector<sai_thrift_vlan_stat_counter_t> &counter_ids,
      const int32_t number_of_counters) {
    // Your implementation goes here
    logger->info("sai_thrift_get_vlan_stats");
  }

  void sai_thrift_parse_vlan_member_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    for (uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
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
        logger->error("--> while parsing vlan_member_attr: {}  was dumped in "
                      "sai_cpp_server",
                      attribute.id);
        break;
      }
    }
  }
  sai_thrift_object_id_t sai_thrift_create_vlan_member(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_vlan_member");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_vlan_api_t *vlan_api;
    sai_attribute_t *attr = (sai_attribute_t *)malloc(sizeof(sai_attribute_t) *
                                                      thrift_attr_list.size());
    status = sai_api_query(SAI_API_VLAN, (void **)&vlan_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      // return SAI_STATUS_NOT_IMPLEMENTED;
    }
    sai_thrift_parse_vlan_member_attributes(thrift_attr_list, attr);
    uint32_t count = thrift_attr_list.size();
    sai_object_id_t s_id = 0;
    sai_object_id_t vlan_member_id = 1;
    vlan_api->create_vlan_member(&vlan_member_id, s_id, count, attr);
    free(attr);
    return (sai_thrift_object_id_t)vlan_member_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_vlan_member(const sai_thrift_object_id_t vlan_member_id) {
    logger->info("sai_thrift_remove_vlan_member");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_vlan_api_t *vlan_api;
    status = sai_api_query(SAI_API_VLAN, (void **)&vlan_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      // return SAI_STATUS_NOT_IMPLEMENTED;
    }
    vlan_api->remove_vlan_member((sai_object_id_t)vlan_member_id);
    return (sai_thrift_object_id_t)vlan_member_id;
  }

  void sai_thrift_get_vlan_member_attribute(sai_thrift_attribute_list_t& _return, const sai_thrift_object_id_t vlan_member_id) {
    // Your implementation goes here
    printf("sai_thrift_get_vlan_member_attribute\n");
  }

   void sai_thrift_get_vlan_attribute(sai_thrift_attribute_list_t& thrift_attr_list, const sai_thrift_object_id_t vlan_id) {
      logger->info("sai_thrift_get_vlan_attribute");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_vlan_api_t *vlan_api;
      sai_attribute_t vlan_member_list_object_attribute;
      sai_thrift_attribute_t thrift_vlan_member_list_attribute;
      sai_object_list_t *vlan_member_list_object;
      status = sai_api_query(SAI_API_VLAN, (void **) &vlan_api);
      if (status != SAI_STATUS_SUCCESS) {
          return;
      }

      vlan_member_list_object_attribute.id = SAI_VLAN_ATTR_MEMBER_LIST;
      vlan_member_list_object_attribute.value.objlist.list = (sai_object_id_t *) malloc(sizeof(sai_object_id_t) * 128);
      vlan_member_list_object_attribute.value.objlist.count = 128;
      vlan_api->get_vlan_attribute(vlan_id, 1, &vlan_member_list_object_attribute);

      thrift_attr_list.attr_count = 1;
      std::vector<sai_thrift_attribute_t>& attr_list = thrift_attr_list.attr_list;
      thrift_vlan_member_list_attribute.id = SAI_VLAN_ATTR_MEMBER_LIST;
      thrift_vlan_member_list_attribute.value.objlist.count = vlan_member_list_object_attribute.value.objlist.count;
      std::vector<sai_thrift_object_id_t>& vlan_member_list = thrift_vlan_member_list_attribute.value.objlist.object_id_list;
      vlan_member_list_object = &vlan_member_list_object_attribute.value.objlist;
      for (int index = 0; index < vlan_member_list_object_attribute.value.objlist.count; index++) {
          vlan_member_list.push_back((sai_thrift_object_id_t) vlan_member_list_object->list[index]);
      }
      attr_list.push_back(thrift_vlan_member_list_attribute);
      free(vlan_member_list_object_attribute.value.objlist.list);
  }

  void sai_thrift_get_vlan_id(sai_thrift_result_t &ret, sai_thrift_object_id_t vlan_id) {
    logger->info("sai_thrift_get_vlan_id");
    sai_attribute_t vlan_attr;
    sai_vlan_api_t *vlan_api;

    // SAI_THRIFT_FUNC_LOG();

    ret.status = sai_api_query(SAI_API_VLAN, (void **) &vlan_api);
    if (ret.status != SAI_STATUS_SUCCESS) {
        // SAI_THRIFT_LOG_ERR("failed to obtain vlan_api, status:%d", ret.status);
        return;
    }
    vlan_attr.id = SAI_VLAN_ATTR_VLAN_ID;
    ret.status = vlan_api->get_vlan_attribute((sai_object_id_t)vlan_id, 1, &vlan_attr);
    if (ret.status != SAI_STATUS_SUCCESS) {
        // SAI_THRIFT_LOG_ERR("failed to get vlan ID, status:%d", ret.status);
        return;
    }
    ret.data.u16 = vlan_attr.value.u16;
  }

  void sai_thrift_parse_vr_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V4_STATE:
              case SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V6_STATE:
                  attr_list[i].value.booldata = attribute.value.booldata;
                  break;
              case SAI_VIRTUAL_ROUTER_ATTR_SRC_MAC_ADDRESS:
                  parse_mac_str(attribute.value.mac, attr_list[i].value.mac);
                  break;
          }
      }
      logger->info("after parsing");
  }

  sai_thrift_object_id_t sai_thrift_create_virtual_router(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_virtual_router");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_virtual_router_api_t *vr_api;
    sai_object_id_t vr_id = 0;
    status = sai_api_query(SAI_API_VIRTUAL_ROUTER, (void **) &vr_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    uint32_t attr_count = thrift_attr_list.size();
    sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * attr_count);
    logger->info("attr_count = {}", attr_count);
    sai_thrift_parse_vr_attributes(thrift_attr_list, attr_list);
    sai_object_id_t switch_id = 0;
    vr_api->create_virtual_router(&vr_id, switch_id, attr_count, attr_list);
    free(attr_list);
    return vr_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_virtual_router(const sai_thrift_object_id_t vr_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_virtual_router");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_virtual_router_api_t *vr_api;
    status = sai_api_query(SAI_API_VIRTUAL_ROUTER, (void **) &vr_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    status = vr_api->remove_virtual_router((sai_object_id_t)vr_id);
    return status;
  }

  void sai_thrift_parse_route_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_ROUTE_ENTRY_ATTR_NEXT_HOP_ID:
                  attr_list[i].value.oid = attribute.value.oid;
                  break;
              case SAI_ROUTE_ENTRY_ATTR_PACKET_ACTION:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
              default:
                  break;
          }
      }
  }

  void sai_thrift_string_to_v4_ip(const std::string s, unsigned int *m) {
      unsigned char r=0;
      unsigned int i;
      *m = 0;
      for(i=0;i<s.size();i++) {
          char let = s.c_str()[i];
          if (let >= '0' && let <= '9') {
              r = (r * 10) + (let - '0');
          } else {
              *m = (*m << 8) | r;
              r=0;
          }
      }
      *m = (*m << 8) | (r & 0xFF);
      *m = htonl(*m);
      return;
  }

  void sai_thrift_string_to_v6_ip(const std::string s, unsigned char *v6_ip) {
      const char *v6_str = s.c_str();
      inet_pton(AF_INET6, v6_str, v6_ip);
      return;
  }

  void sai_thrift_parse_ip_prefix(const sai_thrift_ip_prefix_t &thrift_ip_prefix, sai_ip_prefix_t *ip_prefix) {
      ip_prefix->addr_family = (sai_ip_addr_family_t) thrift_ip_prefix.addr_family;
      if ((sai_ip_addr_family_t)thrift_ip_prefix.addr_family == SAI_IP_ADDR_FAMILY_IPV4) {
          sai_thrift_string_to_v4_ip(thrift_ip_prefix.addr.ip4, &ip_prefix->addr.ip4);
          sai_thrift_string_to_v4_ip(thrift_ip_prefix.mask.ip4, &ip_prefix->mask.ip4);
      } else {
          sai_thrift_string_to_v6_ip(thrift_ip_prefix.addr.ip6, ip_prefix->addr.ip6);
          sai_thrift_string_to_v6_ip(thrift_ip_prefix.mask.ip6, ip_prefix->mask.ip6);
      }
  }

  void sai_thrift_parse_route_entry(const sai_thrift_route_entry_t &thrift_route_entry, sai_route_entry_t *route_entry) {
      route_entry->switch_id = 0;
      route_entry->vr_id = (sai_object_id_t) thrift_route_entry.vr_id;
      sai_thrift_parse_ip_prefix(thrift_route_entry.destination, &route_entry->destination);
  }

  sai_thrift_status_t sai_thrift_create_route(
      const sai_thrift_route_entry_t &thrift_route_entry,
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_route");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_route_api_t *route_api;
    sai_route_entry_t route_entry;
    status = sai_api_query(SAI_API_ROUTE, (void **) &route_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_thrift_parse_route_entry(thrift_route_entry, &route_entry);
    sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    sai_thrift_parse_route_attributes(thrift_attr_list, attr_list);
    uint32_t attr_count = thrift_attr_list.size();
    status = route_api->create_route_entry(&route_entry, attr_count, attr_list);
    free(attr_list);
    return status;
  }

  sai_thrift_status_t
  sai_thrift_remove_route(const sai_thrift_route_entry_t &thrift_route_entry) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_route");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_route_api_t *route_api;
    sai_route_entry_t route_entry;
    status = sai_api_query(SAI_API_ROUTE, (void **) &route_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_thrift_parse_route_entry(thrift_route_entry, &route_entry);
    status = route_api->remove_route_entry(&route_entry);
    return status;
  }

  void sai_thrift_parse_router_interface_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID:
              case SAI_ROUTER_INTERFACE_ATTR_PORT_ID:
              case SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL:
                  attr_list[i].value.oid = attribute.value.oid;
                  break;
              case SAI_ROUTER_INTERFACE_ATTR_TYPE:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
              case SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS:
                  parse_mac_str(attribute.value.mac, attr_list[i].value.mac);
                  break;
              case SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE:
              case SAI_ROUTER_INTERFACE_ATTR_ADMIN_V6_STATE:
                  attr_list[i].value.booldata = attribute.value.booldata;
                  break;
              case SAI_ROUTER_INTERFACE_ATTR_VLAN_ID:
                  attr_list[i].value.u16 = attribute.value.u16;
              default:
                  break;
          }
      }
  }


  sai_thrift_object_id_t sai_thrift_create_router_interface(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_router_interface");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_router_interface_api_t *rif_api;
    sai_object_id_t rif_id = 0;
    status = sai_api_query(SAI_API_ROUTER_INTERFACE, (void **) &rif_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    sai_thrift_parse_router_interface_attributes(thrift_attr_list, attr_list);
    uint32_t attr_count = thrift_attr_list.size();
    sai_object_id_t switch_id =0;
    status = rif_api->create_router_interface(&rif_id, switch_id, attr_count, attr_list);
    free(attr_list);
    return rif_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_router_interface(const sai_thrift_object_id_t rif_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_router_interface");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_router_interface_api_t *rif_api;
    status = sai_api_query(SAI_API_ROUTER_INTERFACE, (void **) &rif_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    status = rif_api->remove_router_interface((sai_object_id_t)rif_id);
    return status;
  }

  sai_thrift_status_t sai_thrift_set_router_interface_attribute(
      const sai_thrift_object_id_t rif_id,
      const sai_thrift_attribute_t &thrift_attr) {
    // Your implementation goes here
    logger->info("sai_thrift_set_router_interface_attribute");
  }

  void sai_thrift_parse_ip_address(const sai_thrift_ip_address_t &thrift_ip_address, sai_ip_address_t *ip_address) {
      ip_address->addr_family = (sai_ip_addr_family_t) thrift_ip_address.addr_family;
      if ((sai_ip_addr_family_t)thrift_ip_address.addr_family == SAI_IP_ADDR_FAMILY_IPV4) {
          sai_thrift_string_to_v4_ip(thrift_ip_address.addr.ip4, &ip_address->addr.ip4);
      } else {
          sai_thrift_string_to_v6_ip(thrift_ip_address.addr.ip6, ip_address->addr.ip6);
      }
  }

  void sai_thrift_parse_next_hop_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
          attribute = (sai_thrift_attribute_t)*it;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_NEXT_HOP_ATTR_TYPE:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
              case SAI_NEXT_HOP_ATTR_IP:
                  sai_thrift_parse_ip_address(attribute.value.ipaddr, &attr_list[i].value.ipaddr);
                  break;
              case SAI_NEXT_HOP_ATTR_ROUTER_INTERFACE_ID:
                  attr_list[i].value.oid = attribute.value.oid;
                  break;
          }
      }
  }


  sai_thrift_object_id_t sai_thrift_create_next_hop(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_next_hop");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_next_hop_api_t *nhop_api;
    sai_object_id_t nhop_id = 0;
    status = sai_api_query(SAI_API_NEXT_HOP, (void **) &nhop_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    sai_thrift_parse_next_hop_attributes(thrift_attr_list, attr_list);
    uint32_t attr_count = thrift_attr_list.size();
    status = nhop_api->create_next_hop(&nhop_id, 0, attr_count, attr_list);
    free(attr_list);
    return nhop_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_next_hop(const sai_thrift_object_id_t next_hop_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_next_hop");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_next_hop_api_t *nhop_api;
    status = sai_api_query(SAI_API_NEXT_HOP, (void **) &nhop_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    status = nhop_api->remove_next_hop((sai_object_id_t)next_hop_id);
    return status;
  }

  sai_thrift_object_id_t sai_thrift_create_next_hop_group(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_next_hop_group");
  }

  sai_thrift_status_t sai_thrift_remove_next_hop_group(
      const sai_thrift_object_id_t nhop_group_oid) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_next_hop_group");
  }

  sai_thrift_object_id_t sai_thrift_create_next_hop_group_member(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_next_hop_group_member");
  }

  sai_thrift_status_t sai_thrift_remove_next_hop_group_member(
      const sai_thrift_object_id_t nhop_group_member_oid) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_next_hop_group_member");
  }

  void sai_thrift_parse_lag_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    for (uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
      attribute = (sai_thrift_attribute_t)*it;
      attr_list[i].id = attribute.id;
      switch (attribute.id) {
      default:
        logger->error("while parsing lag_attr: attribute.id = {} was dumped in "
                      "sai_cpp_server ",
                      attribute.id);
        break;
      }
    }
  }

  void sai_thrift_parse_lag_member_attributes(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list,
      sai_attribute_t *attr_list) {
    std::vector<sai_thrift_attribute_t>::const_iterator it =
        thrift_attr_list.begin();
    sai_thrift_attribute_t attribute;
    for (uint32_t i = 0; i < thrift_attr_list.size(); i++, it++) {
      attribute = (sai_thrift_attribute_t)*it;
      attr_list[i].id = attribute.id;
      switch (attribute.id) {
      case SAI_LAG_MEMBER_ATTR_PORT_ID:
        attr_list[i].value.oid = attribute.value.oid;
        break;
      case SAI_LAG_MEMBER_ATTR_LAG_ID:
        attr_list[i].value.oid = attribute.value.oid;
        break;
      default:
        logger->error(
            "while parsing lag_member_attr: attribute.id = {} was dumped in "
            "sai_cpp_server",
            attribute.id);
        break;
      }
    }
  }

  sai_thrift_object_id_t sai_thrift_create_lag(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_lag");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_lag_api_t *lag_api;
    sai_attribute_t *attr = (sai_attribute_t *)malloc(sizeof(sai_attribute_t) *
                                                      thrift_attr_list.size());
    status = sai_api_query(SAI_API_LAG, (void **)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
    }
    sai_thrift_parse_lag_attributes(thrift_attr_list, attr);
    uint32_t count = thrift_attr_list.size();
    sai_object_id_t s_id = 0;
    sai_object_id_t lag_id = 1;
    lag_api->create_lag(&lag_id, s_id, count, attr);
    free(attr);
    return (sai_thrift_object_id_t)lag_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_lag(const sai_thrift_object_id_t lag_id) {
    logger->info("sai_thrift_remove_lag");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_lag_api_t *lag_api;
    status = sai_api_query(SAI_API_LAG, (void **)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    sai_object_id_t s_id = 0;
    lag_api->remove_lag(lag_id);
    return status;
  }

  sai_thrift_object_id_t sai_thrift_create_lag_member(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    logger->info("sai_thrift_create_lag_member");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_lag_api_t *lag_api;
    sai_attribute_t *attr = (sai_attribute_t *)malloc(sizeof(sai_attribute_t) *
                                                      thrift_attr_list.size());
    status = sai_api_query(SAI_API_LAG, (void **)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
    }
    sai_thrift_parse_lag_member_attributes(thrift_attr_list, attr);
    uint32_t count = thrift_attr_list.size();
    sai_object_id_t s_id = 0;
    sai_object_id_t lag_member_id = 1;
    lag_api->create_lag_member(&lag_member_id, s_id, count, attr);
    free(attr);
    return (sai_thrift_object_id_t)lag_member_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_lag_member(const sai_thrift_object_id_t lag_member_id) {
    logger->info("sai_thrift_remove_lag_member");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_lag_api_t *lag_api;
    status = sai_api_query(SAI_API_LAG, (void **)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return SAI_STATUS_NOT_IMPLEMENTED;
    }
    sai_object_id_t s_id = 0;
    lag_api->remove_lag_member(lag_member_id);
    return status;
  }

  void sai_thrift_get_lag_member_attribute(sai_thrift_attribute_list_t& thrift_attr_list, const sai_thrift_object_id_t lag_member_id) {
    // Your implementation goes here
    logger->info("sai_thrift_get_lag_member_attribute\n");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_attribute_t sai_attrs[2];
    sai_lag_api_t *lag_api;
    status = sai_api_query(SAI_API_LAG, (void **) &lag_api);
    if (status != SAI_STATUS_SUCCESS) {
        logger->error("failed to obtain lag_api, status:{}", status);
        // SAI_THRIFT_LOG_ERR("failed to obtain lag_api, status:%d", status);
        return;
    }
    // SAI_THRIFT_FUNC_LOG();
    sai_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    sai_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;

    status = lag_api->get_lag_member_attribute(lag_member_id, 2, sai_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        logger->error("failed to obtain lag member attributes, status:{}", status);
        // SAI_THRIFT_LOG_ERR("failed to obtain lag member attributes, status:%d", status);
        return;
    }

    sai_attributes_to_sai_thrift_list(sai_attrs, 2, thrift_attr_list.attr_list);
  }

  sai_thrift_object_id_t sai_thrift_create_stp_entry(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_stp_entry");
  }

  sai_thrift_status_t
  sai_thrift_remove_stp_entry(const sai_thrift_object_id_t stp_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_stp_entry");
  }

  sai_thrift_status_t sai_thrift_set_stp_port_state(
      const sai_thrift_object_id_t stp_id, const sai_thrift_object_id_t port_id,
      const sai_thrift_port_stp_port_state_t stp_port_state) {
    // Your implementation goes here
    logger->info("sai_thrift_set_stp_port_state");
  }

  sai_thrift_port_stp_port_state_t
  sai_thrift_get_stp_port_state(const sai_thrift_object_id_t stp_id,
                                const sai_thrift_object_id_t port_id) {
    // Your implementation goes here
    logger->info("sai_thrift_get_stp_port_state");
  }

  void sai_thrift_parse_neighbor_entry(const sai_thrift_neighbor_entry_t &thrift_neighbor_entry, sai_neighbor_entry_t *neighbor_entry) {
      neighbor_entry->switch_id = 0;
      neighbor_entry->rif_id = (sai_object_id_t) thrift_neighbor_entry.rif_id;
      sai_thrift_parse_ip_address(thrift_neighbor_entry.ip_address, &neighbor_entry->ip_address);
  }

  void sai_thrift_parse_neighbor_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it1 = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it1++) {
          attribute = (sai_thrift_attribute_t)*it1;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_NEIGHBOR_ENTRY_ATTR_DST_MAC_ADDRESS:
                  parse_mac_str(attribute.value.mac, attr_list[i].value.mac);
                  break;
          }
      }
  }

  sai_thrift_status_t sai_thrift_create_neighbor_entry(
      const sai_thrift_neighbor_entry_t &thrift_neighbor_entry,
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_neighbor_entry");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_neighbor_api_t *neighbor_api;
    status = sai_api_query(SAI_API_NEIGHBOR, (void **) &neighbor_api);
    sai_neighbor_entry_t neighbor_entry;
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_thrift_parse_neighbor_entry(thrift_neighbor_entry, &neighbor_entry);
    sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    sai_thrift_parse_neighbor_attributes(thrift_attr_list, attr_list);
    uint32_t attr_count = thrift_attr_list.size();
    status = neighbor_api->create_neighbor_entry(&neighbor_entry, attr_count, attr_list);
    free(attr_list);
    return status;
  }

  sai_thrift_status_t sai_thrift_remove_neighbor_entry(
      const sai_thrift_neighbor_entry_t &thrift_neighbor_entry) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_neighbor_entry");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_neighbor_api_t *neighbor_api;
    sai_neighbor_entry_t neighbor_entry;
    status = sai_api_query(SAI_API_NEIGHBOR, (void **) &neighbor_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_thrift_parse_neighbor_entry(thrift_neighbor_entry, &neighbor_entry);
    status = neighbor_api->remove_neighbor_entry(&neighbor_entry);
    return status;
  }

  sai_thrift_object_id_t sai_thrift_create_switch(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // TODO: Currently not caring about attr_list.
    // sai_attribute_t *attr = (sai_attribute_t*) malloc(sizeof(sai_attribute_t)
    // * thrift_attr_list.size());
    logger->info("sai_thrift_create_switch - currently only initiated on "
                 "server constructor");
  }

  sai_attribute_t parse_switch_attribute(sai_thrift_attribute_t thrift_attr) {
    sai_attribute_t sai_attr;
    sai_attr.id = thrift_attr.id;
    if (thrift_attr.id == SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID) {
      sai_attr.value.oid = thrift_attr.value.oid;
    }
    // if (thrift_attr.id == SAI_SWITCH_ATTR_PORT_LIST) {
    //   sai_attr.value.objlist.count = thrift_attr.value.objlist.count;
    //   sai_attr.value.objlist.list = (uint64_t*)
    //   &thrift_attr.value.objlist.object_id_list[0];
    // }
    return sai_attr;
  }

  sai_thrift_attribute_t
  parse_switch_thrift_attribute(sai_attribute_t *sai_attr) {
    sai_thrift_attribute_t thrift_attr;
    thrift_attr.id = sai_attr->id;
    if (sai_attr->id == SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID) {
      thrift_attr.value.oid = sai_attr->value.oid;
    }
    if (sai_attr->id == SAI_SWITCH_ATTR_CPU_PORT) {
      thrift_attr.value.oid = sai_attr->value.oid;
    }
    // if (sai_attr->id == SAI_SWITCH_ATTR_PORT_LIST) {
    //   thrift_attr.value.objlist.object_id_list.clear();
    //   thrift_attr.value.objlist.count = sai_attr->value.objlist.count;
    //   for (int j=0;j<sai_attr->value.objlist.count;j++) {
    //     thrift_attr.value.objlist.object_id_list.push_back(sai_attr->value.objlist.list[j]);
    //   }
    // }
    return thrift_attr;
  }

 void sai_thrift_get_switch_attribute(sai_thrift_attribute_list_t& thrift_attr_list) {
      logger->info("sai_thrift_get_switch_attribute");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_switch_api_t *switch_api;
      sai_attribute_t max_port_attribute;
      sai_attribute_t port_list_object_attribute;
      sai_attribute_t cpu_port_attribute;
      sai_thrift_attribute_t thrift_port_list_attribute;
      sai_thrift_attribute_t thrift_cpu_port_attribute;
      sai_object_list_t *port_list_object;
      int max_ports = 0;
      status = sai_api_query(SAI_API_SWITCH, (void **) &switch_api);
      if (status != SAI_STATUS_SUCCESS) {
          logger->error("sai_api_query failed!!!");
          return;
      }

      max_port_attribute.id = SAI_SWITCH_ATTR_PORT_NUMBER;
      switch_api->get_switch_attribute(0, 1, &max_port_attribute);
      max_ports = max_port_attribute.value.u32;
      port_list_object_attribute.id = SAI_SWITCH_ATTR_PORT_LIST;
      port_list_object_attribute.value.objlist.list = (sai_object_id_t *) malloc(sizeof(sai_object_id_t) * max_ports);
      port_list_object_attribute.value.objlist.count = max_ports;
      switch_api->get_switch_attribute(0, 1, &port_list_object_attribute);

      thrift_attr_list.attr_count = 1;
      std::vector<sai_thrift_attribute_t>& attr_list = thrift_attr_list.attr_list;
      thrift_port_list_attribute.id = SAI_SWITCH_ATTR_PORT_LIST;
      thrift_port_list_attribute.value.objlist.count = max_ports;
      std::vector<sai_thrift_object_id_t>& port_list = thrift_port_list_attribute.value.objlist.object_id_list;
      port_list_object = &port_list_object_attribute.value.objlist;
      for (int index = 0; index < max_ports; index++) {
          port_list.push_back((sai_thrift_object_id_t) port_list_object->list[index]);
      }
      attr_list.push_back(thrift_port_list_attribute);
      free(port_list_object_attribute.value.objlist.list);

      cpu_port_attribute.id = SAI_SWITCH_ATTR_CPU_PORT;
      switch_api->get_switch_attribute(0, 1, &cpu_port_attribute);
      thrift_cpu_port_attribute = parse_switch_thrift_attribute(&cpu_port_attribute);
      attr_list.push_back(thrift_cpu_port_attribute);
  }

  void sai_thrift_get_port_list_by_front_port(sai_thrift_attribute_t &_return) {
    // Your implementation goes here
    logger->info("sai_thrift_get_port_list_by_front_port");
  }

  sai_thrift_object_id_t sai_thrift_get_cpu_port_id() {
    // Your implementation goes here
    logger->info("sai_thrift_get_cpu_port_id");
    sai_switch_api_t *switch_api;
    sai_status_t status = sai_api_query(SAI_API_SWITCH, (void **) &switch_api);
    if (status != SAI_STATUS_SUCCESS) {
        logger->error("sai_api_query failed!!!");
        return SAI_NULL_OBJECT_ID;
    }
    sai_attribute_t cpu_port_attribute;
    cpu_port_attribute.id = SAI_SWITCH_ATTR_CPU_PORT;
    switch_api->get_switch_attribute(0, 1, &cpu_port_attribute);
    return cpu_port_attribute.value.oid;
  }

  sai_thrift_object_id_t sai_thrift_get_default_trap_group() {
    // Your implementation goes here
    logger->info("sai_thrift_get_default_trap_group");
  }

  sai_thrift_object_id_t sai_thrift_get_default_router_id() {
    // Your implementation goes here
    logger->info("sai_thrift_get_default_router_id");
  }

  sai_thrift_object_id_t sai_thrift_get_default_1q_bridge_id()
  {
      logger->info("sai_thrift_get_default_1q_bridge_id");
      sai_switch_api_t *switch_api;
      sai_attribute_t attr;
      sai_status_t status;

      // SAI_THRIFT_FUNC_LOG();
      status = sai_api_query(SAI_API_SWITCH, (void **) &switch_api);
      if (status != SAI_STATUS_SUCCESS) {
          logger->error("failed to obtain switch_api, status:%d\n", status);
          return SAI_NULL_OBJECT_ID;
      }

      attr.id = SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID;
      status = switch_api->get_switch_attribute(0, 1, &attr);
      if (status != SAI_STATUS_SUCCESS)
      {
          logger->error("Failed to get switch virtual router ID, status %d", status);
          return SAI_NULL_OBJECT_ID;
      }
      logger->info("default_1q_bride_id {}", attr.value.oid);
      return (sai_thrift_object_id_t)attr.value.oid;
  }

  void sai_thrift_get_default_vlan_id(sai_thrift_result_t &ret) {
    logger->info("sai_thrift_get_default_vlan_id");
      sai_switch_api_t *switch_api;
      sai_attribute_t attr;

      // SAI_THRIFT_FUNC_LOG();

      ret.status = sai_api_query(SAI_API_SWITCH, (void **) &switch_api);
      if (ret.status != SAI_STATUS_SUCCESS) {
          // SAI_THRIFT_LOG_ERR("failed to obtain switch_api, status:%d", ret.status);
          return;
      }

      attr.id = SAI_SWITCH_ATTR_DEFAULT_VLAN_ID;
      ret.status = switch_api->get_switch_attribute(0, 1, &attr);
      if (ret.status != SAI_STATUS_SUCCESS)
      {
          // SAI_THRIFT_LOG_ERR("failed to get switch default vlan ID, status:%d", ret.status);
          return;
      }

      ret.data.oid = (sai_thrift_object_id_t)attr.value.oid;
  }

  sai_thrift_object_id_t
  sai_thrift_get_port_id_by_front_port(const std::string &port_name) {
    // Your implementation goes here
    uint32_t hw_port = std::stoi(port_name);
    logger->info("sai_thrift_get_port_id_by_front_port ({})", hw_port);

    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_switch_api_t *switch_api;
    sai_port_api_t *port_api;
    status = sai_api_query(SAI_API_SWITCH, (void **)&switch_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return -1;
    }
    status = sai_api_query(SAI_API_PORT, (void **)&port_api);
    if (status != SAI_STATUS_SUCCESS) {
      logger->error("sai_api_query failed!!!");
      return -1;
    }
    sai_attribute_t max_port_attribute;
    max_port_attribute.id = SAI_SWITCH_ATTR_PORT_NUMBER;
    sai_object_id_t s_id = 1;
    switch_api->get_switch_attribute(s_id, 1, &max_port_attribute);
    uint32_t max_ports = max_port_attribute.value.u32;

    sai_attribute_t port_list_object_attribute;
    port_list_object_attribute.id = SAI_SWITCH_ATTR_PORT_LIST;
    port_list_object_attribute.value.objlist.list =
        (sai_object_id_t *)malloc(sizeof(sai_object_id_t) * max_ports);
    port_list_object_attribute.value.objlist.count = max_ports;
    switch_api->get_switch_attribute(s_id, 1, &port_list_object_attribute);
    sai_object_id_t port_id;
    bool found = false;
    bool mem_assigned = false;
    sai_attribute_t hw_lane_list_attr;
    for (int i = 0; i < max_ports; i++) {
      hw_lane_list_attr.id = SAI_PORT_ATTR_HW_LANE_LIST;
      hw_lane_list_attr.value.u32list.list =
          (uint32_t *)malloc(sizeof(uint32_t));
      mem_assigned = true;
      port_api->get_port_attribute(
          port_list_object_attribute.value.objlist.list[i], 1,
          &hw_lane_list_attr);
      if (hw_lane_list_attr.value.u32list.list[0] == hw_port) {
        port_id = port_list_object_attribute.value.objlist.list[i];
        found = true;
      }
    }
    if (!found) {
      logger->info("didn't find port");
    }
    if (mem_assigned == true) {
      // logger->info("--> freeing hw_lane_list_attr.value.u32list.list");
      free(hw_lane_list_attr.value.u32list.list);
    }
    // logger->info("--> freeing
    // port_list_object_attribute.value.objlist.list");
    free(port_list_object_attribute.value.objlist.list);

    return port_id;
  }

  void sai_thrift_parse_switch_attribute(const sai_thrift_attribute_t &thrift_attr, sai_attribute_t *attr) {
      attr->id = thrift_attr.id;

      switch(thrift_attr.id) {
          case SAI_SWITCH_ATTR_SRC_MAC_ADDRESS:
              parse_mac_str(thrift_attr.value.mac, attr->value.mac);
              break;

          case SAI_SWITCH_ATTR_FDB_UNICAST_MISS_PACKET_ACTION:
          case SAI_SWITCH_ATTR_FDB_BROADCAST_MISS_PACKET_ACTION:
          case SAI_SWITCH_ATTR_FDB_MULTICAST_MISS_PACKET_ACTION:
              attr->value.s32 = thrift_attr.value.s32;
              break;
      }
  }

  sai_thrift_status_t sai_thrift_set_switch_attribute(const sai_thrift_attribute_t& thrift_attr) {
    // Your implementation goes here
    logger->info("sai_thrift_set_switch_attribute");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_switch_api_t *switch_api;
    sai_attribute_t attr;
    status = sai_api_query(SAI_API_SWITCH, (void **) &switch_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("sai_api_query failed!!!\n");
        return status;
    }
    sai_thrift_parse_switch_attribute(thrift_attr, &attr);
    sai_object_id_t switch_id = 0;
    status = switch_api->set_switch_attribute(switch_id, &attr);
    return status;
  }

  void sai_thrift_parse_hostif_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it1 = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it1++) {
          attribute = (sai_thrift_attribute_t)*it1;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_HOSTIF_ATTR_TYPE:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
              case SAI_HOSTIF_ATTR_OBJ_ID:
                  attr_list[i].value.oid = attribute.value.oid;
                  break;
              case SAI_HOSTIF_ATTR_NAME:
                  memcpy(attr_list[i].value.chardata, attribute.value.chardata.c_str(), HOSTIF_NAME_SIZE);
                  break;
          }
      }
  }

  sai_thrift_object_id_t sai_thrift_create_hostif(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
      logger->info("sai_thrift_create_hostif");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_hostif_api_t *hostif_api;
      sai_object_id_t hif_id;
      status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
      if (status != SAI_STATUS_SUCCESS) {
          return status;
      }
      sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
      sai_thrift_parse_hostif_attributes(thrift_attr_list, attr_list);
      uint32_t attr_count = thrift_attr_list.size();
      sai_object_id_t s_id = 0;
      status = hostif_api->create_hostif(&hif_id, s_id, attr_count, attr_list);
      free(attr_list);
      return hif_id;
  }

  sai_thrift_status_t sai_thrift_remove_hostif(const sai_thrift_object_id_t hif_id) {
      logger->info("sai_thrift_remove_hostif");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_hostif_api_t *hostif_api;
      status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
      if (status != SAI_STATUS_SUCCESS) {
          return status;
      }
      status = hostif_api->remove_hostif((sai_object_id_t) hif_id);
      return status;
  }

  void sai_thrift_parse_hostif_trap_group_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it1 = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it1++) {
          attribute = (sai_thrift_attribute_t)*it1;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_HOSTIF_TRAP_GROUP_ATTR_QUEUE:
                  attr_list[i].value.u32 = attribute.value.u32;
                  break;
              case SAI_HOSTIF_TRAP_GROUP_ATTR_POLICER:
                  attr_list[i].value.oid = attribute.value.oid;
                  break;
          }
      }
  }

  sai_thrift_object_id_t sai_thrift_create_hostif_trap_group(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
      logger->info("sai_thrift_create_hostif_trap_group");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_hostif_api_t *hostif_api;
      sai_object_id_t hif_trap_group_id;
      status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
      if (status != SAI_STATUS_SUCCESS) {
          return status;
      }
      sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
      sai_thrift_parse_hostif_trap_group_attributes(thrift_attr_list, attr_list);
      uint32_t attr_count = thrift_attr_list.size();
      sai_object_id_t s_id = 0;
      status = hostif_api->create_hostif_trap_group(&hif_trap_group_id, s_id, attr_count, attr_list);
      free(attr_list);
      return hif_trap_group_id;
  }

  sai_thrift_status_t sai_thrift_remove_hostif_trap_group(const sai_thrift_object_id_t hif_trap_group_id) {
      logger->info("sai_thrift_remove_hostif_trap_group\n");
      sai_status_t status = SAI_STATUS_SUCCESS;
      sai_hostif_api_t *hostif_api;
      status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
      if (status != SAI_STATUS_SUCCESS) {
          return status;
      }
      status = hostif_api->remove_hostif_trap_group((sai_object_id_t) hif_trap_group_id);
      return status;
  }

  void sai_thrift_parse_hostif_trap_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it1 = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it1++) {
          attribute = (sai_thrift_attribute_t)*it1;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_HOSTIF_TRAP_ATTR_PACKET_ACTION:
              case SAI_HOSTIF_TRAP_ATTR_TRAP_TYPE:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
              case SAI_HOSTIF_TRAP_ATTR_TRAP_PRIORITY:
                  attr_list[i].value.u32 = attribute.value.u32;
                  break;
              case SAI_HOSTIF_TRAP_ATTR_TRAP_GROUP:
                  attr_list[i].value.oid = attribute.value.oid;
                  break;
              default:
                  break;
          }
      }
  }

  sai_thrift_status_t sai_thrift_create_hostif_trap(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_hostif_trap");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_hostif_api_t *hostif_api;
    sai_object_id_t hif_trap_id;
    status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    sai_thrift_parse_hostif_trap_attributes(thrift_attr_list, attr_list);
    uint32_t attr_count = thrift_attr_list.size();
    sai_object_id_t s_id = 0;
    status = hostif_api->create_hostif_trap(&hif_trap_id, s_id, attr_count, attr_list);
    free(attr_list);
    return hif_trap_id;
  }

  sai_thrift_status_t
  sai_thrift_remove_hostif_trap(const sai_thrift_hostif_trap_id_t trap_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_hostif_trap");
    sai_status_t status = SAI_STATUS_SUCCESS;
      sai_hostif_api_t *hostif_api;
      status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
      if (status != SAI_STATUS_SUCCESS) {
          return status;
      }
      status = hostif_api->remove_hostif_trap((sai_object_id_t) trap_id);
      return status;
  }

  sai_thrift_status_t
  sai_thrift_set_hostif_trap(const sai_thrift_object_id_t trap_id,
                             const sai_thrift_attribute_t &thrift_attr) {
    // Your implementation goes here
    logger->info("sai_thrift_set_hostif_trap");
  }

  sai_thrift_status_t
  sai_thrift_set_hostif_trap_group(const sai_thrift_object_id_t trap_group_id,
                                   const sai_thrift_attribute_t &thrift_attr) {
    // Your implementation goes here
    logger->info("sai_thrift_set_hostif_trap_group");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_table(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_acl_table");
  }

  void sai_thrift_parse_hostif_table_attributes(const std::vector<sai_thrift_attribute_t> &thrift_attr_list, sai_attribute_t *attr_list) {
      std::vector<sai_thrift_attribute_t>::const_iterator it1 = thrift_attr_list.begin();
      sai_thrift_attribute_t attribute;
      for(uint32_t i = 0; i < thrift_attr_list.size(); i++, it1++) {
          attribute = (sai_thrift_attribute_t)*it1;
          attr_list[i].id = attribute.id;
          switch (attribute.id) {
              case SAI_HOSTIF_TABLE_ENTRY_ATTR_TYPE:
              case SAI_HOSTIF_TABLE_ENTRY_ATTR_CHANNEL_TYPE:
                  attr_list[i].value.s32 = attribute.value.s32;
                  break;
              case SAI_HOSTIF_TABLE_ENTRY_ATTR_TRAP_ID:
                  attr_list[i].value.oid = attribute.value.oid;
                  break;
              default:
                  break;
          }
      }
  }

  sai_thrift_object_id_t sai_thrift_create_hostif_table_entry(const std::vector<sai_thrift_attribute_t> & thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_hostif_table_entry");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_hostif_api_t *hostif_api;
    sai_object_id_t hif_table_id;
    status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    sai_attribute_t *attr_list = (sai_attribute_t *) malloc(sizeof(sai_attribute_t) * thrift_attr_list.size());
    sai_thrift_parse_hostif_table_attributes(thrift_attr_list, attr_list);
    uint32_t attr_count = thrift_attr_list.size();
    sai_object_id_t s_id = 0;
    status = hostif_api->create_hostif_table_entry(&hif_table_id, s_id, attr_count, attr_list);
    free(attr_list);
    return hif_table_id;
  }

  sai_thrift_status_t sai_thrift_remove_hostif_table_entry(const sai_thrift_object_id_t hif_table_entry_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_hostif_table_entry");
    sai_status_t status = SAI_STATUS_SUCCESS;
    sai_hostif_api_t *hostif_api;
    status = sai_api_query(SAI_API_HOSTIF, (void **) &hostif_api);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    status = hostif_api->remove_hostif_table_entry(hif_table_entry_id);
    return status;
  }

  sai_thrift_status_t
  sai_thrift_remove_acl_table(const sai_thrift_object_id_t acl_table_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_acl_table");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_entry(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_acl_entry");
  }

  sai_thrift_status_t
  sai_thrift_remove_acl_entry(const sai_thrift_object_id_t acl_entry) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_acl_entry");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_table_group(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_acl_table_group");
  }

  sai_thrift_status_t sai_thrift_remove_acl_table_group(
      const sai_thrift_object_id_t acl_table_group_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_acl_table_group");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_table_group_member(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_acl_table_group_member");
  }

  sai_thrift_status_t sai_thrift_remove_acl_table_group_member(
      const sai_thrift_object_id_t acl_table_group_member_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_acl_table_group_member");
  }

  sai_thrift_object_id_t sai_thrift_create_acl_counter(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_acl_counter");
  }

  sai_thrift_status_t
  sai_thrift_remove_acl_counter(const sai_thrift_object_id_t acl_counter_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_acl_counter");
  }

  void sai_thrift_get_acl_counter_attribute(
      std::vector<sai_thrift_attribute_value_t> &_return,
      const sai_thrift_object_id_t acl_counter_id,
      const std::vector<int32_t> &thrift_attr_ids) {
    // Your implementation goes here
    logger->info("sai_thrift_get_acl_counter_attribute");
  }

  sai_thrift_object_id_t sai_thrift_create_mirror_session(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_mirror_session");
  }

  sai_thrift_status_t
  sai_thrift_remove_mirror_session(const sai_thrift_object_id_t session_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_mirror_session");
  }

  sai_thrift_object_id_t sai_thrift_create_policer(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_policer");
  }

  sai_thrift_status_t
  sai_thrift_remove_policer(const sai_thrift_object_id_t policer_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_policer");
  }

  void sai_thrift_get_policer_stats(
      std::vector<int64_t> &_return, const sai_thrift_object_id_t policer_id,
      const std::vector<sai_thrift_policer_stat_counter_t> &counter_ids) {
    // Your implementation goes here
    logger->info("sai_thrift_get_policer_stats");
  }

  sai_thrift_object_id_t sai_thrift_create_scheduler_profile(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_scheduler_profile");
  }

  sai_thrift_status_t sai_thrift_remove_scheduler_profile(
      const sai_thrift_object_id_t scheduler_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_scheduler_profile");
  }

  void sai_thrift_get_queue_stats(
      std::vector<int64_t> &_return, const sai_thrift_object_id_t queue_id,
      const std::vector<sai_thrift_queue_stat_counter_t> &counter_ids,
      const int32_t number_of_counters) {
    // Your implementation goes here
    logger->info("sai_thrift_get_queue_stats");
  }

  sai_thrift_status_t sai_thrift_clear_queue_stats(
      const sai_thrift_object_id_t queue_id,
      const std::vector<sai_thrift_queue_stat_counter_t> &counter_ids,
      const int32_t number_of_counters) {
    // Your implementation goes here
    logger->info("sai_thrift_clear_queue_stats");
  }

  sai_thrift_status_t
  sai_thrift_set_queue_attribute(const sai_thrift_object_id_t queue_id,
                                 const sai_thrift_attribute_t &thrift_attr) {
    // Your implementation goes here
    logger->info("sai_thrift_set_queue_attribute");
  }

  sai_thrift_object_id_t sai_thrift_create_buffer_profile(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_buffer_profile");
  }

  sai_thrift_object_id_t sai_thrift_create_pool_profile(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_pool_profile");
  }

  sai_thrift_status_t sai_thrift_set_priority_group_attribute(
      const sai_thrift_object_id_t pg_id,
      const sai_thrift_attribute_t &thrift_attr) {
    // Your implementation goes here
    logger->info("sai_thrift_set_priority_group_attribute");
  }

  void sai_thrift_get_pg_stats(
      std::vector<int64_t> &_return, const sai_thrift_object_id_t pg_id,
      const std::vector<sai_thrift_pg_stat_counter_t> &counter_ids,
      const int32_t number_of_counters) {
    // Your implementation goes here
    logger->info("sai_thrift_get_pg_stats");
  }

  sai_thrift_object_id_t sai_thrift_create_wred_profile(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_wred_profile");
  }

  sai_thrift_status_t
  sai_thrift_remove_wred_profile(const sai_thrift_object_id_t wred_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_wred_profile");
  }

  sai_thrift_object_id_t sai_thrift_create_qos_map(
      const std::vector<sai_thrift_attribute_t> &thrift_attr_list) {
    // Your implementation goes here
    logger->info("sai_thrift_create_qos_map");
  }

  sai_thrift_status_t
  sai_thrift_remove_qos_map(const sai_thrift_object_id_t qos_map_id) {
    // Your implementation goes here
    logger->info("sai_thrift_remove_qos_map");
  }
};

TSimpleServer *server_ptr;

void close_rpc_server(int signum) { server_ptr->stop(); }

int main(int argc, char **argv) {
  // logging
  auto logger = spdlog::basic_logger_mt("logger", "log.txt");
  logger->flush_on(spdlog::level::info);     // make err
  spdlog::set_pattern("[thread %t %T.%e] %l %v ");
  auto inline_log = spdlog::stdout_color_mt("inline_log");
  inline_log->info("creating server for SAI on port {}", sai_port);
  logger->info("creating server for SAI on port {}", sai_port);

  // open server to sai functions
  // boost::shared_ptr<switch_sai_rpcHandler> handler(new switch_sai_rpcHandler());
  //int fd = open("/proc/1/ns/net", O_RDONLY);
  //if (setns(fd, 0) == -1) {
  //  return -1;
  //}

  boost::shared_ptr<switch_sai_rpcHandler> handler(new switch_sai_rpcHandler());
  // fd = open("/var/run/netns/sw_net",
  //               O_RDONLY);
  // if (setns(fd, 0) == -1) {
  //   return -1;
  // }
  boost::shared_ptr<TProcessor> processor(new switch_sai_rpcProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(
      new TServerSocket(sai_port));
  boost::shared_ptr<TTransportFactory> transportFactory(
      new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(
      new TBinaryProtocolFactory());
  TSimpleServer server(processor, serverTransport, transportFactory,
                       protocolFactory);
  server_ptr = &server;
  signal(SIGINT, close_rpc_server);
  logger->info("SAI rpc server started on port {}", sai_port);
  server.serve();
  logger->info("thrift done");
  spdlog::drop_all();
  return 0;
}

// fd = open("/proc/1/ns/net",
//                 O_RDONLY);
//   if (setns(fd, 0) == -1) {
//     return -1;
//   }
