# SAI API
The sai lib (C lib) uses P4 thrift to configure the soft switch as described below:
![[sai_arch]](https://github.com/Mellanox/SAI-P4-BM/blob/master/p4-softswitch/docs/SAI_arch.PNG)  

## Including the SAI lib
The sai_api lib is currently under development.
In the meantime, please include sai.h, check the [unit test](test/unittest) folder for example.
Please build it from the src folder using "make".

## PTF tests
SAI Tests were written using the [ptf](https://github.com/p4lang/ptf) framework. 
The directory is divided to tests (ready and passing tests), wip_tests for work in progress.

### Runing the tests
First, bulid the thrift server found in SAI-P4-BM/sai-host_interface/ by typing "make" in this location.

Now, follow these steps to run the PTF tests:
1) In SAI-P4-BM/tree/master/p4-softswitch/targets/P4-SAI/ run:
1.1) veth_setup.sh  - to configure virtual ports,
1.2) run_server.sh  - to start the P4 BM server.

2) Start new terminal and run the sai thrift server:
  SAI-P4-BM/sai-host_interface/saithrift/SaiCppServer

3) Start another new terminal and run the [PTF tests](test/ptf_tests/) directly, or by one the test shell scripts found in the ptf test folder. 

### Python development server
For P4 new features testing, it is possible to bypass implementing new SAI C implementations, and just update the thirft implementation in python.
use the [python thrift server](test/sai_thrift_development_python_server/) found in test/sai_thrift_development_python_server/.
If you decide to do so, replace step (2) with tests/sai_thrift_development_python_server/sai_rpc_server.sh

### LOGS
During the tests, the following logs are outputted:
1) BM server log can be found under SAI-P4-BM/p4-softswitch/targets/P4-SAI/log.txt
2) thrift server log is outputted to stdout.
3) PTF log is stdout.
