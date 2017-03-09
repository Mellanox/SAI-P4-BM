#include <sstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include "sai_object.h"
#include "../../../../thrift_src/gen-cpp/bm/standard_types.h"
#include "../../Sai-Tests/sai_thrift_src/gen-cpp/switch_sai_types.h" // TODO probably need to remove. enum issue with SAI_FDB_ENTRY_ATTR_PACKET_ACTION
StandardClient* sai_object::bm_client_ptr;
sai_id_map_t* sai_object::sai_id_map_ptr;
Switch_metadata* sai_object::switch_metadata_ptr;

std::string parse_param(uint64_t param, uint32_t num_of_bytes) {
		std::string my_string = std::string(static_cast<char*>(static_cast<void*>(&param)),num_of_bytes);
		std::reverse(my_string.begin(),my_string.end());
		return my_string;
	}

BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes) {
	BmMatchParam match_param;
	match_param.type = BmMatchParamType::type::EXACT; 
	BmMatchParamExact match_param_exact;
    match_param_exact.key = parse_param(param, num_of_bytes);
    match_param.__set_exact(match_param_exact);
    return match_param;
}

sai_status_t sai_object::create_switch(sai_object_id_t* switch_id, uint32_t attr_count, const sai_attribute_t *attr_list) {
	BmAddEntryOptions options;
  	BmMatchParams match_params;
  	BmMtEntry entry;

	// Create Default 1Q Bridge, and switch_obj (not sure if switch is needed).
    Bridge_obj *bridge = new Bridge_obj(sai_id_map_ptr);
    printf("Default 1Q bridge. sai_object_id %d bridge_id %d\n", bridge->sai_object_id, bridge->bridge_id);
    Sai_obj *switch_obj = new Sai_obj(sai_id_map_ptr);
	switch_metadata_ptr->bridges[bridge->sai_object_id] = bridge;
	switch_metadata_ptr->default_bridge_id = bridge->sai_object_id;

    for (int i=0; i<switch_metadata_ptr->hw_port_list.count; i++) {
    	int hw_port = switch_metadata_ptr->hw_port_list.list[i];

    	// Create Default Ports (one for each hw_port)
    	Port_obj *port = new Port_obj(sai_id_map_ptr);
		switch_metadata_ptr->ports[port->sai_object_id] = port;
		port->hw_port = hw_port;
		port->l2_if = hw_port;
		printf("Defulat port_id %d. hw_port = %d\n", port->sai_object_id, port->hw_port);

		// Create Default Bridge ports and connect to 1Q bridge
		BridgePort_obj *bridge_port = new BridgePort_obj(sai_id_map_ptr);
		switch_metadata_ptr->bridge_ports[bridge_port->sai_object_id] = bridge_port;		
		bridge_port->port_id = port->sai_object_id;
		bridge_port->bridge_port = hw_port;
		bridge_port->bridge_id = bridge->sai_object_id;
		bridge->bridge_port_list.push_back(bridge_port->sai_object_id);
		printf("Defulat bridge_port_id %d. bridge_port = %d\n", bridge_port->sai_object_id, bridge_port->bridge_port);

		// Store default table entries
		match_params.clear();
	  	match_params.push_back(parse_exact_match_param(port->l2_if,1));
		bm_client_ptr->bm_mt_get_entry_from_key(entry, cxt_id,"table_port_ingress_interface_type",match_params, options);
		bridge_port->handle_port_ingress_interface_type = entry.entry_handle;
	  	match_params.clear();
  		match_params.push_back(parse_exact_match_param(bridge_port->bridge_port,1));
		bm_client_ptr->bm_mt_get_entry_from_key(entry,cxt_id,"table_egress_br_port_to_if",match_params, options);
		bridge_port->handle_egress_br_port_to_if = entry.entry_handle;

    }
    return switch_obj->sai_object_id;
}

