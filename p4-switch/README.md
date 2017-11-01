SAI p4 soft switch is based on thee behavioral-model framework, and currently uses P4_14 v1.1 (Migrationg to the new P4_16 language has started).

## Insatllation Instructions
### Dependecies
Please enter the behavioral model dir and follow instructions for intallation of p4 behavioral model dependecies.
### Build
To build, please run:
1) ```./autogen.sh```
2) ```./configure --without-targets```  
  the flag ```--without-targets``` disables building p4's default targets, if they are needed, please remove the flag.
3) ```make```
4) optional, run ```make install``` if installation is needed.  
Alternatively, use ```build.sh``` to run all 3 steps.

### Installing P4 Compiler
Please follow [p4c-bm](https://github.com/p4lang/p4c-bm) to install the p4 compiler. Please note that SAI bm uses p4_14 v1.1 version.

## Contribution Guide
All the p4 code describing the SAI bm resides inside [p4src](/p4-switch/sai-p4-bm/p4src).
After editing it, run the compile scripts (compile_json.sh), to compile the new p4 program. 
Test it using PTF tests as explained in sai_adapter dir.
