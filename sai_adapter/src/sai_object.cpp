#include "sai_object.h"

StandardClient *sai_object::bm_client_ptr;
sai_id_map_t *sai_object::sai_id_map_ptr;
Switch_metadata *sai_object::switch_metadata_ptr;
std::vector<sai_object_id_t> *sai_object::switch_list_ptr;
std::shared_ptr<spdlog::logger> *sai_object::logger;

std::string parse_param(uint64_t param, uint32_t num_of_bytes) {
  std::string my_string = std::string(
      static_cast<char *>(static_cast<void *>(&param)), num_of_bytes);
  std::reverse(my_string.begin(), my_string.end());
  return my_string;
}

BmMatchParam parse_exact_match_param(uint64_t param, uint32_t num_of_bytes) {
  BmMatchParam match_param;
  match_param.type = BmMatchParamType::type::EXACT;
  BmMatchParamExact match_param_exact;
  match_param_exact.key = parse_param(param, num_of_bytes);
  match_param.__set_exact(match_param_exact);
  return match_param;
}

BmMatchParam parse_valid_match_param(bool param) {
  BmMatchParam match_param;
  match_param.type = BmMatchParamType::type::VALID;
  BmMatchParamValid match_param_valid;
  match_param_valid.key = param;
  match_param.__set_valid(match_param_valid);
  return match_param;
}

uint64_t parse_mac_64(uint8_t const mac_8[6]) {
  uint64_t mac_64 = 0;
  memcpy(&mac_64, mac_8, 6);
  return mac_64;
}

sai_object_id_t sai_object::temp_sai_get_bridge_port(uint32_t bridge_port_num) {
    (*logger)->debug("sai_object::temp_sai_get_bridge_port. bridge_port_num = {}", bridge_port_num);
    for (bridge_port_id_map_t::iterator it = switch_metadata_ptr->bridge_ports.begin(); it != switch_metadata_ptr->bridge_ports.end(); ++it) {
      if (it->second->bridge_port == bridge_port_num) {
        return it->first;
      }
    }
}