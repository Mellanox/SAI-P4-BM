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

#include <bm/pdfixed/pd_common.h>
#include <bm/pdfixed/int/pd_helpers.h>

#include <string>
#include <vector>
#include <iostream>

#include "pd/pd_types.h"
#include "pd_client.h"

#define PD_DEBUG 1

// default is disabled
// #define HOST_BYTE_ORDER_CALLER

extern int *my_devices;

namespace {

template <int L>
std::string string_from_field(char *field) {
  return std::string((char *) field, L);
}

template <>
std::string string_from_field<1>(char *field) {
  return std::string(field, 1);
}

template <>
std::string string_from_field<2>(char *field) {
  uint16_t tmp = *(uint16_t *) field;
#ifdef HOST_BYTE_ORDER_CALLER
  tmp = htons(tmp);
#endif
  return std::string((char *) &tmp, 2);
}

template <>
std::string string_from_field<3>(char *field) {
  uint32_t tmp = *(uint32_t *) field;
#ifdef HOST_BYTE_ORDER_CALLER
  tmp = htonl(tmp);
#endif
  return std::string(((char *) &tmp) + 1, 3);
}

template <>
std::string string_from_field<4>(char *field) {
  uint32_t tmp = *(uint32_t *) field;
#ifdef HOST_BYTE_ORDER_CALLER
  tmp = htonl(tmp);
#endif
  return std::string((char *) &tmp, 4);
}

template <int L>
void string_to_field(const std::string &s, char *field) {
  assert(s.size() <= L);
  size_t offset = L - s.size();
  std::memset(field, 0, offset);
  std::memcpy(field + offset, s.data(), s.size());
}

template <>
void string_to_field<1>(const std::string &s, char *field) {
  assert(s.size() <= 1);
  if (s.size() == 1) *field = s[0];
}

template <>
void string_to_field<2>(const std::string &s, char *field) {
  uint16_t *tmp = (uint16_t *) field;
  *tmp = 0;
  assert(s.size() <= 2);
  size_t offset = 2 - s.size();
  std::memcpy(field, s.data(), s.size());
#ifdef HOST_BYTE_ORDER_CALLER
  *tmp = ntohs(*tmp);
#endif
}

template <>
void string_to_field<3>(const std::string &s, char *field) {
  uint32_t *tmp = (uint32_t *) field;
  *tmp = 0;
  assert(s.size() <= 3);
  size_t offset = 3 - s.size();
  std::memcpy(field, s.data(), s.size());
#ifdef HOST_BYTE_ORDER_CALLER
  *tmp = ntohl(*tmp);
#endif
}

template <>
void string_to_field<4>(const std::string &s, char *field) {
  uint32_t *tmp = (uint32_t *) field;
  *tmp = 0;
  assert(s.size() <= 4);
  size_t offset = 4 - s.size();
  std::memcpy(field, s.data(), s.size());
#ifdef HOST_BYTE_ORDER_CALLER
  *tmp = ntohl(*tmp);
#endif
}

std::vector<BmMatchParam> build_key_table_xSTP_instance (
    p4_pd_prog_table_xSTP_instance_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<2>((char *) &(match_spec->ingress_metadata_vid));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_xSTP_instance (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_xSTP_instance_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<2>(param.exact.key, (char *) &(match_spec->ingress_metadata_vid));
  }

}

std::vector<BmMatchParam> build_key_table_ingress_lag (
    p4_pd_prog_table_ingress_lag_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<2>((char *) &(match_spec->standard_metadata_ingress_port));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_ingress_lag (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_ingress_lag_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<2>(param.exact.key, (char *) &(match_spec->standard_metadata_ingress_port));
  }

}

std::vector<BmMatchParam> build_key_table_mc_fdb (
    p4_pd_prog_table_mc_fdb_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<6>((char *) &(match_spec->ethernet_dstAddr));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_mc_fdb (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_mc_fdb_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<6>(param.exact.key, (char *) &(match_spec->ethernet_dstAddr));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

}

std::vector<BmMatchParam> build_key_table_mc_l2_sg_g (
    p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(3);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<4>((char *) &(match_spec->ipv4_srcAddr));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<4>((char *) &(match_spec->ipv4_dstAddr));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_mc_l2_sg_g (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<4>(param.exact.key, (char *) &(match_spec->ipv4_srcAddr));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<4>(param.exact.key, (char *) &(match_spec->ipv4_dstAddr));
  }

}

std::vector<BmMatchParam> build_key_table_learn_fdb (
    p4_pd_prog_table_learn_fdb_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<6>((char *) &(match_spec->ethernet_srcAddr));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_learn_fdb (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_learn_fdb_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<6>(param.exact.key, (char *) &(match_spec->ethernet_srcAddr));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

}

std::vector<BmMatchParam> build_key_table_lag_hash (
    p4_pd_prog_table_lag_hash_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->egress_metadata_out_if));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_lag_hash (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_lag_hash_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->egress_metadata_out_if));
  }

}

