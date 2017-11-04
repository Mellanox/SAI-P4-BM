#ifndef FLEXSDK_H
#define FLEXSDK_H

#include "flextrum_types.h"
#ifdef __cplusplus
extern "C"{
#endif


typedef int (*init_flex_api_fn) ();
typedef int (*create_pipe_fn) ();
typedef int (*delete_pipe_fn) ();
typedef int (*add_table_entry_fn) (void** keys,void** masks,void** params,flextrum_action_type_t action, uint16_t* offset_ptr); // TODO add const to all types
typedef int (*remove_table_entry_fn) (uint16_t offset);
typedef int (*read_rule_counters_fn) ();
typedef int (*clear_rule_counters_fn) ();
#ifdef __cplusplus
}
#endif

#endif
