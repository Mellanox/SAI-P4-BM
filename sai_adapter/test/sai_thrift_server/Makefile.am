SAI_ADAPTER_DIR = ../..
SAI_THRIFT_SRC_DIR = ../sai_thrift_src
AM_CXXFLAGS = -I${SAI_ADAPTER_DIR}/inc/sai -I${SAI_ADAPTER_DIR}/inc -I${SAI_THRIFT_SRC_DIR}/gen-cpp -Wall -O3 -std=c++11 -pthread -w -D=HAVE_CONFIG_H

noinst_PROGRAMS = SaiCppServer
nodist_SaiCppServer_SOURCES = SaiCppServer.cpp 

SaiCppServer_LDADD = ${SAI_ADAPTER_DIR}/src/libsai.la ${SAI_THRIFT_SRC_DIR}/libsai_thrift.la -lthrift
# -lthrift -lpcap