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
sai_status_t    sai_object_create_switch(S_O_Handle, sai_object_id_t*, uint32_t, const sai_attribute_t*);
sai_status_t    sai_object_get_switch_attribute(S_O_Handle, sai_object_id_t, sai_uint32_t, sai_attribute_t*);

//port
sai_status_t 	sai_obj_create_port(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_port(S_O_Handle, sai_object_id_t);
sai_status_t    sai_obj_set_port_attribute(S_O_Handle, sai_object_id_t port_id, sai_attribute_t*);
sai_status_t    sai_obj_get_port_attribute(S_O_Handle, sai_object_id_t port_id, uint32_t, sai_attribute_t*);

//bridge
sai_status_t 	sai_obj_create_bridge(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_bridge(S_O_Handle, sai_object_id_t);
sai_status_t    get_bridge_attribute(S_O_Handle, sai_object_id_t bridge_id, uint32_t, sai_attribute_t*);

//bridge_port
sai_status_t 	sai_obj_create_bridge_port(S_O_Handle, sai_object_id_t*, sai_object_id_t,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_bridge_port(S_O_Handle, sai_object_id_t);
sai_status_t    get_bridge_port_attribute(S_O_Handle, sai_object_id_t bridge_port_id, uint32_t, sai_attribute_t*);

// FDB
sai_status_t 	sai_obj_create_fdb_entry(S_O_Handle, sai_fdb_entry_t*,uint32_t,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_fdb_entry(S_O_Handle, sai_fdb_entry_t*);

// VLAN
sai_status_t 	sai_obj_create_vlan(S_O_Handle ,sai_object_id_t * , sai_object_id_t , uint32_t ,const sai_attribute_t*);
sai_status_t 	sai_obj_remove_vlan(S_O_Handle ,sai_object_id_t);
sai_status_t 	sai_obj_set_vlan_attribute(S_O_Handle ,sai_object_id_t , const sai_attribute_t *);
sai_status_t 	sai_obj_get_vlan_attribute(S_O_Handle ,sai_object_id_t , const uint32_t , sai_attribute_t *);
sai_status_t 	sai_obj_create_vlan_member(S_O_Handle ,sai_object_id_t* , sai_object_id_t, uint32_t, const sai_attribute_t*);
sai_status_t 	sai_obj_remove_vlan_member(S_O_Handle ,sai_object_id_t);
sai_status_t 	sai_obj_set_vlan_member_attribute(S_O_Handle ,sai_object_id_t , const sai_attribute_t *);
sai_status_t 	sai_obj_get_vlan_member_attribute(S_O_Handle ,sai_object_id_t , const uint32_t , sai_attribute_t *);
sai_status_t 	sai_obj_get_vlan_stats(S_O_Handle ,sai_object_id_t , const sai_vlan_stat_t *, uint32_t , uint64_t *);
sai_status_t 	sai_obj_clear_vlan_stats(S_O_Handle ,sai_object_id_t , const sai_vlan_stat_t *, uint32_t);

// LAG
sai_status_t 	sai_obj_create_lag(S_O_Handle, sai_object_id_t *,sai_object_id_t, uint32_t,const sai_attribute_t *);
sai_status_t 	sai_obj_remove_lag(S_O_Handle, sai_object_id_t);
sai_status_t 	sai_obj_create_lag_member(S_O_Handle, sai_object_id_t *,sai_object_id_t ,uint32_t ,const sai_attribute_t *);
sai_status_t 	sai_obj_remove_lag_member(S_O_Handle, sai_object_id_t);


#ifdef __cplusplus
}
#endif