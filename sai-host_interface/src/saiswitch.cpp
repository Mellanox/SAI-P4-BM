#include "saiswitch.h";
sai_status_t sai_get_switch_attribute(sai_object_id_t switch_id,sai_uint32_t attr_count,sai_attribute_t *attr_list){

}

    // hw_port_list = sai_thrift_u32_list_t(u32list=self.hw_port_list, count=len(self.hw_port_list))
    // for attr in thrift_attr_list:
    //   if attr.id == sai_switch_attr.SAI_SWITCH_ATTR_PORT_LIST:
    //     attr.value = sai_thrift_attribute_value_t(u32list=hw_port_list)
    //     new_attr = sai_thrift_attribute_t(id=attr.id, value=attr.value)
    // new_attr_list = sai_thrift_attribute_list_t(attr_list = [new_attr], attr_count=1)
    // return new_attr_list