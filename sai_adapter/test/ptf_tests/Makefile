SAI_INC_DIR = ../../inc/SAI
all: sai_types.py
default: sai_types.py
sai_types.py: 
	ctypesgen.py ${SAI_INC_DIR}/*.h -o sai_types.py -I${SAI_INC_DIR}