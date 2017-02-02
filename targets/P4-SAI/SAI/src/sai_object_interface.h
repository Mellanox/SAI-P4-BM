#include <sai.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void* S_O_Handle;
S_O_Handle 		create_sai_object();
void    		free_sai_object(S_O_Handle);

//api
sai_status_t 	sai_obj_api_query(S_O_Handle,sai_api_t,void**);

//port
sai_status_t 	sai_obj_create_port(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_port(S_O_Handle, sai_object_id_t);

//bridge
sai_status_t 	sai_obj_create_bridge(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_bridge(S_O_Handle, sai_object_id_t);

#ifdef __cplusplus
}
#endif