std::vector<BmMatchParam> build_key_table_egress_lag (
    p4_pd_prog_table_egress_lag_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->egress_metadata_out_if));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->egress_metadata_hash_val));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_egress_lag (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_egress_lag_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->egress_metadata_out_if));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->egress_metadata_hash_val));
  }

}

std::vector<BmMatchParam> build_key_table_egress_vlan_filtering (
    p4_pd_prog_table_egress_vlan_filtering_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->egress_metadata_out_if));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<2>((char *) &(match_spec->ingress_metadata_vid));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_egress_vlan_filtering (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_egress_vlan_filtering_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->egress_metadata_out_if));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<2>(param.exact.key, (char *) &(match_spec->ingress_metadata_vid));
  }

}

std::vector<BmMatchParam> build_key_table_vbridge_STP (
    p4_pd_prog_table_vbridge_STP_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_port));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_vbridge_STP (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_vbridge_STP_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_port));
  }

}

std::vector<BmMatchParam> build_key_table_ingress_l2_interface_type (
    p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_l2_if));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<2>((char *) &(match_spec->ingress_metadata_vid));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_ingress_l2_interface_type (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_l2_if));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<2>(param.exact.key, (char *) &(match_spec->ingress_metadata_vid));
  }

}

std::vector<BmMatchParam> build_key_table_egress_vbridge_STP (
    p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_port));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_egress_vbridge_STP (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_port));
  }

}

std::vector<BmMatchParam> build_key_table_unknown_unicast (
    p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_unknown_unicast (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

}

std::vector<BmMatchParam> build_key_table_l3_if (
    p4_pd_prog_table_l3_if_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<6>((char *) &(match_spec->ethernet_dstAddr));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_l3_if (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_l3_if_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<6>(param.exact.key, (char *) &(match_spec->ethernet_dstAddr));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

}

std::vector<BmMatchParam> build_key_table_unknown_multicast (
    p4_pd_prog_table_unknown_multicast_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_unknown_multicast (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_unknown_multicast_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

}

std::vector<BmMatchParam> build_key_table_fdb (
    p4_pd_prog_table_fdb_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<6>((char *) &(match_spec->ethernet_dstAddr));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_fdb (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_fdb_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<6>(param.exact.key, (char *) &(match_spec->ethernet_dstAddr));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

}

std::vector<BmMatchParam> build_key_table_xSTP (
    p4_pd_prog_table_xSTP_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_port));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_stp_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_xSTP (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_xSTP_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_port));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_stp_id));
  }

}

std::vector<BmMatchParam> build_key_table_ingress_vlan (
    p4_pd_prog_table_ingress_vlan_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<2>((char *) &(match_spec->ingress_metadata_vid));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_ingress_vlan (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_ingress_vlan_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<2>(param.exact.key, (char *) &(match_spec->ingress_metadata_vid));
  }

}

std::vector<BmMatchParam> build_key_table_egress_xSTP (
    p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->egress_metadata_out_if));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_stp_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_egress_xSTP (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->egress_metadata_out_if));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_stp_id));
  }

}

std::vector<BmMatchParam> build_key_table_ingress_vlan_filtering (
    p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_l2_if));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<2>((char *) &(match_spec->ingress_metadata_vid));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_ingress_vlan_filtering (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_l2_if));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<2>(param.exact.key, (char *) &(match_spec->ingress_metadata_vid));
  }

}

std::vector<BmMatchParam> build_key_table_accepted_frame_type (
    p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_l2_if));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_valid.key = (match_spec->vlan_valid == 1);
  param = BmMatchParam();
  param.type = BmMatchParamType::type::VALID;
  param.__set_valid(param_valid); // does a copy of param_valid
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_accepted_frame_type (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_l2_if));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::VALID);
    match_spec->vlan_valid = (param.valid.key) ? 1 : 0;
  }

}

std::vector<BmMatchParam> build_key_table_egress_vbridge (
    p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_port));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_egress_vbridge (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_port));
  }

}

