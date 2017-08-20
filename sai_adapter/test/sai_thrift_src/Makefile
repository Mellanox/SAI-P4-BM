all: gen-cpp/switch_sai_constants.cpp
install: ;
uninstall: ;
gen-cpp/switch_sai_constants.cpp: switch_sai.thrift
	thrift --gen cpp --gen py switch_sai.thrift
clean:
	${RM} -rf gen-cpp/ gen-py/