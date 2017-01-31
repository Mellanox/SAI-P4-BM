#include "sai_object.h"
StandardClient* sai_object::bm_client_ptr;
sai_id_map_t sai_object::sai_id_map;
Switch_metadata* sai_object::switch_metadata_ptr;

std::string parse_param(uint64_t param, uint32_t num_of_bytes) {
		return std::string(static_cast<char*>(static_cast<void*>(&param)),num_of_bytes);
	}

BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes) {
	BmMatchParam match_param;
	match_param.type = BmMatchParamType::type::EXACT; 
	BmMatchParamExact match_param_exact;
    match_param_exact.key = parse_param(param, num_of_bytes);
    match_param.__set_exact(match_param_exact);
    return match_param;
}


void sai_object::config_port(Port_obj port){
    BmAddEntryOptions options;
	BmMatchParams match_params;
	match_params.clear();
    match_params.push_back(parse_exact_match_param(port.hw_port,2));
    BmActionData action_data;
   	action_data.clear();
    action_data.push_back(parse_param(port.pvid,1));
    action_data.push_back(parse_param(port.bind_mode,1));
    action_data.push_back(parse_param(port.mtu,4));
    action_data.push_back(parse_param(port.drop_tagged,1));
    action_data.push_back(parse_param(port.drop_untagged,1));
    try{bm_client_ptr->bm_mt_modify_entry(cxt_id,"table_port_configurations",port.handle_port_cfg,"action_set_port_configurations",action_data);}
    catch(int e) {printf("InvalidTableOperation while removing table_port_configurations entry\n");}
	printf("port cfg modified, handle= %d\n",port.handle_port_cfg);
    
}

sai_status_t sai_object::create_port (sai_object_id_t *port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list){
	printf("create port\n");
	//printf("sai_id_map addr in create port fn :%d\n",&sai_object::sai_id_map);
	Port_obj port(sai_id_map);
	switch_metadata_ptr->ports.insert(std::make_pair(port.sai_object_id,&port));
	printf("port sai_id = %d\n",port.sai_object_id);
	//parsing attributes
	sai_attribute_t attribute;
	for(uint32_t i = 0; i < attr_count; i++) {
      attribute =attr_list[i];
      switch (attribute.id) {
      	case SAI_PORT_ATTR_PORT_VLAN_ID:
      		port.pvid = attribute.value.u16;
      	break;
      	case SAI_PORT_ATTR_BIND_MODE:
      		port.bind_mode = attribute.value.s32;
      	break;
      	case SAI_PORT_ATTR_HW_LANE_LIST:
      		port.hw_port = attribute.value.u32list.list[0];
      	break;
      }
  	}
  	printf("pvid %d, bind_mode %d, hw_port %d \n", port.pvid, port.bind_mode, port.hw_port);
    BmAddEntryOptions options;
	BmMatchParams match_params;
    BmActionData action_data;

    action_data.clear();
    match_params.clear();
    match_params.push_back(parse_exact_match_param(port.hw_port,2));
    action_data.push_back(parse_param(0,1));
    action_data.push_back(parse_param(port.sai_object_id,1));
    port.handle_lag_if = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_ingress_lag",match_params, "action_set_lag_l2if",  action_data, options);
    printf("lag handle= %d\n",port.handle_lag_if);
	*port_id = port.sai_object_id;

	action_data.clear();
    action_data.push_back(parse_param(port.pvid,1));
    action_data.push_back(parse_param(port.bind_mode,1));
    action_data.push_back(parse_param(port.mtu,4));
    action_data.push_back(parse_param(port.drop_tagged,1));
    action_data.push_back(parse_param(port.drop_untagged,1));
    port.handle_port_cfg = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_port_configurations",match_params, "action_set_port_configurations",  action_data, options);
    printf("port cfg handle= %d",port.handle_port_cfg);
	*port_id = port.sai_object_id;
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_port(sai_object_id_t port_id){
	printf("remove_port\n");
	Port_obj port = *switch_metadata_ptr->ports[port_id];
	try{
		bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_ingress_lag",port.handle_lag_if);
		bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_port_configurations",port.handle_port_cfg);
	}
	catch(int e){printf("unable to remove port tables entries\n");}
	sai_id_map.free_id(port.sai_object_id);
	switch_metadata_ptr->ports.erase(port.sai_object_id);
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_bridge (sai_object_id_t *bridge_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list){
	BmEntryHandle handle = 0;
	//parsing attributes
	sai_attribute_t attribute;
	sai_bridge_type_t type;
	for(uint32_t i = 0; i < attr_count; i++) {
      attribute =attr_list[i];
      switch (attribute.id) {
      	case SAI_BRIDGE_ATTR_TYPE:
      		type = (sai_bridge_type_t) attribute.value.s32;
      	break;
      }
	}
	Bridge_obj bridge(sai_id_map, type);
	printf("bridge sai_id = %d\n",bridge.sai_object_id);
    *bridge_id = bridge.sai_object_id;
	return SAI_STATUS_SUCCESS;
}
