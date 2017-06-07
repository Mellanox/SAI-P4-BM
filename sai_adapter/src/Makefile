THRIFT_VER =thrift-0.9.2
USR_DIR    =${HOME}/usr
THRIFT_DIR =${USR_DIR}/${THRIFT_VER}
LIBS_DIRS  =-L${USR_DIR}/lib -L${USR_DIR}/${THRIFT_VER}/lib -L../inc 
CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O3 -std=c++11 -pthread -w
LIBS       =-lthrift -lpcap ${LIBS_DIRS}
BM_DIR     = ../../p4-softswitch/thrift_src/gen-cpp/bm

STAND_SRC  = ${BM_DIR}/Standard.cpp  \
             ${BM_DIR}/standard_types.cpp \
             ${BM_DIR}/standard_constants.cpp
STAND_OBJS = ${BM_DIR}/Standard.o  \
             ${BM_DIR}/standard_types.o \
             ${BM_DIR}/standard_constants.o
SIMPLE_PRE_LAG_OBJS = ${BM_DIR}/SimplePreLAG.o  \
             ${BM_DIR}/simple_pre_lag_types.o \
             ${BM_DIR}/simple_pre_lag_constants.o

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
OBJECTS = $(SAI_OBJ) $(SAI_OBJ_IF_OBJ) $(STAND_OBJS) $(SIMPLE_PRE_LAG_OBJS)
default = libp4sai

.PHONY: libp4sai clean

# default: sai_adapter

${OBJ_DIR}/%.o: %.cpp ${HEADERS}
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@ ${LIBS}

${OBJ_DIR}/%.o: %.c
	gcc ${SAI_INC} -c $< -o $@

# all: $(SAI_OBJ) $(SAI_OBJ_IF_OBJ)

libp4sai: ${OBJECTS}
	ar rvs ../lib/libp4sai.a ${OBJECTS}

clean:
	rm -f $(SAI_OBJ) $(SAI_OBJ_IF_OBJ)