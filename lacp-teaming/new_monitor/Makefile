CPP_DEFS = -D=HAVE_CONFIG_H
CPP_OPTS = -Wall -O3 -std=c++11
TEAM_DIR = ../libteam
TEAM_INC = -I${TEAM_DIR}/include
TEAM_LIB = -lteam

MONITOR_SRC = $(wildcard *.c)
MONITOR_OBJ = $(patsubst %.c,%.o, $(MONITOR_SRC))

INC_DIRS = ${TEAM_INC}
LIBS = ${TEAM_LIB}
OBJECTS = ${MONITOR_OBJ}

default: new_monitor

%.o: %.c
	gcc ${CPP_OPTS} ${CPP_DEFS} $(INC_DIRS) -c $< -o $@

new_monitor: ${OBJECTS}
	gcc ${CPP_OPTS} ${CPP_DEFS} ${OBJECTS} ${INC_DIRS} -o new_monitor ${LIBS} 

