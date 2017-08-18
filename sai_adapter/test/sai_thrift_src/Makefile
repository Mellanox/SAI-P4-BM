all: gen-cpp/switch_sai_constants.cpp
gen-cpp/switch_sai_constants.cpp: switch_sai.thrift
	thrift --gen cpp --gen py switch_sai.thrift