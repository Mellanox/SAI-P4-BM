#include "sai_object.h"
StandardClient* sai_object::bm_client_ptr;
sai_id_map_t* sai_object::sai_id_map_ptr;

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

	sai_status_t sai_object::create_port (sai_object_id_t *port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list){
		printf("create port");
		Port_obj port(*sai_id_map_ptr);
		printf("port sai_id = %d\n",port.sai_object_id);
		BmEntryHandle handle = 0;
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

        match_params.push_back(parse_exact_match_param(port.sai_object_id,1));
        action_data.push_back(parse_param(port.pvid,1));
        handle=bm_client_ptr->bm_mt_add_entry(cxt_id,"table_port_PVID",match_params, "action_set_pvid" ,action_data, options);
		printf("pvid handle= %d\n",handle);


		action_data.clear();
		action_data.push_back(parse_param(port.bind_mode,1));
        handle=bm_client_ptr->bm_mt_add_entry(cxt_id,"table_port_mode",match_params,  "action_set_port_mode", action_data, options);
		printf("port mode handle= %d\n",handle);


        action_data.clear();
        match_params.clear();
        match_params.push_back(parse_exact_match_param(port.hw_port,2));
        action_data.push_back(parse_param(0,1));
        action_data.push_back(parse_param(port.sai_object_id,1));
        handle=bm_client_ptr->bm_mt_add_entry(cxt_id,"table_ingress_lag",match_params, "action_set_lag_l2if",  action_data, options);
        printf("lag handle= %d",handle);
		*port_id = port.sai_object_id;

		return SAI_STATUS_SUCCESS;
	}