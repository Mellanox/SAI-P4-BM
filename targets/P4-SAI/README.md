# P4-SAI
SAI implementation in P4

For easy use of useful symbolic links, clone inside behavioral-model/targets/

# Running
execute ```sudo veth_setup.sh``` to create veth interfaces.
execute ```run_server.sh``` to run the behavioral_model switch.  
log is written automatically to log.txt

# Compiling Target
## Only Once:
(Do this if you still don't have Makefile.in).
In behavioural-model/configure.ac:  
Under Generate makefiles:
  * targets/P4-SAI/Makefile
  * targets/P4-SAI/tests/Makefile
  * targets/P4-SAI/tests/CLI_tests/Makefile
  
Under Generate Other files 
  * AC_CONFIG_FILES([targets/P4-SAI/tests/CLI_tests/run_one_test.py], [chmod +x targets/P4-SAI/tests/CLI_tests/run_one_test.py])

In behaviooural-model/targets folder/Makefile.am:  
* add P4-SAI to SUBDIRS variable.  

now execute in behavioural-model/ folder:
* ```./autogen.sh```
* ```./configure```
* ```make```


## Changes only in target:
For every change in simple_switch.cpp target only needed to do:  
execute ```make``` in repositor directory.

# Compiling P4 to JSON
make sure you have the [p4c-bm](https://github.com/p4lang/p4c-bm) compiler installed.
run ```p4c-bmv2 --p4-v1.1 --json sai.json --pd PD p4src/sai.p4``` on main directory.
