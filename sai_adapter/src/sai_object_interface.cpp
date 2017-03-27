#include "sai_object_interface.h"
#include "sai_object.h"
#include "switch_meta_data.h"

extern "C" {
S_O_Handle create_sai_object() { return new sai_object(); }
void free_sai_object(S_O_Handle p) {
  sai_object *q = (sai_object *)p;
  q->~sai_object();
  delete p;
}

// API
sai_status_t sai_obj_api_query(S_O_Handle p, sai_api_t sai_api_id,
                               void **api_method_table) {
  sai_object *q = (sai_object *)p;
  return q->sai_api_query(sai_api_id, api_method_table);
}
// SWITCH
sai_status_t sai_object_create_switch(S_O_Handle p, sai_object_id_t *switch_id,
                                      uint32_t attr_count,
                                      const sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->create_switch(switch_id, attr_count, attr_list);
}
sai_status_t sai_object_get_switch_attribute(S_O_Handle p,
                                             sai_object_id_t switch_id,
                                             sai_uint32_t attr_count,
                                             sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->get_switch_attribute(switch_id, attr_count, attr_list);
}

// PORT
sai_status_t sai_obj_create_port(S_O_Handle p, sai_object_id_t *port_id,
                                 sai_object_id_t switch_id, uint32_t attr_count,
                                 const sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->create_port(port_id, switch_id, attr_count, attr_list);
}

sai_status_t sai_obj_remove_port(S_O_Handle p, sai_object_id_t port_id) {
  sai_object *q = (sai_object *)p;
  return q->remove_port(port_id);
}

sai_status_t sai_obj_set_port_attribute(S_O_Handle p, sai_object_id_t port_id,
                                        sai_attribute_t *attr) {
  sai_object *q = (sai_object *)p;
  return q->set_port_attribute(port_id, attr);
}

sai_status_t sai_obj_get_port_attribute(S_O_Handle p, sai_object_id_t port_id,
                                        uint32_t attr_count,
                                        sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->get_port_attribute(port_id, attr_count, attr_list);
}

// BRIDGE
sai_status_t sai_obj_create_bridge(S_O_Handle p, sai_object_id_t *bridge_id,
                                   sai_object_id_t switch_id,
                                   uint32_t attr_count,
                                   const sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->create_bridge(bridge_id, switch_id, attr_count, attr_list);
}
sai_status_t sai_obj_remove_bridge(S_O_Handle p, sai_object_id_t bridge_id) {
  sai_object *q = (sai_object *)p;
  return q->remove_bridge(bridge_id);
}
sai_status_t get_bridge_attribute(S_O_Handle p, sai_object_id_t bridge_id,
                                  uint32_t attr_count,
                                  sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->get_bridge_attribute(bridge_id, attr_count, attr_list);
}

// BRIDGE PORT
sai_status_t sai_obj_create_bridge_port(S_O_Handle p,
                                        sai_object_id_t *bridge_port_id,
                                        sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->create_bridge_port(bridge_port_id, switch_id, attr_count,
                               attr_list);
}
sai_status_t sai_obj_remove_bridge_port(S_O_Handle p,
                                        sai_object_id_t bridge_port_id) {
  sai_object *q = (sai_object *)p;
  return q->remove_bridge_port(bridge_port_id);
}
sai_status_t get_bridge_port_attribute(S_O_Handle p,
                                       sai_object_id_t bridge_port_id,
                                       uint32_t attr_count,
                                       sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->get_bridge_port_attribute(bridge_port_id, attr_count, attr_list);
}

// FDB
sai_status_t sai_obj_create_fdb_entry(S_O_Handle p, sai_fdb_entry_t *fdb_entry,
                                      uint32_t attr_count,
                                      const sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->create_fdb_entry(fdb_entry, attr_count, attr_list);
}
sai_status_t sai_obj_remove_fdb_entry(S_O_Handle p,
                                      sai_fdb_entry_t *fdb_entry) {
  sai_object *q = (sai_object *)p;
  return q->remove_fdb_entry(fdb_entry);
}

// VLAN
sai_status_t sai_obj_create_vlan(S_O_Handle p, sai_object_id_t *vlan_id,
                                 sai_object_id_t switch_id, uint32_t attr_count,
                                 const sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->create_vlan(vlan_id, switch_id, attr_count, attr_list);
}
sai_status_t sai_obj_remove_vlan(S_O_Handle p, sai_object_id_t vlan_id) {
  sai_object *q = (sai_object *)p;
  return q->remove_vlan(vlan_id);
}

sai_status_t sai_obj_set_vlan_attribute(S_O_Handle p, sai_object_id_t vlan_id,
                                        const sai_attribute_t *attr) {
  sai_object *q = (sai_object *)p;
  return q->set_vlan_attribute(vlan_id, attr);
}

sai_status_t sai_obj_get_vlan_attribute(S_O_Handle p, sai_object_id_t vlan_id,
                                        const uint32_t attr_count,
                                        sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->get_vlan_attribute(vlan_id, attr_count, attr_list);
}

sai_status_t sai_obj_create_vlan_member(S_O_Handle p,
                                        sai_object_id_t *vlan_member_id,
                                        sai_object_id_t switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->create_vlan_member(vlan_member_id, switch_id, attr_count,
                               attr_list);
}
sai_status_t sai_obj_remove_vlan_member(S_O_Handle p,
                                        sai_object_id_t vlan_member_id) {
  sai_object *q = (sai_object *)p;
  return q->remove_vlan_member(vlan_member_id);
}
sai_status_t sai_obj_set_vlan_member_attribute(S_O_Handle p,
                                               sai_object_id_t vlan_member_id,
                                               const sai_attribute_t *attr) {
  sai_object *q = (sai_object *)p;
  return q->set_vlan_member_attribute(vlan_member_id, attr);
}
sai_status_t sai_obj_get_vlan_member_attribute(S_O_Handle p,
                                               sai_object_id_t vlan_member_id,
                                               const uint32_t attr_count,
                                               sai_attribute_t *attr_list) {
  sai_object *q = (sai_object *)p;
  return q->get_vlan_member_attribute(vlan_member_id, attr_count, attr_list);
}
sai_status_t sai_obj_get_vlan_stats(S_O_Handle p, sai_object_id_t vlan_id,
                                    const sai_vlan_stat_t *counter_ids,
                                    uint32_t number_of_counters,
                                    uint64_t *counters) {
  sai_object *q = (sai_object *)p;
  return q->get_vlan_stats(vlan_id, counter_ids, number_of_counters, counters);
}
sai_status_t sai_obj_clear_vlan_stats(S_O_Handle p, sai_object_id_t vlan_id,
                                      const sai_vlan_stat_t *counter_ids,
                                      uint32_t number_of_counters) {
  sai_object *q = (sai_object *)p;
  return q->clear_vlan_stats(vlan_id, counter_ids, number_of_counters);
}

// LAG
sai_status_t sai_obj_create_lag(S_O_Handle p, sai_object_id_t *lag_id,
                                sai_object_id_t switch_id, uint32_t attr_count,
                                const sai_attribute_t *attr_list);
sai_status_t sai_obj_remove_lag(S_O_Handle p, sai_object_id_t lag_id);
sai_status_t sai_obj_create_lag_member(S_O_Handle p,
                                       sai_object_id_t *lag_member_id,
                                       sai_object_id_t switch_id,
                                       uint32_t attr_count,
                                       const sai_attribute_t *attr_list);
sai_status_t sai_obj_remove_lag_member(S_O_Handle p,
                                       sai_object_id_t lag_member_id);
}
