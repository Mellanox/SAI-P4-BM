SAI_INC_DIR = ../../inc/sai
all: tests_new/sai_types.py tests_old/sai_types.py 
default: all
tests_new/sai_types.py: 
	ctypesgen.py ${SAI_INC_DIR}/*.h -o sai_types.py -I${SAI_INC_DIR}
	mv sai_types.py tests_new/.
tests_old/sai_types.py: tests_new/sai_types.py
	cp tests_new/sai_types.py tests_old/sai_types.py
clean:
	${RM} -rf tests_old/sai_types.py
	${RM} -rf tests_new/sai_types.py
install: ;
uninstall : ;