sai_status_t sai_object::get_switch_attribute(sai_object_id_t switch_id, sai_uint32_t attr_count, sai_attribute_t *attr_list) {
	int i;
	int index = 0;
	for (i=0;i<attr_count;i++) {
		switch ((attr_list+i)->id) {
		case SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID:
			(attr_list+i)->value.oid = switch_metadata_ptr->default_bridge_id;
      		break;
		case SAI_SWITCH_ATTR_PORT_LIST:
			for (port_id_map_t::iterator it=switch_metadata_ptr->ports.begin(); it!=switch_metadata_ptr->ports.end(); ++it) {
    			(attr_list+i)->value.objlist.list[index] = it->first;
    			index +=1;
			}
			break;
		case SAI_SWITCH_ATTR_PORT_NUMBER:
			(attr_list+i)->value.u32 = switch_metadata_ptr->hw_port_list.count;
			break;
		}
	}
  return SAI_STATUS_SUCCESS;
}

void sai_object::set_parsed_port_attribute(Port_obj* port, sai_attribute_t attribute) {
	switch (attribute.id) {
     	case SAI_PORT_ATTR_PORT_VLAN_ID:
     		port->pvid = attribute.value.u16;
     		break;
     	case SAI_PORT_ATTR_BIND_MODE:
     		port->bind_mode = attribute.value.s32;
     		break;
     	case SAI_PORT_ATTR_HW_LANE_LIST:
     		port->hw_port = attribute.value.u32list.list[0];
     		break;
     	case SAI_PORT_ATTR_DROP_UNTAGGED:
     		port->drop_untagged = attribute.value.booldata;
     		break;
     	case SAI_PORT_ATTR_DROP_TAGGED:
     		port->drop_tagged = attribute.value.booldata;
     		break;
    }
}

void sai_object::get_parsed_port_attribute(Port_obj* port, sai_attribute_t *attribute) {
	switch (attribute->id) {
     	case SAI_PORT_ATTR_PORT_VLAN_ID:
     		attribute->value.u16 = port->pvid;
     		break;
     	case SAI_PORT_ATTR_BIND_MODE:
     		attribute->value.s32 = port->bind_mode;
     		break;
     	case SAI_PORT_ATTR_HW_LANE_LIST:
     		attribute->value.u32list.count = 1;
     		attribute->value.u32list.list[0] = port->hw_port;
     		break;
     	case SAI_PORT_ATTR_DROP_UNTAGGED:
     		attribute->value.booldata = port->drop_untagged;
     		break;
     	case SAI_PORT_ATTR_DROP_TAGGED:
     		attribute->value.booldata = port->drop_tagged;
     		break;
    }
}

void sai_object::config_port(Port_obj* port){
  BmAddEntryOptions options;
  BmMatchParams match_params;
  match_params.clear();
  match_params.push_back(parse_exact_match_param(port->l2_if,1));
  BmActionData action_data;
  action_data.clear();
  action_data.push_back(parse_param(port->pvid,2));
  action_data.push_back(parse_param(port->bind_mode,1));
  action_data.push_back(parse_param(port->mtu,4));
  action_data.push_back(parse_param(port->drop_tagged,1));
  action_data.push_back(parse_param(port->drop_untagged,1));
  if (port->is_default) {
  	port->handle_port_cfg = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_port_configurations",match_params, "action_set_port_configurations",  action_data, options);
  	port->is_default = false;
  } else {
  	try{bm_client_ptr->bm_mt_modify_entry(cxt_id,"table_port_configurations",port->handle_port_cfg,"action_set_port_configurations",action_data);}
  	catch(int e) {printf("InvalidTableOperation while removing table_port_configurations entry\n");}
  }
  // printf("port cfg modified, handle= %d\n",port->handle_port_cfg);
}

