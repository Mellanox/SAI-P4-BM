/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Antonin Bas (antonin@barefootnetworks.com)
 *
 */

#ifndef _P4_PD_LEARNING_H_
#define _P4_PD_LEARNING_H_

#include <bm/pdfixed/pd_common.h>

#include "pd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

p4_pd_status_t p4_pd_prog_set_learning_timeout
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t device_id,
 uint64_t usecs
);

typedef struct p4_pd_prog_mac_learn_digest_digest_entry {
  uint8_t ethernet_srcAddr[6];
  uint8_t ingress_metadata_bridge_id;
  uint16_t standard_metadata_ingress_port;
} p4_pd_prog_mac_learn_digest_digest_entry_t;

typedef struct p4_pd_prog_mac_learn_digest_digest_msg {
  p4_pd_dev_target_t dev_tgt;
  uint16_t num_entries;
  p4_pd_prog_mac_learn_digest_digest_entry_t *entries;
  uint64_t buffer_id; // added by me, needed for BMv2 compatibility
} p4_pd_prog_mac_learn_digest_digest_msg_t;

typedef p4_pd_status_t
(*p4_pd_prog_mac_learn_digest_digest_notify_cb)(p4_pd_sess_hdl_t,
					   p4_pd_prog_mac_learn_digest_digest_msg_t *,
					   void *cookie);

p4_pd_status_t p4_pd_prog_mac_learn_digest_register
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t device_id,
 p4_pd_prog_mac_learn_digest_digest_notify_cb cb_fn,
 void *cb_fn_cookie
);

p4_pd_status_t p4_pd_prog_mac_learn_digest_deregister
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t device_id
);

p4_pd_status_t p4_pd_prog_mac_learn_digest_notify_ack
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_prog_mac_learn_digest_digest_msg_t *msg
);


#ifdef __cplusplus
}
#endif

#endif
