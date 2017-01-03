#include "saifdb.h"
#include "Standard.h"
#include <iostream>
using namespace std;
using namespace bm_runtime::standard;

static sai_status_t sai_create_fdb_entry_fn(const sai_fdb_entry_t *fdb_entry,uint32_t attr_count,const sai_attribute_t *attr_list){
	int32_t cxt_id=0;
	cout << "print";
};

/**
 * @brief Remove FDB entry
 *
 * @param[in] fdb_entry FDB entry
 *
 * @return #SAI_STATUS_SUCCESS on success Failure status code on error
 */

// static sai_status_t (*sai_remove_fdb_entry_fn)(
//         _In_ const sai_fdb_entry_t *fdb_entry);

/**
 * @brief Set fdb entry attribute value
 *
 * @param[in] fdb_entry FDB entry
 * @param[in] attr Attribute
 *
 * @return #SAI_STATUS_SUCCESS on success Failure status code on error
 */
// static sai_status_t (*sai_set_fdb_entry_attribute_fn)(
//         _In_ const sai_fdb_entry_t *fdb_entry,
//         _In_ const sai_attribute_t *attr);

/**
 * @brief Get fdb entry attribute value
 *
 * @param[in] fdb_entry FDB entry
 * @param[in] attr_count Number of attributes
 * @param[inout] attr_list Array of attributes
 *
 * @return #SAI_STATUS_SUCCESS on success Failure status code on error
 */
// static sai_status_t (*sai_get_fdb_entry_attribute_fn)(
//         _In_ const sai_fdb_entry_t *fdb_entry,
//         _In_ uint32_t attr_count,
//         _Inout_ sai_attribute_t *attr_list);

/**
 * @brief Remove all FDB entries by attribute set in sai_fdb_flush_attr
 *
 * @param[in] switch_id Switch object id
 * @param[in] attr_count Number of attributes
 * @param[in] attr_list Array of attributes
 *
 * @return #SAI_STATUS_SUCCESS on success Failure status code on error
 */
// static sai_status_t (*sai_flush_fdb_entries_fn)(
//         _In_ sai_object_id_t switch_id,
//         _In_ uint32_t attr_count,
//         _In_ const sai_attribute_t *attr_list);

/**
 * @brief FDB notifications
 *
 * @param[in] count Number of notifications
 * @param[in] data Pointer to fdb event notification data array
 */
// static void (*sai_fdb_event_notification_fn)(
//         _In_ uint32_t count,
//         _In_ sai_fdb_event_notification_data_t *data);
  //   sai_create_fdb_entry_fn                     create_fdb_entry;
  //   sai_remove_fdb_entry_fn                     remove_fdb_entry;
  //   sai_set_fdb_entry_attribute_fn              set_fdb_entry_attribute;
  //   sai_get_fdb_entry_attribute_fn              get_fdb_entry_attribute;
  //   sai_flush_fdb_entries_fn                    flush_fdb_entries;