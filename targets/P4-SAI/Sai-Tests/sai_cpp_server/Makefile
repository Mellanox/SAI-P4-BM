THRIFT_VER =thrift-0.9.2
USR_DIR    =${HOME}/usr
THRIFT_DIR =${USR_DIR}/${THRIFT_VER}
INCS_DIRS  =-I${USR_DIR}/include -I${THRIFT_DIR}/include/thrift
LIBS_DIRS  =-L${USR_DIR}/lib -L${USR_DIR}/${THRIFT_VER}/lib -L../inc
CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O2 -std=c++11
LIBS       =-lthrift
BM_DIR     = ../../../../thrift_src/gen-cpp/bm
STAND_OBJS = {BM_DIR}/Standard.o  \
             {BM_DIR}/standard_types.o \
             {BM_DIR}/standard_constants.o \

GEN_SRC    = switch_sai_types.cpp switch_sai_constants.cpp switch_sai_rpc.cpp 
SRC_DIR    = ../sai_thrift_src/gen-cpp
OBJ_DIR    = obj
GEN_OBJ    = $(patsubst %.cpp,${OBJ_DIR}/%.o, $(GEN_SRC))
SERVER_INC = -I{BM_DIR} -I../inc
			
default: server

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	g++ ${CPP_OPTS} ${CPP_DEFS} $(INCS_DIRS) -c $< -o $@

server: ${GEN_OBJ}
	g++ ${CPP_OPTS} ${CPP_DEFS} -o SaiCppServer ${SERVER_INC} ${INCS_DIRS} SaiCppServer.cpp ${GEN_OBJ} ${LIBS_DIRS} ${LIBS}

clean:
	$(RM) -r SaiCppServer *.o