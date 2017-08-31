SAI_ADAPTER_DIR = ../sai_adapter
AM_CFLAGS = -I${SAI_ADAPTER_DIR}/inc/sai -I${SAI_ADAPTER_DIR}/inc -Wall -O3 -std=c++11 -pthread -w -D=HAVE_CONFIG_H

noinst_PROGRAMS = lacp_teaming_sai
nodist_lacp_teaming_sai_SOURCES = lacp_teaming_sai.c

lacp_teaming_sai_LDADD = ${SAI_ADAPTER_DIR}/src/libsai.la -lthrift -lteam -ljansson