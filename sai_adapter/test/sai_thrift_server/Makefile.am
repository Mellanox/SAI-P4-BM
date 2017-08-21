SAI_ADAPTER_DIR = ../..
SAI_THRIFT_SRC_DIR = ../sai_thrift_src/gen-cpp
AM_CXXFLAGS = -I${SAI_ADAPTER_DIR}/inc/sai -I${SAI_ADAPTER_DIR}/inc -I${SAI_THRIFT_SRC_DIR} -Wall -O3 -std=c++11 -pthread -w -D=HAVE_CONFIG_H

noinst_PROGRAMS = SaiCppServer
nodist_SaiCppServer_SOURCES = SaiCppServer.cpp 

SaiCppServer_LDADD = ../../src/libp4sai.a ../sai_thrift_src/libsai_thrift.a -lthrift -lpcap -lruntimestubs 