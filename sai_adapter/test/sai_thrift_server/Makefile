# THRIFT_VER =thrift-0.9.2
# USR_DIR    =${HOME}/usr
# THRIFT_DIR =${USR_DIR}/${THRIFT_VER}
SAI_ADAPTER_DIR = ../..
# BM_DIR     = ${SAI_ADAPTER_DIR}/../p4-softswitch/thrift_src/gen-cpp/bm
INC_DIR = ${SAI_ADAPTER_DIR}/inc
SAI_INC_DIR    = ${INC_DIR}/SAI
SAI_SRC_DIR = ${SAI_ADAPTER_DIR}/src
# LOG_DIR    = ${INC_DIR}/spdlog
# LIBS_DIRS  =-L${USR_DIR}/lib -L${USR_DIR}/${THRIFT_VER}/lib
LIBS_DIRS = -L${SAI_ADAPTER_DIR}/lib
CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O3 -std=c++11 -pthread -w
LIBS       = ${LIBS_DIRS} -lp4sai -lthrift -lpcap
SAI_THRIFT_SRC_DIR    = ${SAI_ADAPTER_DIR}/test/sai_thrift_src/gen-cpp

# STAND_OBJS = ${BM_DIR}/Standard.o  \
#              ${BM_DIR}/standard_types.o \
#              ${BM_DIR}/standard_constants.o
# SIMPLE_PRE_LAG_OBJS = ${BM_DIR}/SimplePreLAG.o  \
#              ${BM_DIR}/simple_pre_lag_types.o \
#              ${BM_DIR}/simple_pre_lag_constants.o

# INC_DIRS  =-I${USR_DIR}/include -I${THRIFT_DIR}/include/thrift -I${INC_DIR} -I${SAI_INC_DIR} -I${BM_DIR} -I${SAI_THRIFT_SRC_DIR}
INC_DIRS = -I${INC_DIR} -I${SAI_INC_DIR} -I${SAI_THRIFT_SRC_DIR}

OBJ_DIR    = obj
SAI_THRIFT_SRC = switch_sai_types.cpp switch_sai_constants.cpp switch_sai_rpc.cpp
SAI_THRIFT_OBJ = $(patsubst %.cpp,${SAI_THRIFT_SRC_DIR}/${OBJ_DIR}/%.o, $(SAI_THRIFT_SRC))
# SAI_SRC =  $(wildcard ${SAI_SRC_DIR}/*.c)
# SAI_SRC_HEADERS = $(wildcard ${INC_DIR}/*.h)
# SAI_OBJS = $(patsubst %.c,${SAI_SRC_DIR}/${OBJ_DIR}/%.o, $(notdir $(SAI_SRC)))
# SAI_OBJ_IF_SRC    = $(wildcard ${SAI_SRC_DIR}/*.cpp)
# SAI_OBJ_IF_OBJ    = $(patsubst %.cpp,${SAI_SRC_DIR}/${OBJ_DIR}/%.o, $(notdir $(SAI_OBJ_IF_SRC)))
# OBJECTS = ${SAI_THRIFT_OBJ} ${SAI_OBJS} ${SAI_OBJ_IF_OBJ} ${STAND_OBJS} ${SIMPLE_PRE_LAG_OBJS}
OBJECTS = ${SAI_THRIFT_OBJ}

default: server

${SAI_THRIFT_SRC_DIR}/${OBJ_DIR}/%.o: ${SAI_THRIFT_SRC_DIR}/%.cpp 
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

# ${BM_DIR}/%.o: ${BM_DIR}/%.cpp
# 	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

# ${SAI_SRC_DIR}/${OBJ_DIR}%.o: ${SAI_SRC_DIR}/%.c
# 	gcc ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

# ${SAI_SRC_DIR}/${OBJ_DIR}/%.o: ${SAI_SRC_DIR}/%.cpp ${SAI_SRC_HEADERS}
# 	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@ ${LIBS}

%.o: %.cpp
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

server: ${OBJECTS} SaiCppServer.o
	g++ ${CPP_OPTS} ${CPP_DEFS} -o SaiCppServer ${INC_DIRS} SaiCppServer.o ${OBJECTS} ${LIBS}

clean:
	$(RM) -r SaiCppServer ${OBJ_DIR}/*.o