AM_CXXFLAGS =  -Wall -O3 -std=c++11 -pthread -w -D=HAVE_CONFIG_H

sai_thrift_sources = \
					../sai_thrift_src/gen-cpp/switch_sai_types.cpp \
					../sai_thrift_src/gen-cpp/switch_sai_constants.cpp \
					../sai_thrift_src/gen-cpp/switch_sai_rpc.cpp



noinst_LIBRARIES = libsai_thrift.a
libsai_thrift_a_SOURCES = ${sai_thrift_files}

${sai_thrift_files}: switch_sai.thrift 
	thrift --gen cpp --gen py switch_sai.thrift 

CLEANFILES = ${CLEANFILES}