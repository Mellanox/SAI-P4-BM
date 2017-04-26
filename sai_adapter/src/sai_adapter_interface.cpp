#include "../inc/sai_adapter_interface.h"
#include "../inc/sai_adapter.h"
#include "../inc/switch_meta_data.h"

extern "C" {
S_O_Handle create_sai_adapter() { return new sai_adapter(); }
void free_sai_adapter(S_O_Handle p) {
  sai_adapter *q = (sai_adapter *)p;
  // q->~sai_adapter();
  delete q;
}

// API
sai_status_t sai_adapter_api_query(S_O_Handle p, sai_api_t sai_api_id,
                                   void **api_method_table) {
  sai_adapter *q = (sai_adapter *)p;
  return q->sai_api_query(sai_api_id, api_method_table);
}
}