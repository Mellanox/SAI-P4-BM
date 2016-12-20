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

#include <gtest/gtest.h>

#include <bm/bm_apps/packet_pipe.h>

#include <boost/filesystem.hpp>

#include <string>
#include <memory>
#include <vector>

#include "simple_switch.h"

#include "utils.h"

namespace fs = boost::filesystem;

using bm::MatchErrorCode;
using bm::ActionData;
using bm::MatchKeyParam;
using bm::entry_handle_t;

namespace {

void
packet_handler(int port_num, const char *buffer, int len, void *cookie) {
  static_cast<SimpleSwitch *>(cookie)->receive(port_num, buffer, len);
}

}  // namespace

class SimpleSwitchSAIVlan : public ::testing::Test {
 protected:
  static constexpr size_t kMaxBufSize = 512;

  static constexpr int device_id{0};

  SimpleSwitchSAIVlan()
      : packet_inject(packet_in_addr),
        events(event_logger_addr) { }

  // Per-test-case set-up.
  // We make the switch a shared resource for all tests. This is mainly because
  // the simple_switch target detaches threads
  static void SetUpTestCase() {
    // bm::Logger::set_logger_console();
    auto event_transport = bm::TransportIface::make_nanomsg(event_logger_addr);
    event_transport->open();
    bm::EventLogger::init(std::move(event_transport));

    test_switch = new SimpleSwitch(8);  // 8 ports

    // load JSON
    fs::path json_path = fs::path(testdata_dir) / fs::path(test_json);
    test_switch->init_objects(json_path.string());

    // packet in - packet out
    test_switch->set_dev_mgr_packet_in(device_id, packet_in_addr, nullptr);
    test_switch->Switch::start();  // there is a start member in SimpleSwitch
    test_switch->set_packet_handler(packet_handler,
                                    static_cast<void *>(test_switch));
    test_switch->start_and_return();
  }

  // Per-test-case tear-down.
  static void TearDownTestCase() {
    delete test_switch;
  }

  virtual void SetUp() {
    // TODO(antonin): a lot of manual work here, can I add some of it?

    packet_inject.start();
    auto cb = std::bind(&PacketInReceiver::receive, &receiver,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4);
    packet_inject.set_packet_receiver(cb, nullptr);

    events.start();

    // default actions for all tables
    test_switch->mt_set_default_action(0, "t_ingress_1", "_nop", ActionData());
    test_switch->mt_set_default_action(0, "t_ingress_2", "_nop", ActionData());
    test_switch->mt_set_default_action(0, "t_egress", "_nop", ActionData());
    test_switch->mt_set_default_action(0, "t_exit", "set_hdr", ActionData());
  }

  virtual void TearDown() {
    // kind of experimental, so reserved for testing
    test_switch->reset_state();
  }

  bool check_event_table_hit(const NNEventListener::NNEvent &event,
                             const std::string &name) {
    return (event.type == NNEventListener::TABLE_HIT) &&
        (event.id == test_switch->get_table_id(name));
  }

  bool check_event_table_miss(const NNEventListener::NNEvent &event,
                              const std::string &name) {
    return (event.type == NNEventListener::TABLE_MISS) &&
        (event.id == test_switch->get_table_id(name));
  }

  bool check_event_action_execute(const NNEventListener::NNEvent &event,
                                  const std::string &t_name,
                                  const std::string &a_name) {
    return (event.type == NNEventListener::ACTION_EXECUTE) &&
        (event.id == test_switch->get_action_id(t_name, a_name));
  }

 protected:
  static const std::string event_logger_addr;
  static const std::string packet_in_addr;
  static SimpleSwitch *test_switch;
  bm_apps::PacketInject packet_inject;
  PacketInReceiver receiver{};
  NNEventListener events;

 private:
  static const std::string testdata_dir;
  static const std::string test_json;
};

// In theory, I could be using an 'inproc' transport here. However, I observe a
// high number of packet drops when switching to 'inproc', which is obviosuly
// causing the tests to fail. PUB/SUB is not a reliable protocol and therefore
// packet drops are to be expected when the phblisher is faster than the
// consummer. However, I do not believe my consummer is that slow and I never
// observe the drops with 'ipc'
const std::string SimpleSwitchSAIVlan::event_logger_addr =
    "ipc:///tmp/test_events_abc123";
const std::string SimpleSwitchSAIVlan, Baseline) {
  static constexpr int port_in = 1;
  static constexpr int port_out = 2;

  std::vector<MatchKeyParam> match_key;
  match_key.emplace_back(MatchKeyParam::Type::EXACT, std::string("\x01"));
  match_key.emplace_back(MatchKeyParam::Type::EXACT, std::string("\x00", 1));
  ActionData data;
  data.push_back_action_data(port_out);
  entry_handle_t handle;
  MatchErrorCode rc = test_switch->mt_add_entry(0, "t_ingress_1", match_key,
                                                "_set_port", std::move(data),
                                                &handle);
  ASSERT_EQ(MatchErrorCode::SUCCESS, rc);
  const char pkt[] = {'\x01', '\x00', '\x00', '\x00'};
  packet_inject.send(port_in, pkt, sizeof(pkt));
  char recv_buffer[kMaxBufSize];
  int recv_port = -1;
  receiver.read(recv_buffer, sizeof(pkt), &recv_port);
  // make sure that standard_metadata.packet_length was updated after removing
  // header
  ASSERT_EQ(2, static_cast<int>(recv_buffer[1]));
  ASSERT_EQ(port_out, recv_port);

#ifdef BMELOG_ON
  // event check
  std::vector<NNEventListener::NNEvent> pevents;
  events.get_and_remove_events("0.0", &pevents, 8u);
  ASSERT_EQ(8u, pevents.size());
  ASSERT_TRUE(check_event_table_hit(pevents[0], "t_ingress_1"));
  ASSERT_TRUE(check_event_action_execute(pevents[1], "t_ingress_1",
                                         "_set_port"));
  ASSERT_TRUE(check_event_table_miss(pevents[2], "t_ingress_2"));
  ASSERT_TRUE(check_event_action_execute(pevents[3], "t_ingress_2", "_nop"));
  ASSERT_TRUE(check_event_table_miss(pevents[4], "t_egress"));
  ASSERT_TRUE(check_event_action_execute(pevents[5], "t_egress", "_nop"));
  ASSERT_TRUE(check_event_table_miss(pevents[6], "t_exit"));
  ASSERT_TRUE(check_event_action_execute(pevents[7], "t_exit", "set_hdr"));
#endif
}
