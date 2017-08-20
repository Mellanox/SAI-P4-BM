CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O3 -std=c++11 -pthread -w
BM_DIR = ../../p4-softswitch/thrift_src/gen-cpp/bm
OBJ_DIR    = obj
SAI_OBJ_IF_SRC    = $(wildcard *.cpp)
SAI_OBJ_IF_OBJ    = $(patsubst %.cpp,${OBJ_DIR}/%.o, $(SAI_OBJ_IF_SRC))
INC_DIR = ../inc
SAI_DIR = ${INC_DIR}/SAI/
SAI_SRC = $(wildcard *.c)
SAI_OBJ = $(patsubst %.c,${OBJ_DIR}/%.o, $(SAI_SRC))
HEADERS = $(wildcard *.h)
INC_DIRS =-I${USR_DIR}/include -I${THRIFT_DIR}/include/thrift -I${INC_DIR} -I${SAI_DIR} -I${BM_DIR}
SAI_INC = -I${SAI_DIR} -I$(INC_DIR)
OBJECTS = $(SAI_OBJ) $(SAI_OBJ_IF_OBJ)
default = libp4sai

.PHONY: libp4sai clean

# default: sai_adapter

${OBJ_DIR}/%.o: %.cpp ${HEADERS}
	g++ -fPIC ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

${OBJ_DIR}/%.o: %.c
	gcc -fPIC ${SAI_INC} -c $< -o $@

libp4sai: ${OBJECTS}
	ar rvs ../lib/libp4sai.a ${OBJECTS}

clean:
	rm -f $(SAI_OBJ) $(SAI_OBJ_IF_OBJ) ../lib/libp4sai.a