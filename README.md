# SAI BEHAVIORAL MODEL
This repository clearly defines widely accepted network switch behaviour.
The switch behavioral model provides a commom framework for all network vendors and users.
It meant to describe the standard ethernet switch features and flows.
The model enables for both users and vendors to experiance the SAI switch model using SAI API over P4 soft switch.

## Repository Structure
The repository consist of:

### [P4 soft-switch](/p4-switch/)
P4_14 source files to be complied by the p4c complier 
Can be independently configured via the CLI interface.
### [SAI adapter](/sai_adapter/)
C source files implementation for the SAI API build for P4 soft switch model.
SAI lib
Tests frameworks: PTF, unittests.
### [Example](/example/)
Example applications built on top of SAI api. Currently only teamd based lacp app exists.

## Insatllation Instructions
The installation should be in the following order:
1) Enter `p4-switch/` dir, and follow the readme instructions.
2) Enter `sai_adapter/` dir, and follow the readme instructions.

## Architecture
The following image describes the architecture of the SAI bm:
![](sai_p4_bm_arch.png)