std::vector<BmMatchParam> build_key_table_vbridge (
    p4_pd_prog_table_vbridge_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(1);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_port));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_vbridge (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_vbridge_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_port));
  }

}

std::vector<BmMatchParam> build_key_table_l3_interface (
    p4_pd_prog_table_l3_interface_match_spec_t *match_spec
) {
  std::vector<BmMatchParam> key;
  key.reserve(2);

  BmMatchParam param;
  BmMatchParamExact param_exact; (void) param_exact;
  BmMatchParamLPM param_lpm; (void) param_lpm;
  BmMatchParamTernary param_ternary; (void) param_ternary;
  BmMatchParamValid param_valid; (void) param_valid;
  BmMatchParamRange param_range; (void) param_range;

  param_exact.key = string_from_field<6>((char *) &(match_spec->ethernet_dstAddr));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  param_exact.key = string_from_field<1>((char *) &(match_spec->ingress_metadata_bridge_id));
  param = BmMatchParam();
  param.type = BmMatchParamType::type::EXACT;
  param.__set_exact(param_exact); // does a copy of param_exact
  key.push_back(std::move(param));

  return key;
}

void unbuild_key_table_l3_interface (
    const std::vector<BmMatchParam> &key,
    p4_pd_prog_table_l3_interface_match_spec_t *match_spec
) {
  size_t i = 0;
  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<6>(param.exact.key, (char *) &(match_spec->ethernet_dstAddr));
  }

  {
    const BmMatchParam &param = key.at(i++);
    assert(param.type == BmMatchParamType::type::EXACT);
    string_to_field<1>(param.exact.key, (char *) &(match_spec->ingress_metadata_bridge_id));
  }

}


std::vector<std::string> build_action_data_action_set_egress_stp_state (
    p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_stp_state)));
  return action_data;
}

void unbuild_action_data_action_set_egress_stp_state (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_stp_state));
}
std::vector<std::string> build_action_data_action_set_lag_l2if (
    p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_is_lag)));
  action_data.push_back(string_from_field<2>((char *) &(action_spec->action_lag_id)));
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_l2_if)));
  return action_data;
}

void unbuild_action_data_action_set_lag_l2if (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_is_lag));
  string_to_field<2>(action_data.at(i++), (char *) &(action_spec->action_lag_id));
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_l2_if));
}
std::vector<std::string> build_action_data_action_set_vlan_tag_mode (
    p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_pcp)));
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_cfi)));
  action_data.push_back(string_from_field<2>((char *) &(action_spec->action_vid)));
  action_data.push_back(string_from_field<2>((char *) &(action_spec->action_ethType)));
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_tag_mode)));
  return action_data;
}

void unbuild_action_data_action_set_vlan_tag_mode (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_pcp));
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_cfi));
  string_to_field<2>(action_data.at(i++), (char *) &(action_spec->action_vid));
  string_to_field<2>(action_data.at(i++), (char *) &(action_spec->action_ethType));
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_tag_mode));
}
std::vector<std::string> build_action_data_action_set_out_port (
    p4_pd_prog_action_set_out_port_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_port)));
  return action_data;
}

void unbuild_action_data_action_set_out_port (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_out_port_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_port));
}
std::vector<std::string> build_action_data_action_forward_set_outIfType (
    p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_out_if)));
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_out_if_type)));
  return action_data;
}

void unbuild_action_data_action_forward_set_outIfType (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_out_if));
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_out_if_type));
}
std::vector<std::string> build_action_data_action_forward (
    p4_pd_prog_action_forward_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_port)));
  return action_data;
}

void unbuild_action_data_action_forward (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_forward_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_port));
}
std::vector<std::string> build_action_data_action_set_lag_hash_size (
    p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_lag_size)));
  return action_data;
}

void unbuild_action_data_action_set_lag_hash_size (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_lag_size));
}
std::vector<std::string> build_action_data_action_set_l2_if_type (
    p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_l2_if_type)));
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_bridge_port)));
  return action_data;
}

void unbuild_action_data_action_set_l2_if_type (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_l2_if_type));
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_bridge_port));
}
std::vector<std::string> build_action_data_action_set_bridge_id (
    p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_bridge_id)));
  return action_data;
}

void unbuild_action_data_action_set_bridge_id (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_bridge_id));
}
std::vector<std::string> build_action_data_action_set_mcast_snp (
    p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_mcast_snp)));
  return action_data;
}

