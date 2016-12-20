#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TMultiplexedProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "../../../thrift_src/gen-cpp/bm/Standard.h"
//#include "../../../thrift_src/gen-cpp/bm/Standard.cpp"
//#include "../../../thrift_src/gen-cpp/bm/standard_types.cpp"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace bm_runtime::standard;
int main() {
  cout << "creating socket" << endl;
  boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> bprotocol(new TBinaryProtocol(transport));
  boost::shared_ptr<TProtocol> protocol (new TMultiplexedProtocol(bprotocol, "standard"));
  StandardClient client(protocol);
  cout << "starting connection" << endl;
  try {
    transport->open();
    
    const string key = "\x00\x15";//"\x00\x15";

    // size_t match_params_num =1;
    // BmMatchParams match_keys(match_params_num);
    // match_keys[0].type = BmMatchParamType::EXACT;
    // match_keys[0].__set_exact(BmMatchParamExact());
    // match_keys[0].exact.__set_key(key);
    // cout << "match_key: " << match_keys[0] << endl;
  
	// std::string action_name = "action_set_lag_l2if";    
    // size_t action_param_num =3;
    // BmActionData action_data(action_param_num);
    // action_data[0]="0";
    // action_data[1]="00";
    // action_data[2]="0";

    // BmAddEntryOptions options;
    // options.__set_priority(0);
    // cout << "options: " << options << endl; 

    int32_t cxt_id=0;
    std::string table_name = "table_ingress_lag"; 
    BmEntryHandle handle = 0;
    // client.bm_mt_add_entry(cxt_id,table_name, match_keys, action_name , action_data, options);
    client.bm_mt_delete_entry(cxt_id, table_name, handle);
    transport->close();
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
cout << "thrift done" << endl;
}
