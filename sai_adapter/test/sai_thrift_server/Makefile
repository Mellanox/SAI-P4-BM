SAI_ADAPTER_DIR = ../..
BM_DIR      = ${SAI_ADAPTER_DIR}/../p4-softswitch
INC_DIR     = ${SAI_ADAPTER_DIR}/inc
SAI_INC_DIR = ${INC_DIR}/SAI 
SAI_SRC_DIR = ${SAI_ADAPTER_DIR}/src
LIBS_DIRS = -L${SAI_ADAPTER_DIR}/lib
CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O3 -std=c++11 -pthread -w
LIBS       = ${LIBS_DIRS} -lp4sai -lthrift -lpcap -lruntimestubs
SAI_THRIFT_SRC_DIR    = ${SAI_ADAPTER_DIR}/test/sai_thrift_src/gen-cpp
INC_DIRS = -I${INC_DIR} -I${SAI_INC_DIR} -I${SAI_THRIFT_SRC_DIR} 
OBJ_DIR    = obj
SAI_THRIFT_SRC = switch_sai_types.cpp switch_sai_constants.cpp switch_sai_rpc.cpp
SAI_THRIFT_OBJ = $(patsubst %.cpp,${SAI_THRIFT_SRC_DIR}/${OBJ_DIR}/%.o, $(SAI_THRIFT_SRC))
OBJECTS = ${SAI_THRIFT_OBJ} ${OBJ_DIR}/SaiCppServer.o

default: server

${SAI_THRIFT_SRC_DIR}/${OBJ_DIR}/%.o: ${SAI_THRIFT_SRC_DIR}/%.cpp 
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

${OBJ_DIR}/%.o: %.cpp
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

server: ${OBJECTS} 
	g++ ${CPP_OPTS} ${CPP_DEFS} -o SaiCppServer ${INC_DIRS} ${OBJECTS} ${LIBS}

clean:
	$(RM) -r SaiCppServer ${OBJ_DIR}/*.o