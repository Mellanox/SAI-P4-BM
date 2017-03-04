#include <sai.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void* S_O_Handle;
S_O_Handle 		create_sai_object();
void    		free_sai_object(S_O_Handle);

//api
sai_status_t 	sai_obj_api_query(S_O_Handle,sai_api_t,void**);

//switch
sai_status_t    sai_object_create_switch(S_O_Handle p, sai_object_id_t* switch_id, uint32_t attr_count, const sai_attribute_t *attr_list);

//port
sai_status_t 	sai_obj_create_port(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_port(S_O_Handle, sai_object_id_t);
sai_status_t    sai_obj_set_port_attribute(S_O_Handle p, sai_object_id_t port_id, sai_attribute_t *attr);

//bridge
sai_status_t 	sai_obj_create_bridge(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_bridge(S_O_Handle, sai_object_id_t);

//bridge_port
sai_status_t 	sai_obj_create_bridge_port(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_bridge_port(S_O_Handle, sai_object_id_t);

// FDB
sai_status_t 	sai_obj_create_fdb_entry(S_O_Handle, sai_fdb_entry_t*,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_fdb_entry(S_O_Handle, sai_fdb_entry_t*);


#ifdef __cplusplus
}
#endif