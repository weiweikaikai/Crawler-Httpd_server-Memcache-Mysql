WORK_PATH=$(shell pwd)

BIN=sql_connect
CC=g++
INCLUDE= -I${WORK_PATH}/mysql_lib/include -I./
LIB_PATH= -L${WORK_PATH}/mysql_lib/lib
SRC=sql_connect.cpp

LIB_NAME=-lmysqlclient

${BIN}:${SRC}
	${CC} -o $@ $^ ${INCLUDE} ${LIB_PATH} $(LIB_NAME) -g

.PHONY:clean
clean:
	rm -f ${BIN}