void unbuild_action_data_action_set_mcast_snp (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_mcast_snp));
}
std::vector<std::string> build_action_data_action_set_pvid (
    p4_pd_prog_action_set_pvid_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<2>((char *) &(action_spec->action_pvid)));
  return action_data;
}

void unbuild_action_data_action_set_pvid (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_pvid_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<2>(action_data.at(i++), (char *) &(action_spec->action_pvid));
}
std::vector<std::string> build_action_data_action_set_stp_id (
    p4_pd_prog_action_set_stp_id_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_stp_id)));
  return action_data;
}

void unbuild_action_data_action_set_stp_id (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_stp_id_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_stp_id));
}
std::vector<std::string> build_action_data_action_set_stp_state (
    p4_pd_prog_action_set_stp_state_action_spec_t *action_spec
) {
  std::vector<std::string> action_data;
  action_data.push_back(string_from_field<1>((char *) &(action_spec->action_stp_state)));
  return action_data;
}

void unbuild_action_data_action_set_stp_state (
    const std::vector<std::string> &action_data,
    p4_pd_prog_action_set_stp_state_action_spec_t *action_spec
) {
  size_t i = 0;
  string_to_field<1>(action_data.at(i++), (char *) &(action_spec->action_stp_state));
}

}


extern "C" {

/* ADD ENTRIES */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_table_add_with_action_set_stp_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_xSTP_instance_match_spec_t *match_spec,
 p4_pd_prog_action_set_stp_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_xSTP_instance(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_stp_id(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_xSTP_instance", match_key, "action_set_stp_id", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP_instance" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_lag_table_add_with_action_set_lag_l2if
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_lag_match_spec_t *match_spec,
 p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_ingress_lag(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_lag_l2if(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_ingress_lag", match_key, "action_set_lag_l2if", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_add_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_mc_fdb(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_mc_fdb", match_key, "action_set_mc_fdb_miss", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_add_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_mc_fdb(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_mc_fdb", match_key, "action_forward_mc_set_if_list", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_add_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_mc_l2_sg_g(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_mc_l2_sg_g", match_key, "action_set_mc_fdb_miss", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_add_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_mc_l2_sg_g(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_mc_l2_sg_g", match_key, "action_forward_mc_set_if_list", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_add_with_action_learn_mac
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_learn_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_learn_fdb(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_learn_fdb", match_key, "action_learn_mac", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_add_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_learn_fdb_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_learn_fdb(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_learn_fdb", match_key, "_nop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_lag_hash_table_add_with_action_set_lag_hash_size
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_lag_hash_match_spec_t *match_spec,
 p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_lag_hash(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_lag_hash_size(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_lag_hash", match_key, "action_set_lag_hash_size", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_lag_hash" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_lag_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_lag(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_lag", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_add_with_action_set_out_port
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_lag_match_spec_t *match_spec,
 p4_pd_prog_action_set_out_port_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_lag(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_out_port(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_lag", match_key, "action_set_out_port", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vlan_filtering_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_vlan_filtering(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_vlan_filtering", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_table_add_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_vbridge_STP_match_spec_t *match_spec,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_vbridge_STP(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_stp_state(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_vbridge_STP", match_key, "action_set_stp_state", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_add_with_action_set_l2_if_type
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec,
 p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_ingress_l2_interface_type(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_l2_if_type(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_ingress_l2_interface_type", match_key, "action_set_l2_if_type", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_ingress_l2_interface_type(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_ingress_l2_interface_type", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_add_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_vbridge_STP(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_egress_stp_state(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_vbridge_STP", match_key, "action_set_egress_stp_state", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_vbridge_STP(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_vbridge_STP", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_unknown_unicast(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_unknown_unicast", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_add_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_unknown_unicast(match_spec);
  std::vector<std::string> action_data = build_action_data_action_forward(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_unknown_unicast", match_key, "action_forward", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_if_table_add_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_l3_if_match_spec_t *match_spec,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_l3_if(match_spec);
  std::vector<std::string> action_data = build_action_data_action_forward(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_l3_if", match_key, "action_forward", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_if" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_table_add_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_unknown_multicast_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_unknown_multicast(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_unknown_multicast", match_key, "action_forward_mc_set_if_list", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_multicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_fdb_table_add_with_action_forward_set_outIfType
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_fdb_match_spec_t *match_spec,
 p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_fdb(match_spec);
  std::vector<std::string> action_data = build_action_data_action_forward_set_outIfType(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_fdb", match_key, "action_forward_set_outIfType", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_xSTP_table_add_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_xSTP_match_spec_t *match_spec,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_xSTP(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_stp_state(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_xSTP", match_key, "action_set_stp_state", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_table_add_with_action_set_mcast_snp
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_vlan_match_spec_t *match_spec,
 p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_ingress_vlan(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_mcast_snp(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_ingress_vlan", match_key, "action_set_mcast_snp", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_add_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_xSTP(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_egress_stp_state(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_xSTP", match_key, "action_set_egress_stp_state", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_xSTP(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_xSTP", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_ingress_vlan_filtering(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_ingress_vlan_filtering", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_add_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_ingress_vlan_filtering(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_ingress_vlan_filtering", match_key, "_nop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_accepted_frame_type(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_accepted_frame_type", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_add_with_action_set_packet_vid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_accepted_frame_type(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_accepted_frame_type", match_key, "action_set_packet_vid", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_add_with_action_set_pvid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 p4_pd_prog_action_set_pvid_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_accepted_frame_type(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_pvid(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_accepted_frame_type", match_key, "action_set_pvid", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_vbridge(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_vbridge", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_add_with_action_set_vlan_tag_mode
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec,
 p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_egress_vbridge(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_vlan_tag_mode(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_egress_vbridge", match_key, "action_set_vlan_tag_mode", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_table_add_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_vbridge_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_vbridge(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_vbridge", match_key, "_drop", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_table_add_with_action_set_bridge_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_vbridge_match_spec_t *match_spec,
 p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_vbridge(match_spec);
  std::vector<std::string> action_data = build_action_data_action_set_bridge_id(action_spec);
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_vbridge", match_key, "action_set_bridge_id", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_interface_table_add_with_action_go_to_in_l3_if_table
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_table_l3_interface_match_spec_t *match_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<BmMatchParam> match_key = build_key_table_l3_interface(match_spec);
  std::vector<std::string> action_data;
  BmAddEntryOptions options;
  try {
    *entry_hdl = pd_client(dev_tgt.device_id).c->bm_mt_add_entry(
        0, "table_l3_interface", match_key, "action_go_to_in_l3_if_table", action_data, options);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_interface" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}



/* DELETE ENTRIES */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_xSTP_instance", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP_instance" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_lag_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_ingress_lag", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_mc_fdb", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_mc_l2_sg_g", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_learn_fdb", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_lag_hash_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_lag_hash", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_lag_hash" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_egress_lag", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_egress_vlan_filtering", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_vbridge_STP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_ingress_l2_interface_type", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_egress_vbridge_STP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_unknown_unicast", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_if_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_l3_if", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_if" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_unknown_multicast", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_multicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_fdb_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_fdb", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_xSTP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_xSTP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_ingress_vlan", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_egress_xSTP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_ingress_vlan_filtering", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_accepted_frame_type", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_egress_vbridge", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_vbridge", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_interface_table_delete
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  auto client = pd_client(dev_id);
  try {
    client.c->bm_mt_delete_entry(0, "table_l3_interface", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_interface" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}


/* MODIFY ENTRIES */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_table_modify_with_action_set_stp_id
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_stp_id_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_stp_id(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_xSTP_instance", entry_hdl, "action_set_stp_id", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP_instance" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_lag_table_modify_with_action_set_lag_l2if
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_lag_l2if(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_ingress_lag", entry_hdl, "action_set_lag_l2if", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_modify_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_mc_fdb", entry_hdl, "action_set_mc_fdb_miss", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_table_modify_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_mc_fdb", entry_hdl, "action_forward_mc_set_if_list", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_modify_with_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_mc_l2_sg_g", entry_hdl, "action_set_mc_fdb_miss", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_table_modify_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_mc_l2_sg_g", entry_hdl, "action_forward_mc_set_if_list", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_modify_with_action_learn_mac
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_learn_fdb", entry_hdl, "action_learn_mac", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_table_modify_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_learn_fdb", entry_hdl, "_nop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_lag_hash_table_modify_with_action_set_lag_hash_size
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_lag_hash_size(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_lag_hash", entry_hdl, "action_set_lag_hash_size", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_lag_hash" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_lag", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_table_modify_with_action_set_out_port
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_out_port_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_out_port(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_lag", entry_hdl, "action_set_out_port", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_vlan_filtering", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_table_modify_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_stp_state(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_vbridge_STP", entry_hdl, "action_set_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_modify_with_action_set_l2_if_type
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_l2_if_type(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_ingress_l2_interface_type", entry_hdl, "action_set_l2_if_type", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_ingress_l2_interface_type", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_modify_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_egress_stp_state(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_vbridge_STP", entry_hdl, "action_set_egress_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_vbridge_STP", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_unknown_unicast", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_table_modify_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_forward_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_forward(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_unknown_unicast", entry_hdl, "action_forward", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_if_table_modify_with_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_forward_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_forward(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_l3_if", entry_hdl, "action_forward", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_if" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_table_modify_with_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_unknown_multicast", entry_hdl, "action_forward_mc_set_if_list", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_multicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_fdb_table_modify_with_action_forward_set_outIfType
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_forward_set_outIfType(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_fdb", entry_hdl, "action_forward_set_outIfType", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_xSTP_table_modify_with_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_stp_state(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_xSTP", entry_hdl, "action_set_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_table_modify_with_action_set_mcast_snp
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_mcast_snp(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_ingress_vlan", entry_hdl, "action_set_mcast_snp", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_modify_with_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_egress_stp_state(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_xSTP", entry_hdl, "action_set_egress_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_xSTP", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_ingress_vlan_filtering", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_table_modify_with__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_ingress_vlan_filtering", entry_hdl, "_nop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_accepted_frame_type", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_modify_with_action_set_packet_vid
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_accepted_frame_type", entry_hdl, "action_set_packet_vid", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_table_modify_with_action_set_pvid
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_pvid_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_pvid(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_accepted_frame_type", entry_hdl, "action_set_pvid", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_vbridge", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_table_modify_with_action_set_vlan_tag_mode
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_vlan_tag_mode(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_egress_vbridge", entry_hdl, "action_set_vlan_tag_mode", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_table_modify_with__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_vbridge", entry_hdl, "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_table_modify_with_action_set_bridge_id
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data = build_action_data_action_set_bridge_id(action_spec);
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_vbridge", entry_hdl, "action_set_bridge_id", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_interface_table_modify_with_action_go_to_in_l3_if_table
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
) {
  assert(my_devices[dev_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_id).c->bm_mt_modify_entry(
        0, "table_l3_interface", entry_hdl, "action_go_to_in_l3_if_table", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_interface" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}



/* SET DEFAULT_ACTION */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_set_default_action_action_set_stp_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_stp_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_stp_id(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_xSTP_instance", "action_set_stp_id", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP_instance" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_lag_set_default_action_action_set_lag_l2if
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_lag_l2if_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_lag_l2if(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_ingress_lag", "action_set_lag_l2if", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_set_default_action_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_mc_fdb", "action_set_mc_fdb_miss", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_set_default_action_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_mc_fdb", "action_forward_mc_set_if_list", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_set_default_action_action_set_mc_fdb_miss
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_mc_l2_sg_g", "action_set_mc_fdb_miss", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_set_default_action_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_mc_l2_sg_g", "action_forward_mc_set_if_list", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_set_default_action_action_learn_mac
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_learn_fdb", "action_learn_mac", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_set_default_action__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_learn_fdb", "_nop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_lag_hash_set_default_action_action_set_lag_hash_size
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_lag_hash_size_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_lag_hash_size(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_lag_hash", "action_set_lag_hash_size", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_lag_hash" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_lag", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_set_default_action_action_set_out_port
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_out_port_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_out_port(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_lag", "action_set_out_port", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_vlan_filtering", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_set_default_action_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_stp_state(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_vbridge_STP", "action_set_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_set_default_action_action_set_l2_if_type
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_l2_if_type_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_l2_if_type(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_ingress_l2_interface_type", "action_set_l2_if_type", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_ingress_l2_interface_type", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_set_default_action_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_egress_stp_state(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_vbridge_STP", "action_set_egress_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_vbridge_STP", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_unknown_unicast", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_set_default_action_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_forward(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_unknown_unicast", "action_forward", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_if_set_default_action_action_forward
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_forward_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_forward(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_l3_if", "action_forward", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_if" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_set_default_action_action_forward_mc_set_if_list
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_unknown_multicast", "action_forward_mc_set_if_list", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_multicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_fdb_set_default_action_action_forward_set_outIfType
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_forward_set_outIfType_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_forward_set_outIfType(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_fdb", "action_forward_set_outIfType", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_xSTP_set_default_action_action_set_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_stp_state(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_xSTP", "action_set_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_set_default_action_action_set_mcast_snp
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_mcast_snp_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_mcast_snp(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_ingress_vlan", "action_set_mcast_snp", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_set_default_action_action_set_egress_stp_state
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_egress_stp_state_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_egress_stp_state(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_xSTP", "action_set_egress_stp_state", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_xSTP", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_ingress_vlan_filtering", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_set_default_action__nop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_ingress_vlan_filtering", "_nop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_accepted_frame_type", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_set_default_action_action_set_packet_vid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_accepted_frame_type", "action_set_packet_vid", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_set_default_action_action_set_pvid
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_pvid_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_pvid(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_accepted_frame_type", "action_set_pvid", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_vbridge", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_set_default_action_action_set_vlan_tag_mode
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_vlan_tag_mode(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_egress_vbridge", "action_set_vlan_tag_mode", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_set_default_action__drop
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_vbridge", "_drop", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_set_default_action_action_set_bridge_id
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_prog_action_set_bridge_id_action_spec_t *action_spec,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data = build_action_data_action_set_bridge_id(action_spec);
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_vbridge", "action_set_bridge_id", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_interface_set_default_action_action_go_to_in_l3_if_table
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t *entry_hdl
) {
  assert(my_devices[dev_tgt.device_id]);
  std::vector<std::string> action_data;
  try {
    pd_client(dev_tgt.device_id).c->bm_mt_set_default_action(
        0, "table_l3_interface", "action_go_to_in_l3_if_table", action_data);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_interface" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  return 0;
}




/* ENTRY RETRIEVAL */

p4_pd_status_t
p4_pd_prog_table_xSTP_instance_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_xSTP_instance_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_xSTP_instance", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP_instance" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_xSTP_instance(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_stp_id") {
    unbuild_action_data_action_set_stp_id(
        action_entry.action_data,
        (p4_pd_prog_action_set_stp_id_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_stp_id_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_stp_id";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_lag_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_lag_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_ingress_lag", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_ingress_lag(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_lag_l2if") {
    unbuild_action_data_action_set_lag_l2if(
        action_entry.action_data,
        (p4_pd_prog_action_set_lag_l2if_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_lag_l2if_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_lag_l2if";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_fdb_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_mc_fdb_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_mc_fdb", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_mc_fdb(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_mc_l2_sg_g_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_mc_l2_sg_g_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_mc_l2_sg_g", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_mc_l2_sg_g" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_mc_l2_sg_g(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_learn_fdb_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_learn_fdb_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_learn_fdb", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_learn_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_learn_fdb(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_lag_hash_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_lag_hash_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_lag_hash", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_lag_hash" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_lag_hash(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_lag_hash_size") {
    unbuild_action_data_action_set_lag_hash_size(
        action_entry.action_data,
        (p4_pd_prog_action_set_lag_hash_size_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_lag_hash_size_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_lag_hash_size";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_lag_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_lag_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_egress_lag", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_lag" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_egress_lag(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_out_port") {
    unbuild_action_data_action_set_out_port(
        action_entry.action_data,
        (p4_pd_prog_action_set_out_port_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_out_port_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_out_port";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vlan_filtering_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_vlan_filtering_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_egress_vlan_filtering", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_egress_vlan_filtering(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_STP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_vbridge_STP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_vbridge_STP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_vbridge_STP(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_stp_state") {
    unbuild_action_data_action_set_stp_state(
        action_entry.action_data,
        (p4_pd_prog_action_set_stp_state_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_stp_state_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_stp_state";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_l2_interface_type_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_l2_interface_type_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_ingress_l2_interface_type", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_l2_interface_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_ingress_l2_interface_type(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_l2_if_type") {
    unbuild_action_data_action_set_l2_if_type(
        action_entry.action_data,
        (p4_pd_prog_action_set_l2_if_type_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_l2_if_type_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_l2_if_type";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_STP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_vbridge_STP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_egress_vbridge_STP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge_STP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_egress_vbridge_STP(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_egress_stp_state") {
    unbuild_action_data_action_set_egress_stp_state(
        action_entry.action_data,
        (p4_pd_prog_action_set_egress_stp_state_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_egress_stp_state_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_egress_stp_state";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_unicast_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_unknown_unicast_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_unknown_unicast", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_unicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_unknown_unicast(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_forward") {
    unbuild_action_data_action_forward(
        action_entry.action_data,
        (p4_pd_prog_action_forward_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_forward_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_forward";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_if_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_l3_if_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_l3_if", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_if" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_l3_if(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_forward") {
    unbuild_action_data_action_forward(
        action_entry.action_data,
        (p4_pd_prog_action_forward_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_forward_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_forward";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_unknown_multicast_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_unknown_multicast_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_unknown_multicast", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_unknown_multicast" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_unknown_multicast(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_fdb_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_fdb_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_fdb", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_fdb" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_fdb(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_forward_set_outIfType") {
    unbuild_action_data_action_forward_set_outIfType(
        action_entry.action_data,
        (p4_pd_prog_action_forward_set_outIfType_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_forward_set_outIfType_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_forward_set_outIfType";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_xSTP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_xSTP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_xSTP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_xSTP(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_stp_state") {
    unbuild_action_data_action_set_stp_state(
        action_entry.action_data,
        (p4_pd_prog_action_set_stp_state_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_stp_state_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_stp_state";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_vlan_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_ingress_vlan", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_ingress_vlan(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_mcast_snp") {
    unbuild_action_data_action_set_mcast_snp(
        action_entry.action_data,
        (p4_pd_prog_action_set_mcast_snp_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_mcast_snp_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_mcast_snp";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_xSTP_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_xSTP_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_egress_xSTP", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_xSTP" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_egress_xSTP(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_egress_stp_state") {
    unbuild_action_data_action_set_egress_stp_state(
        action_entry.action_data,
        (p4_pd_prog_action_set_egress_stp_state_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_egress_stp_state_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_egress_stp_state";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_ingress_vlan_filtering_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_ingress_vlan_filtering_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_ingress_vlan_filtering", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_ingress_vlan_filtering" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_ingress_vlan_filtering(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_accepted_frame_type_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_accepted_frame_type_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_accepted_frame_type", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_accepted_frame_type" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_accepted_frame_type(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_pvid") {
    unbuild_action_data_action_set_pvid(
        action_entry.action_data,
        (p4_pd_prog_action_set_pvid_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_pvid_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_pvid";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_egress_vbridge_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_egress_vbridge_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_egress_vbridge", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_egress_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_egress_vbridge(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_vlan_tag_mode") {
    unbuild_action_data_action_set_vlan_tag_mode(
        action_entry.action_data,
        (p4_pd_prog_action_set_vlan_tag_mode_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_vlan_tag_mode_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_vlan_tag_mode";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_vbridge_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_vbridge_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_vbridge", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_vbridge" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_vbridge(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares
  if (action_entry.action_name == "action_set_bridge_id") {
    unbuild_action_data_action_set_bridge_id(
        action_entry.action_data,
        (p4_pd_prog_action_set_bridge_id_action_spec_t *) action_data);
    *num_action_bytes = sizeof(p4_pd_prog_action_set_bridge_id_action_spec_t);
    // not valid in C++, hence the cast, but I have no choice (can't change the
    // signature of the method)
    *action_name = (char *) "action_set_bridge_id";
  }


  return 0;
}

p4_pd_status_t
p4_pd_prog_table_l3_interface_get_entry
(
 p4_pd_sess_hdl_t sess_hdl,
 uint8_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 bool read_from_hw,
 p4_pd_prog_table_l3_interface_match_spec_t *match_spec,
 char **action_name,
 uint8_t *action_data,
 int *num_action_bytes
) {
  assert(my_devices[dev_id]);
  BmMtEntry entry;
  try {
    pd_client(dev_id).c->bm_mt_get_entry(entry, 0, "table_l3_interface", entry_hdl);
  } catch (InvalidTableOperation &ito) {
    const char *what =
      _TableOperationErrorCode_VALUES_TO_NAMES.find(ito.code)->second;
    std::cout << "Invalid table (" << "table_l3_interface" << ") operation ("
	      << ito.code << "): " << what << std::endl;
    return ito.code;
  }
  unbuild_key_table_l3_interface(entry.match_key, match_spec);

  const BmActionEntry &action_entry = entry.action_entry;
  assert(action_entry.action_type == BmActionEntryType::ACTION_DATA);
  *num_action_bytes = 0;
  // not efficient, but who cares


  return 0;
}



/* DIRECT COUNTERS */

/* legacy code, to be removed at some point */



/* Clean all state */
p4_pd_status_t
p4_pd_prog_clean_all(p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt) {
  assert(my_devices[dev_tgt.device_id]);
  pd_client(dev_tgt.device_id).c->bm_reset_state();
  return 0;
}

}
