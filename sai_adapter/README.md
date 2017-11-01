# SAI API
The sai lib (C lib) uses P4 thrift to configure the soft switch.

## Build
1) ```install_deps.sh``` will install python depencdecies.
2) ```./autogen.sh```
2) ```./configure```
4) ```make```
5) ```make install```    will install sai lib to default lib folder (usually /usr/local/lib/).

## PTF tests
SAI Tests were written using the [ptf](https://github.com/p4lang/ptf) framework. 

### Runing the tests
First, run make from this location. This will build the sai library, generate the needed thrift and ctypesgen files, and will build the C thrift server, needed to run the tests. \\

Now, follow these steps to run the PTF tests:  
1. In SAI-P4-BM/tree/master/p4-switch/sai-p4-target run:  
    1. veth_setup.sh  - to configure virtual ports. 
    2. run_server.sh  - to start the P4 BM server.  

2. Start new terminal and run the sai thrift server:  
    SAI-P4-BM/sai_adapter/test/sai_thrift_server/run_server.sh

3. Start another new terminal and run the [PTF tests](test/ptf_tests/) directly, or by one the test shell scripts found in the ptf test folder:  tests/ptf_tests/run_ptf_l2.sh

### LOGS
During the tests, the following logs are outputted:
1) BM server logs can be found under SAI-P4-BM/tree/master/p4-switch/sai-p4-target/logs
2) thrift server log is outputted to SAI-P4-BM/sai_adapter/test/sai_thrift_server/logs/log.txt
3) PTF log is stdout.  

# SAI BM Contribution Guide
## Adding BM functionaity
For new Soft Switch features ("HW" flows), please follow the [SAI-P4-BM/p4-softswitch](../SAI-P4-BM/p4-softswitch/) readme.

## Implementing new SAI functionaity:
* in [sai_object_interface.h](inc/sai_object_interface.h) create C function decleration. 
* in [sai_object_interface.cpp](src/sai_object_interface.cpp) create the function link, with the sai_object pointer.
* in [sai_object.h](inc/sai_object.h) add a static decleration for the function.
* for new/unimplemented api, also add the sai_api decleration, and link in the sai_api_query in the sai_object.h, and sai.c files.
* Add the function implementation to a .cpp file in the [src/](src/) dir, under the sai_object namespace.
### Additional information
* the sai_object uses the [switch_meta_data.h](inc/switch_meta_data.h) to manage the runtime information regarding the sai_object_ids etc... 
* to configure the BM tables, use the P4 thrift API is found (after p4-sofswitch compliation) in /SAI-P4-BM/p4-switch/behavioral-model/thrift_src/gen-cpp/bm/Standard.h
