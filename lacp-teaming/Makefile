SAI_ADAPTER_DIR = ../sai_adapter
BM_DIR = ${SAI_ADAPTER_DIR}/../p4-softswitch/
INC_DIR = ${SAI_ADAPTER_DIR}/inc
SAI_INC_DIR    = ${INC_DIR}/SAI
SAI_SRC_DIR = ${SAI_ADAPTER_DIR}/src
CPP_DEFS   = -D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O3 -std=c++11 -pthread -w
LIBS       = -lp4sai -lpcap -lthrift -lteam -ljansson -lruntimestubs
INC_DIRS = -I${INC_DIR} -I${SAI_INC_DIR}
OBJ_DIR = obj
LACP_SRC = $(wildcard *.c)
LACP_OBJ = $(patsubst %.c,${OBJ_DIR}/%.o, $(LACP_SRC))
			
default: lacp_teaming_sai

${OBJ_DIR}/%.o: %.c
	gcc ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

lacp_teaming_sai: ${LACP_OBJ}
	g++ ${CPP_OPTS} ${CPP_DEFS} ${LACP_OBJ} -o lacp_teaming_sai ${INC_DIRS} ${LIBS}

clean:
	$(RM) -r lacp_teaming_sai ${OBJ_DIR}/*.o