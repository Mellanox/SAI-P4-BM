SAI_INC_DIR = ../inc/SAI
SAI_HEADERS = $(wildcard ${SAI_INC_DIR}/*.h)
sai_types.py: ${SAI_HEADERS}
	ctypesgen.py ${SAI_INC_DIR}/*.h -o sai_types.py -I${SAI_INC_DIR}
