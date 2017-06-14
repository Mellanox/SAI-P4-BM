CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Wall -O3 -std=c++11 -pthread -w

BM_DIR     = ../thrift_src/gen-cpp/bm
HEADERS = ${BM_DIR}/standard_constants.h \
		  ${BM_DIR}/Standard.h \
		  ${BM_DIR}/standard_types.h \
		  ${BM_DIR}/simple_pre_lag_constants.h \
		  ${BM_DIR}/SimplePreLAG.h \
		  ${BM_DIR}/simple_pre_lag_types.h

SRC     = ${BM_DIR}/Standard.cpp  \
		  ${BM_DIR}/standard_types.cpp \
		  ${BM_DIR}/standard_constants.cpp \
		  ${BM_DIR}/simple_pre_lag_constants.cpp \
		  ${BM_DIR}/SimplePreLAG.cpp \
		  ${BM_DIR}/simple_pre_lag_types.cpp 

OBJECTS = ${BM_DIR}/Standard.o  \
		  ${BM_DIR}/standard_types.o \
		  ${BM_DIR}/standard_constants.o \
		  ${BM_DIR}/SimplePreLAG.o  \
		  ${BM_DIR}/simple_pre_lag_types.o \
		  ${BM_DIR}/simple_pre_lag_constants.o

default: libp4bm

${BM_DIR}/%.o: ${BM_DIR}/%.cpp ${HEADERS}
	g++ ${CPP_OPTS} ${CPP_DEFS} -c $< -o $@

libp4bm: ${OBJECTS}
	# g++ -shared -o libp4bm.so ${OBJECTS}
	ar rvs libp4bm.a ${OBJECTS}

clean: 
	rm -rf ${OBJECTS}