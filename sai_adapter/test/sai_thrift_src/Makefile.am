AM_CXXFLAGS =  -Wall -O3 -std=c++11 -pthread -w -D=HAVE_CONFIG_H

sai_thrift_files = \
					gen-cpp/switch_sai_types.cpp \
					gen-cpp/switch_sai_constants.cpp \
					gen-cpp/switch_sai_rpc.cpp



noinst_LTLIBRARIES = libsai_thrift.la
libsai_thrift_la_SOURCES = ${sai_thrift_files}

${sai_thrift_files}: switch_sai.thrift 
	thrift --gen cpp --gen py switch_sai.thrift 

CLEANFILES = ${sai_thrift_files}