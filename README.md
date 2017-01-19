# SAI BEHAVIORAL MODEL USING P4 
This repository is ment to provide a conformal behavioral model with SAI interface.
it implements SAI 1.0 on top of soft switch coded via P4.
the SAI API can can be used by including "sai.h" (see below for details).
the Packet Test Framework (pft) implements test using python.

##P4 - switch architecture
##SAI - switch api
[PTF](#ptf) - test frame work (currently using python thrift server and API)


This repository contains P4 implementation of SAI behavioural model, forked from [p4lang/behavioural_model](https://github.com/p4lang/behavioral-model/).

## Compiling P4 Behavioural model 
following instructions were taken from [p4lang/behavioural_model](https://github.com/p4lang/behavioral-model/).

### Dependencies
On Ubuntu 14.04, the following packages are required:

- automake
- cmake
- libjudy-dev
- libgmp-dev
- libpcap-dev
- libboost-dev
- libboost-test-dev
- libboost-program-options-dev
- libboost-system-dev
- libboost-filesystem-dev
- libboost-thread-dev
- libevent-dev
- libtool
- flex
- bison
- pkg-config
- g++
- libssl-dev

You also need to install [thrift](https://github.com/apache/thrift) and
[nanomsg](http://download.nanomsg.org/nanomsg-0.5-beta.tar.gz) from source. Feel
free to use the install scripts under travis/.

To use the CLI, you will need to install the
[nnpy](https://github.com/nanomsg/nnpy) Python package. Feel free to use
travis/install-nnpy.sh

To make your life easier, we provide the *install_deps.sh* script, which will
install all the dependencies needed on Ubuntu 14.04.

Our Travis regression tests now run on Ubuntu 14.04.

On MacOS you can use the tools/macos/bootstrap_mac.sh script to
install all the above dependencies using homebrew. Note that in order
to compile the code you need [XCode 8](https://itunes.apple.com/us/app/xcode/id497799835?mt=12)
or later.

### Building the code

    1. ./autogen.sh
    2. ./configure
    3. make
    4. [sudo] make install  # if you need to install bmv2

In addition, on Linux, you may have to run `sudo ldconfig` after installing
bmv2, to refresh the shared library cache.

Debug logging is enabled by default. If you want to disable it for performance
reasons, you can pass `--disable-logging-macros` to the `configure` script.

In 'debug mode', you probably want to disable compiler optimization and enable
symbols in the binary:

    ./configure 'CXXFLAGS=-O0 -g'

The new bmv2 debugger can be enabled by passing `--enable-debugger` to
`configure`.


## SAI Target
SAI P4 target is found inside targets/P4-SAI.

### Running SAI P4 program
running the switch can be done with the following command:

    sudo ./simple_switch -i 0@<iface0> -i 1@<iface1> <path to JSON file>

In this example \<iface0\> and \<iface1\> are the interfaces which are bound to
the switch (as ports 0 and 1).
Some default configurations are required (TODO: Maybe we can get rid of this need), to do this run:

     ./runtime_CLI < p4src/DefaultConfig.txt

Or more easily, you can use the script ```run_server.sh``` found in P4-SAI target directory, which does both.

### Using SAI to configure the switch
TODO.

### Compiling P4 code
if any editing to the P4 program was made, before running it you first need to transform the P4 code into a json representation which can be consumed by the software switch. This
representation will tell bmv2 which tables to initialize, how to configure the
parser, ... It is produced by the [p4c-bm](https://github.com/p4lang/p4c-bm)
tool. Please take a look at the
[README](https://github.com/p4lang/p4c-bm/blob/master/README.rst) for this repo
to find out how to install it. Once this is done, you can obtain the json file
as follows:

    p4c-bm --json <path to JSON file> <path to P4 file>
    
or more easily you can use the ```compile_json.sh``` script that is found inside the P4-SAI target directory.
<a name="ptf"></a>
# Running PTF tests
Inside target/P4-SAI/Sai-tests some tests were written using the [ptf](https://github.com/p4lang/ptf) framework. 
To run them you need run the sai switch with ```run_server.sh```.
The tests themselves are run in another terminal. run the python the rpc_server with ```sai_rpc_server.sh``` (TODO: this will change to the new C++ server when ready),  and in another terminal you can run ptf tests, for example using ```run_ptf_l2_tests.sh```
The directory is divided to tests (ready and passing tests), wip_tests for work in progress, and old_tests for old sai tests.
