THRIFT_VER =thrift-0.9.2
USR_DIR    =${HOME}/usr
THRIFT_DIR =${USR_DIR}/${THRIFT_VER}
SAI_INC_DIR    = ../inc/
SAI_SRC_DIR = ../src
LIBS_DIRS  =-L${USR_DIR}/lib -L${USR_DIR}/${THRIFT_VER}/lib -L../inc 
CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O2 -std=c++11 -w
LIBS       =-lthrift
BM_DIR     = ../../../../thrift_src/gen-cpp/bm

STAND_OBJS = ${BM_DIR}/Standard.o  \
             ${BM_DIR}/standard_types.o \
             ${BM_DIR}/standard_constants.o

INC_DIRS  =-I${USR_DIR}/include -I${THRIFT_DIR}/include/thrift -I${SAI_INC_DIR} -I${BM_DIR}


GEN_SRC    = switch_sai_types.cpp switch_sai_constants.cpp switch_sai_rpc.cpp
SRC_DIR    = ../../Sai-Tests/sai_thrift_src/gen-cpp
OBJ_DIR    = obj

SAI_SRC =  $(wildcard ${SAI_SRC_DIR}/*.c)
SAI_OBJS = $(patsubst %.c,${SAI_SRC_DIR}/${OBJ_DIR}/%.o, $(notdir $(SAI_SRC)))
SAI_OBJ_IF_SRC    = ${SAI_SRC_DIR}/sai_object_interface.cpp
SAI_OBJ_IF_OBJ    = $(patsubst %.cpp,${SAI_SRC_DIR}/${OBJ_DIR}/%.o, $(notdir $(SAI_OBJ_IF_SRC)))

GEN_OBJ    = $(patsubst %.cpp,${OBJ_DIR}/%.o, $(GEN_SRC))
SERVER_INC = -I{BM_DIR} -I../inc -I{SAI_INC_DIR}
			
default: server

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

${BM_DIR}/%.o: ${BM_DIR}/%.cpp
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

${SAI_SRC_DIR}/${OBJ_DIR}%.o: ${SAI_SRC_DIR}/%.c
	gcc ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

${SAI_SRC_DIR}/${OBJ_DIR}/%.o: ${SAI_SRC_DIR}/%.cpp
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@ ${STAND_OBJS} ${LIBS_DIRS} ${LIBS}

%.o: %.cpp
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

server: ${GEN_OBJ} ${SAI_OBJS} ${SAI_OBJ_IF_OBJ} SaiCppServer.o
	g++ ${CPP_OPTS} ${CPP_DEFS} -o SaiCppServer ${SERVER_INC} ${INC_DIRS} SaiCppServer.o ${SAI_OBJS} ${SAI_OBJ_IF_OBJ} ${GEN_OBJ} ${STAND_OBJS} ${LIBS_DIRS} ${LIBS}

clean:
	$(RM) -r SaiCppServer *.o