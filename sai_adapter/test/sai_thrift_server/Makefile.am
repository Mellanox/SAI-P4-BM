SAI_ADAPTER_DIR = ../..
SAI_THRIFT_SRC_DIR = ../sai_thrift_src/gen-cpp
AM_CXXFLAGS = -I/usr/include/sai -I${SAI_ADAPTER_DIR}/inc -I${SAI_THRIFT_SRC_DIR} -Wall -O3 -std=c++11 -pthread -w -D=HAVE_CONFIG_H

noinst_PROGRAMS = SaiCppServer
SaiCppServer_SOURCES = SaiCppServer.cpp \
						../sai_thrift_src/gen-cpp/switch_sai_types.cpp \
						../sai_thrift_src/gen-cpp/switch_sai_constants.cpp \
						../sai_thrift_src/gen-cpp/switch_sai_rpc.cpp

SaiCppServer_LDADD = ../../src/libp4sai.a -lthrift -lpcap -lruntimestubs 