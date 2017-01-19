# SAI BEHAVIORAL MODEL USING P4 
This repository is ment to provide a conformal behavioral model with SAI interface.  
It implements SAI 1.0 on top of soft switch coded via P4.  
The Packet Test Framework (pft) implements test using python.

###[P4](#p4) - switch architecture
###[SAI](#sai) - switch api
###[PTF](#ptf) - test frame work


This repository contains P4 implementation of SAI behavioural model, forked from [p4lang/behavioural_model](https://github.com/p4lang/behavioral-model/).
<a name="p4"></a>
# P4
## Compiling P4 Behavioural model 
For list of dependencies and build instructions:
[p4lang/behavioural_model](https://github.com/p4lang/behavioral-model/).

## P4 Target
P4 target is found inside [targets/P4-SAI](targets/P4-SAI).

### Create virtual interfaces
Use the ```veth_setup.sh``` script to create the virtual interfaces and hosts.
### Running P4 program
running the switch can be done with the following command: ```run_server.sh``` found in P4-SAI target directory.
This script olso implements some default configurations found in: p4src/DefaultConfig.txt


### Compiling P4 code
if any editing to the P4 program was made, before running it you first need to transform the P4 code into a json representation which can be consumed by the software switch. This
representation will tell bmv2 which tables to initialize, how to configure the
parser, ... It is produced by the [p4c-bm](https://github.com/p4lang/p4c-bm)
tool. Please take a look at the
[README](https://github.com/p4lang/p4c-bm/blob/master/README.rst) for this repo
to find out how to install it. Once this is done, 
use the ```compile_json.sh``` script that is found inside the P4-SAI target directory.
or manualy, as follows:

    p4c-bm --json <path to JSON file> <path to P4 file>
    

<a name="sai"></a>
# SAI API
![[sai_arch]](https://github.com/YonatanPitz/SAI-P4-BM/blob/master/docs/SAI_arch.PNG)  
The SAI lib (C lib) can be found and included in [SAI dir](targets/P4-SAI/SAI/)  
The sai lib uses P4 thrift to configure the soft switch

<a name="ptf"></a>
# Running PTF tests
Inside target/P4-SAI/Sai-tests some tests were written using the [ptf](https://github.com/p4lang/ptf) framework. 
To run them you need run the sai switch with ```run_server.sh```.
The tests themselves are run in another terminal. run the python the rpc_server with ```sai_rpc_server.sh``` (TODO: this will change to the new C++ server when ready),  and in another terminal you can run ptf tests, for example using ```run_ptf_l2_tests.sh```
The directory is divided to tests (ready and passing tests), wip_tests for work in progress, and old_tests for old sai tests.
