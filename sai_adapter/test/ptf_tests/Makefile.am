SAI_INC_DIR = ../../inc/sai

SAI_TYPES_PY = \
				tests_new/sai_types.py \
				tests_old/sai_types.py

${SAI_TYPES_PY}: 
	ctypesgen.py ${SAI_INC_DIR}/*.h -o sai_types.py -I${SAI_INC_DIR}
	cp sai_types.py tests_new/.
	mv sai_types.py tests_old/sai_types.py

BUILT_SOURCES = ${SAI_TYPES_PY}
CLEANFILES = ${SAI_TYPES_PY}