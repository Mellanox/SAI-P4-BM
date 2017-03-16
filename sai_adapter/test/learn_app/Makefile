THRIFT_VER =thrift-0.9.2
USR_DIR    =${HOME}/usr
THRIFT_DIR =${USR_DIR}/${THRIFT_VER}
SAI_INC_DIR    = ../../inc/
SAI_SRC_DIR = ../../src
LIBS_DIRS  =-L${USR_DIR}/lib -L${USR_DIR}/${THRIFT_VER}/lib 
CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O2 -std=c++11 -w
LIBS       =-lthrift ${LIBS_DIRS}
BM_DIR     = ../../../p4-softswitch/thrift_src/gen-cpp/bm
SAI_THRIFT_SRC_DIR    = ../sai_thrift_src/gen-cpp

STAND_OBJS = ${BM_DIR}/Standard.o  \
             ${BM_DIR}/standard_types.o \
             ${BM_DIR}/standard_constants.o

INC_DIRS  =-I${USR_DIR}/include -I${THRIFT_DIR}/include/thrift -I${SAI_INC_DIR} -I${BM_DIR} -I${SAI_THRIFT_SRC_DIR}
OBJ_DIR = obj
SAI_SRC =  $(wildcard ${SAI_SRC_DIR}/*.c)
SAI_SRC_HEADERS = $(wildcard ${SAI_SRC_DIR}/*.h)
SAI_OBJS = $(patsubst %.c,${SAI_SRC_DIR}/${OBJ_DIR}/%.o, $(notdir $(SAI_SRC)))
SAI_OBJ_IF_SRC    = ${SAI_SRC_DIR}/sai_object_interface.cpp ${SAI_SRC_DIR}/sai_object.cpp 
SAI_OBJ_IF_OBJ    = $(patsubst %.cpp,${SAI_SRC_DIR}/${OBJ_DIR}/%.o, $(notdir $(SAI_OBJ_IF_SRC)))
LEARN_SRC = $(wildcard *.c)
LEARN_OBJ = $(patsubst %.c,%.o,${LEARN_SRC})

GEN_OBJ    = $(patsubst %.cpp,${OBJ_DIR}/%.o, $(GEN_SRC))
			
default: learn_app

%.o: %.c
	gcc ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

learn_app: ${LEARN_OBJ}
	g++ ${CPP_OPTS} ${CPP_DEFS} ${LEARN_OBJ} -I${SAI_INC_DIR} ${SAI_OBJS} ${SAI_OBJ_IF_OBJ} ${STAND_OBJS} -o learn_app ${LIBS}

clean:
	$(RM) -r SaiCppServer *.o