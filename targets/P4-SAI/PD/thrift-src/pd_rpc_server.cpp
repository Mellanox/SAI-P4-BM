
#ifdef P4THRIFT
#include <p4thrift/processor/TMultiplexedProcessor.h>

namespace thrift_provider = p4::thrift;
#else
#include <thrift/processor/TMultiplexedProcessor.h>

namespace thrift_provider = apache::thrift;
#endif

using namespace ::thrift_provider;

using boost::shared_ptr;

#include "p4_pd_rpc_server.ipp"

extern "C" {

// processor needs to be of type TMultiplexedProcessor,
// I am keeping a void * pointer for 
// now, in case this function is called from C code
int add_to_rpc_server(void *processor) {
  std::cerr << "Adding Thrift service for P4 program prog to server\n";

  shared_ptr<progHandler> prog_handler(new progHandler());

  TMultiplexedProcessor *processor_ = (TMultiplexedProcessor *) processor;
  processor_->registerProcessor(
				"prog",
				shared_ptr<TProcessor>(new progProcessor(prog_handler))
				);
  
  return 0;
}

}