sai_status_t sai_object::set_port_attribute(sai_object_id_t port_id, const sai_attribute_t *attr) {
	Port_obj* port = (Port_obj*) sai_id_map_ptr->get_object(port_id);
	set_parsed_port_attribute(port, *attr);
	config_port(port);
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::get_port_attribute(sai_object_id_t port_id, uint32_t attr_count, sai_attribute_t *attr_list) {
	Port_obj* port = (Port_obj*) sai_id_map_ptr->get_object(port_id);
	for (int i=0;i<attr_count;i++) {
		get_parsed_port_attribute(port, attr_list+i);
	}
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_port (sai_object_id_t *port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list){
	//printf("sai_id_map addr in create port fn :%d\n",&sai_object::sai_id_map);
	Port_obj *port = new Port_obj(sai_id_map_ptr);
	//printf("switch_metadata_ptr->ports.size = %d\n",switch_metadata_ptr->ports.size());
	switch_metadata_ptr->ports[port->sai_object_id] = port;
	// switch_metadata_ptr->ports.insert(std::make_pair(port->sai_object_id,&port));
	//printf("new port sai_id = %d\n",port->sai_object_id);
	//parsing attributes
	sai_attribute_t attribute;
	for(uint32_t i = 0; i < attr_count; i++) {
     	attribute =attr_list[i];
     	set_parsed_port_attribute(port, attribute);
  	}
  	//printf("pvid %d, bind_mode %d, hw_port %d \n", port->pvid, port->bind_mode, port->hw_port);
  	BmAddEntryOptions options;
  	BmMatchParams match_params;
  	BmActionData action_data;
  	action_data.clear();
  	match_params.clear();
  	match_params.push_back(parse_exact_match_param(port->hw_port,2));
  	action_data.push_back(parse_param(0,1));
  	action_data.push_back(parse_param(port->sai_object_id,1));
  	port->handle_lag_if = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_ingress_lag",match_params, "action_set_lag_l2if",  action_data, options);
  	//printf("lag handle= %d, ",port->handle_lag_if);
  	// config_port(port); Todo: Change to this
  	action_data.clear();
  	match_params.clear();
  	match_params.push_back(parse_exact_match_param(port->sai_object_id,1));
  	action_data.push_back(parse_param(port->pvid,2));
  	action_data.push_back(parse_param(port->bind_mode,1));
  	action_data.push_back(parse_param(port->mtu,4));
  	action_data.push_back(parse_param(port->drop_tagged,1));
  	action_data.push_back(parse_param(port->drop_untagged,1));
  	port->handle_port_cfg = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_port_configurations",match_params, "action_set_port_configurations",  action_data, options);
  	port->is_default = false;
  	//printf("port cfg handle= %d\n",port->handle_port_cfg);
	*port_id = port->sai_object_id;
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_port(sai_object_id_t port_id){
	Port_obj* port = switch_metadata_ptr->ports[port_id];
	try{
		bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_ingress_lag",port->handle_lag_if);
		bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_port_configurations",port->handle_port_cfg);
//		printf("deleted port bm_entries\n");
	}
	catch(int e){printf("unable to remove port tables entries\n");}
	switch_metadata_ptr->ports.erase(port->sai_object_id);
	sai_id_map_ptr->free_id(port->sai_object_id);
	//printf("ports.size=%d\n",switch_metadata_ptr->ports.size());
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_bridge (sai_object_id_t *bridge_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list){
	uint32_t bridge_id_num = switch_metadata_ptr->GetNewBridgeID();
	Bridge_obj *bridge = new Bridge_obj(sai_id_map_ptr);
	switch_metadata_ptr->bridges[bridge->sai_object_id] = bridge;
	bridge->bridge_id = bridge_id_num;
	printf("Created new bridge %d (sai_object_id=%d)\n", bridge->bridge_id, bridge->sai_object_id);
	//parsing attributes
	sai_attribute_t attribute;
	for(uint32_t i = 0; i < attr_count; i++) {
     attribute =attr_list[i];
     switch (attribute.id) {
     	case SAI_BRIDGE_ATTR_TYPE:
     		bridge->bridge_type = (sai_bridge_type_t) attribute.value.s32;
     	break;
     }
    }
    *bridge_id = bridge->sai_object_id;
    return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_bridge(sai_object_id_t bridge_id){
	Bridge_obj* bridge = switch_metadata_ptr->bridges[bridge_id];
	switch_metadata_ptr->bridges.erase(bridge->sai_object_id);
	sai_id_map_ptr->free_id(bridge->sai_object_id);
	//printf("bridges.size=%d\n",switch_metadata_ptr->bridges.size());
	return SAI_STATUS_SUCCESS;
}



sai_status_t sai_object::create_bridge_port (sai_object_id_t *bridge_port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list){
	uint32_t bridge_port_num = switch_metadata_ptr->GetNewBridgePort();
	BridgePort_obj *bridge_port = new BridgePort_obj(sai_id_map_ptr);
	switch_metadata_ptr->bridge_ports[bridge_port->sai_object_id] = bridge_port;
	bridge_port->bridge_port = bridge_port_num;
	sai_attribute_t attribute;
	for(uint32_t i = 0; i < attr_count; i++) {
      attribute =attr_list[i];
      switch (attribute.id) {
     	case SAI_BRIDGE_PORT_ATTR_VLAN_ID:
     		bridge_port->vlan_id = attribute.value.u16;
     		break;
     	case SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
     		bridge_port->bridge_id = attribute.value.oid;
     		break;
     	case SAI_BRIDGE_PORT_ATTR_TYPE:
     		bridge_port->bridge_port_type = (sai_bridge_port_type_t) attribute.value.s32;
     		break;
     	case SAI_BRIDGE_PORT_ATTR_PORT_ID:
     		bridge_port->port_id = attribute.value.oid;
     		break;
      }
    }
    BmAddEntryOptions options;
    BmMatchParams match_params;
    BmActionData action_data;
    match_params.clear();
    action_data.clear();
    int32_t l2_if_type;
    uint32_t bridge_id = switch_metadata_ptr->bridges[bridge_port->bridge_id]->bridge_id;
    // 1D
    if(bridge_port->bridge_port_type==SAI_BRIDGE_PORT_TYPE_SUB_PORT){ 
    	match_params.push_back(parse_exact_match_param(bridge_port->bridge_port,1));
    	action_data.push_back(parse_param(bridge_id,2));
    	bridge_port->handle_id_1d = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_bridge_id_1d",match_params, "action_set_bridge_id",  action_data, options);
    	action_data.clear();
    	action_data.push_back(parse_param(bridge_port->vlan_id,2));
    	bridge_port->handle_egress_set_vlan = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_egress_set_vlan",match_params, "action_set_vlan",  action_data, options);
    	l2_if_type=2;
    }
    // 1Q
    else{
    	match_params.clear();
    	match_params.push_back(parse_exact_match_param(bridge_port->vlan_id,2));
    	action_data.clear();
    	action_data.push_back(parse_param(bridge_id,2));
    	try { // TODO: This needs to be done only once per 1Q bridge, maybe a better solution then try?
  			bridge_port->handle_id_1q = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_bridge_id_1q",match_params, "action_set_bridge_id",  action_data, options);
  		} catch (...) { }
		l2_if_type=3;
    }
    // TODO add lag check here
  	//if port_id in self.lags.keys(): # LAG
      // self.cli_client.AddTable('table_egress_br_port_to_if', 'action_forward_set_outIfType', str(br_port), list_to_str([port_id, 1]))
      // bind_mode = self.ports[self.lag_members[self.lags[port_id].lag_members[0]].port_id].bind_mode
    //else: # port

	match_params.clear();
  	match_params.push_back(parse_exact_match_param(bridge_port->bridge_port,1));
  	action_data.clear();
  	Port_obj* port = switch_metadata_ptr->ports[bridge_port->port_id];
  	action_data.push_back(parse_param(port->l2_if,1));
  	action_data.push_back(parse_param(0,1));
  	// action_data.push_back(parse_param(999999,3));
  	// action_data.push_back(parse_param(55,3));
  	// std::cout << "table_egress_br_port_to_if : " <<match_params[0] << endl;
  	// std::cout << "action 0 "<< action_data[0] << endl;
  	// std::cout << "action 1 "<< action_data[1] << endl;
  	bridge_port->handle_egress_br_port_to_if = bm_client_ptr->bm_mt_add_entry(cxt_id,"table_egress_br_port_to_if",match_params, "action_forward_set_outIfType",  action_data, options);
    uint32_t bind_mode = port->bind_mode;
    if (bind_mode == SAI_PORT_BIND_MODE_SUB_PORT){
		match_params.clear();
		match_params.push_back(parse_exact_match_param(port->l2_if,1));// TODO p4 table match is on l2_if
	  	match_params.push_back(parse_exact_match_param(bridge_port->vlan_id,2));
	  	action_data.clear();
	  	action_data.push_back(parse_param(l2_if_type,1));
	  	action_data.push_back(parse_param(bridge_port->bridge_port,1));
	    bridge_port->handle_subport_ingress_interface_type=bm_client_ptr->bm_mt_add_entry(cxt_id,"table_subport_ingress_interface_type",match_params, "action_set_l2_if_type",  action_data, options);
  	}
    else{
		match_params.clear();
	  	match_params.push_back(parse_exact_match_param(port->l2_if,1));
		action_data.clear();
		action_data.push_back(parse_param(l2_if_type,1));
		action_data.push_back(parse_param(bridge_port->bridge_port,1));
    	bridge_port->handle_port_ingress_interface_type=bm_client_ptr->bm_mt_add_entry(cxt_id,"table_port_ingress_interface_type",match_params, "action_set_l2_if_type",  action_data, options);
    }
    *bridge_port_id = bridge_port->sai_object_id;
    return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_bridge_port(sai_object_id_t bridge_port_id){
	sai_status_t status = SAI_STATUS_SUCCESS;
	BridgePort_obj* bridge_port = switch_metadata_ptr->bridge_ports[bridge_port_id];

	try{
		if (bridge_port->handle_id_1d != 999){
			bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_bridge_id_1d",bridge_port->handle_id_1d);
		}
		if (bridge_port->handle_id_1q != 999){
			bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_bridge_id_1q",bridge_port->handle_id_1q);
		}
		if (bridge_port->handle_egress_set_vlan != 999){
			bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_egress_set_vlan",bridge_port->handle_egress_set_vlan);
		}
		if (bridge_port->handle_egress_br_port_to_if != 999){
			bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_egress_br_port_to_if",bridge_port->handle_egress_br_port_to_if);
		}
		if (bridge_port->handle_subport_ingress_interface_type != 999){
			bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_subport_ingress_interface_type",bridge_port->handle_subport_ingress_interface_type);
		}
		if (bridge_port->handle_port_ingress_interface_type != 999){
			bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_port_ingress_interface_type",bridge_port->handle_port_ingress_interface_type);
		}
		printf("deleted bridge port %d bm_entries\n",bridge_port->sai_object_id);
	}
	catch(int e){
		status = SAI_STATUS_FAILURE;
		printf("unable to remove bridge port tables entries\n");
	}
	switch_metadata_ptr->bridge_ports.erase(bridge_port->sai_object_id);
	sai_id_map_ptr->free_id(bridge_port->sai_object_id);
	//printf("ports.size=%d\n",switch_metadata_ptr->ports.size());
	return status;
}

sai_status_t sai_object::get_bridge_port_attribute(sai_object_id_t bridge_port_id, uint32_t attr_count, sai_attribute_t *attr_list) {
	BridgePort_obj* bridge_port = (BridgePort_obj*) sai_id_map_ptr->get_object(bridge_port_id);
	for (int i=0; i<attr_count; i++) {
		switch ((attr_list+i)->id) {
			case SAI_BRIDGE_PORT_ATTR_PORT_ID:
				(attr_list+i)->value.oid = bridge_port->port_id;
				break;
			case SAI_BRIDGE_PORT_ATTR_VLAN_ID:
				(attr_list+i)->value.u16 = bridge_port->vlan_id;
				break;
			case SAI_BRIDGE_PORT_ATTR_TYPE:
				(attr_list+i)->value.s32 = bridge_port->bridge_port_type;
				break;
			case SAI_BRIDGE_PORT_ATTR_BRIDGE_ID:
				(attr_list+i)->value.oid = bridge_port->bridge_port_type;
				break;
		}
	}
}

sai_status_t sai_object::get_bridge_attribute(sai_object_id_t bridge_id, uint32_t attr_count, sai_attribute_t *attr_list) {
	int i;
	Bridge_obj* bridge = (Bridge_obj*) sai_id_map_ptr->get_object(bridge_id);
	for (i=0; i<attr_count; i++) {
		switch ((attr_list+i)->id) {
			case SAI_BRIDGE_ATTR_TYPE:
				(attr_list+i)->value.s32 = bridge->bridge_type;
				break;
			case SAI_BRIDGE_ATTR_PORT_LIST:
				(attr_list+i)->value.objlist.count = bridge->bridge_port_list.size();
				(attr_list+i)->value.objlist.list = &bridge->bridge_port_list[0];
				break;
		}
	}
}

uint64_t parse_mac_64(uint8_t const mac_8[6])
  {
    uint64_t mac_64 = 0;
    memcpy(&mac_64, mac_8, 6);
    return mac_64;
  }

sai_status_t sai_object::create_fdb_entry(const sai_fdb_entry_t *fdb_entry,uint32_t attr_count,const sai_attribute_t *attr_list){
	sai_status_t status = SAI_STATUS_SUCCESS;
	//parsing attributes
	
	uint32_t entry_type;
	uint32_t bridge_port;
	uint32_t packet_action;
	sai_attribute_t attribute;
	for(uint32_t i = 0; i < attr_count; i++) {
    	attribute =attr_list[i];
    	switch (attribute.id) {
	        case SAI_FDB_ENTRY_ATTR_TYPE:
                entry_type = attribute.value.s32;
                //std::cout << "--> attr packet type="<<attribute.value.s32<<endl;
                //std::cout << "--> attr packet_static" << SAI_FDB_ENTRY_TYPE_STATIC <<endl;
                break;
             case SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID:
                bridge_port = switch_metadata_ptr->bridge_ports[attribute.value.oid]->bridge_port;
                break;
             case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
                packet_action = attribute.value.s32;
                //std::cout << "--> attr packet_action="<<attribute.value.s32<<endl;
                //std::cout << "--> attr packet_action_fwd=" << SAI_PACKET_ACTION_FORWARD <<endl;
                break;
             default:
                std::cout << "attribute.id = " << attribute.id << "was dumped in sai_obj" << endl; 
                break;
    	}
    }
	//out_if_type = 0 # port_type (not lag or router). TODO: check how to do it with SAI
	uint32_t bridge_id = switch_metadata_ptr->bridges[fdb_entry->bridge_id]->bridge_id;
	if (packet_action == SAI_PACKET_ACTION_FORWARD){
		std::cout << "SAI_PACKET_ACTION_FORWARD" << endl;
	      if (entry_type == SAI_FDB_ENTRY_TYPE_STATIC){
	      	std::cout << "SAI_FDB_ENTRY_TYPE_STATIC" << endl;
	   		BmAddEntryOptions options;
	  		BmMatchParams match_params;
	  		BmActionData action_data;
			uint64_t mac_address = parse_mac_64(fdb_entry->mac_address);
			match_params.push_back(parse_exact_match_param(mac_address,6));
			match_params.push_back(parse_exact_match_param(bridge_id,2));
			action_data.push_back(parse_param(bridge_port,1));
	   		BmEntryHandle handle;
	   		bm_client_ptr->bm_mt_add_entry(cxt_id,"table_fdb",match_params, "action_set_egress_br_port",  action_data, options);
	      }
  	}
	return status;
}

sai_status_t sai_object::remove_fdb_entry(const sai_fdb_entry_t *fdb_entry){
	sai_status_t status = SAI_STATUS_SUCCESS;
	BmAddEntryOptions options;
	BmMatchParams match_params;
	BmActionData action_data;
	uint64_t mac_address = parse_mac_64(fdb_entry->mac_address);
	match_params.push_back(parse_exact_match_param(mac_address,6));
	uint32_t bridge_id = switch_metadata_ptr->bridges[fdb_entry->bridge_id]->bridge_id;
	match_params.push_back(parse_exact_match_param(bridge_id,2));

	BmMtEntry bm_entry;
	try{
	bm_client_ptr->bm_mt_get_entry_from_key(bm_entry,cxt_id,"table_fdb",match_params,options);
	} catch (int e) {
		printf("Unable to delete table. possible key missmatch:%d\n",mac_address);
		return SAI_STATUS_FAILURE;
	};
	bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_fdb",bm_entry.entry_handle);
	return status;
}


sai_status_t sai_object::create_vlan (sai_object_id_t *vlan_id , sai_object_id_t switch_id, uint32_t attr_count,const sai_attribute_t *attr_list){
	Vlan_obj *vlan = new Vlan_obj(sai_id_map_ptr);
	switch_metadata_ptr->vlans[vlan->sai_object_id] = vlan;
	//parsing attributes
	sai_attribute_t attribute;
	for(uint32_t i = 0; i < attr_count; i++) {
      attribute = attr_list[i];
      switch (attribute.id) {
     	case SAI_VLAN_ATTR_VLAN_ID:
     		vlan->vid = (uint32_t) attribute.value.u16; // TODO correct casting type
     	break;
      }
    }
    *vlan_id = vlan->sai_object_id;
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_vlan(sai_object_id_t vlan_id){
	Vlan_obj* vlan = switch_metadata_ptr->vlans[vlan_id];
	switch_metadata_ptr->vlans.erase(vlan->sai_object_id);
	sai_id_map_ptr->free_id(vlan->sai_object_id);
	//printf("vlans.size=%d\n",switch_metadata_ptr->vlans.size());
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::create_vlan_member (sai_object_id_t *vlan_member_id , sai_object_id_t switch_id, uint32_t attr_count,const sai_attribute_t *attr_list){
	Vlan_member_obj *vlan_member = new Vlan_member_obj(sai_id_map_ptr);
	switch_metadata_ptr->vlan_members[vlan_member->sai_object_id] = vlan_member;
	//parsing attributes
	sai_attribute_t attribute;
	for(uint32_t i = 0; i < attr_count; i++) {
     	attribute =attr_list[i];
     	switch (attribute.id) {
	     	case SAI_VLAN_MEMBER_ATTR_VLAN_ID:
	     		vlan_member->vlan_oid = (sai_object_id_t) attribute.value.oid;
	     		break;
	     	case SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID:
	     		vlan_member->bridge_port_id = (sai_object_id_t) attribute.value.oid;
	     		break;
	     	case SAI_VLAN_MEMBER_ATTR_VLAN_TAGGING_MODE:
	     		vlan_member->tagging_mode = (uint32_t) attribute.value.s32;
	     		break;
	     	default:
		     	std::cout << "while parsing vlan member, attribute.id = " << attribute.id << "was dumped in sai_obj" << endl; 
	        	break;
	        }
    }
    Vlan_obj* vlan = switch_metadata_ptr->vlans[vlan_member->vlan_oid];
    vlan_member->vid = vlan->vid;
    vlan->vlan_members.push_back(vlan_member->sai_object_id);
    uint32_t port_id=bridge_ports[vlan_member->bridge_port_id];
    if(lags.find(port_id)==lags.end()){
    	uint32_t out_if = lags[port_id]->l2_if;
    }
    else{
    	uint32_t out_if = ports[port_id]->hw_port;
    }
    BmAddEntryOptions options;
	BmMatchParams match_params;
	BmActionData action_data;
    if(vlan_member->tagging_mode == SAI_VLAN_TAGGING_MODE_TAGGED){
    	uint32_t vlan_pcp = 0;
    	uint32_t vlan_cfi = 0;
		match_params.push_back(parse_exact_match_param(out_if,1));
		match_params.push_back(parse_exact_match_param(vlan_member->vid,2));
		match_params.push_back(parse_exact_match_param(0,1));
		action_data.push_back(parse_param(bridge_port,1));
   		vlan_member->handle_egress_vlan_tag =
   			bm_client_ptr->bm_mt_add_entry(cxt_id,"table_egress_vlan_lag",match_params, "action_forward_vlan_tag",  action_data, options);
    }
    else if (vlan_member->tagging_mode == SAI_VLAN_TAGGING_MODE_PRIORITY_TAGGED) {
    	uint32_t vlan_pcp = 0;
    	uint32_t vlan_cfi = 0;
    	match_params.push_back(parse_exact_match_param(out_if,1));
		match_params.push_back(parse_exact_match_param(vlan_member->vid,2));
		match_params.push_back(parse_exact_match_param(0,1));
		action_data.push_back(parse_param(bridge_port,1));
		vlan_member->handle_egress_vlan_tag =
   			bm_client_ptr->bm_mt_add_entry(cxt_id,"table_egress_vlan_lag",match_params, "action_forward_vlan_tag",  action_data, options);
    }
    else{
    	match_params.push_back(parse_exact_match_param(out_if,1));
		match_params.push_back(parse_exact_match_param(vlan_member->vid,2));
		match_params.push_back(parse_exact_match_param(1,1));
		vlan_member->handle_egress_vlan_tag =
   			bm_client_ptr->bm_mt_add_entry(cxt_id,"table_egress_vlan_lag",match_params, "action_forward_vlan_untag",  action_data, options);	
    }
    match_params.clear();
    match_params.push_back(parse_exact_match_param(bridge_port,1));
	match_params.push_back(parse_exact_match_param(vlan_member->vid,2));
    action_data.clear();
    vlan_member->handle_egress_vlan_filtering =
    	bm_client_ptr->bm_mt_add_entry(cxt_id,"table_egress_vlan_filtering",match_params, "_nop",  action_data, options);	
    vlan_member->handle_egress_vlan_filtering =
    	bm_client_ptr->bm_mt_add_entry(cxt_id,"table_ingress_vlan_filtering",match_params, "_nop",  action_data, options);	
	*vlan_id = bridge->sai_object_id;
	return SAI_STATUS_SUCCESS;
}

sai_status_t sai_object::remove_vlan_member (sai_object_id_t vlan_member_id) {
	printf("remove vlan_member: vlan_id = %d\n",vlan_member_id);
	sai_status_t status = SAI_STATUS_SUCCESS;
	Vlan_member_obj *vlan_member = vlan_members[vlan_member_id];
	BmAddEntryOptions options;
	BmMatchParams match_params;
	BmActionData action_data;
	match_params.push_back(parse_exact_match_param(mac_address,6));
	match_params.push_back(parse_exact_match_param(fdb_entry->bridge_id,2));

	BmMtEntry bm_entry;
	try{
		bm_client_ptr->bm_mt_get_entry_from_key(bm_entry,cxt_id,"table_fdb",match_params,options);
	} 
	catch (int e) {
		printf("Unable to delete table. possible key missmatch:%d\n",mac_address);
		return SAI_STATUS_FAILURE;
	};
	//std::cout << "got entry handle"<< endl;
	std::cout << "got entry handle: " << bm_entry.entry_handle <<endl;
	bm_client_ptr->bm_mt_delete_entry(cxt_id,"table_fdb",bm_entry.entry_handle);
	return status;


}


sai_status_t sai_object::set_vlan_attribute (sai_object_id_t vlan_id, const sai_attribute_t *attr) {
	// implementation
}
sai_status_t sai_object::get_vlan_attribute (sai_object_id_t vlan_id, const uint32_t attr_count, sai_attribute_t *attr_list) {
	// implementation
}
sai_status_t sai_object::remove_vlan_member (sai_object_id_t vlan_member_id) {
	// implementation
}
sai_status_t sai_object::set_vlan_member_attribute (sai_object_id_t vlan_member_id, const sai_attribute_t *attr) {
	// implementation
}
sai_status_t sai_object::get_vlan_member_attribute (sai_object_id_t vlan_member_id, const uint32_t attr_count, sai_attribute_t *attr_list) {
	// implementation
}
sai_status_t sai_object::get_vlan_stats (sai_object_id_t vlan_id, const sai_vlan_stat_t *counter_ids, uint32_t number_of_counters, uint64_t *counters) {
	// implementation
}
sai_status_t sai_object::clear_vlan_stats (sai_object_id_t vlan_id, const sai_vlan_stat_t *counter_ids, uint32_t number_of_counters) {
	// implementation
}