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
