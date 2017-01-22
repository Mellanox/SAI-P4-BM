#include "sai_object_interface.h"
#include "sai_objec.h"

extern "C"
{
    S_O_Handle create_sai_object() { return new sai_obect(); }
    void    free_sai_object(S_O_Handle p) { p->~sai_obect();
    										delete p; }
   	sai_status_t create_port (S_O_Handle p,sai_object_id_t *port_id, sai_object_id_t switch_id,uint32_t attr_count,const sai_attribute_t *attr_list) { return p->create_port(*port_id,switch_id,attr_count,*attr_list); }
	sai_status_t sai_obj_api_query(S_O_Handle p,sai_api_t sai_api_id,void** api_method_table){ return p->sai_api_query(sai_api_t sai_api_id,void** api_method_table); }
}