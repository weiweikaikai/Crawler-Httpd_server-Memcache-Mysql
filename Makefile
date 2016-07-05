.PHONY:all clean
PWD=$(shell pwd)
CC=gcc
INCLUDE=-I./thread_pool -I./memory_pool  -I./src
FLAGS= -g #-Wall -DDEBUG
LDFLAGS= -lpthread #-static
BIN=myhttpd  
LIB=
all:$(BIN)
%.o:%.c
	$(CC) $(FLAGS) -c $< -o $@ $(LDFLAGS) $(INCLUDE)
myhttpd:./src/httpd.c ./src/comm.c ./src/epoll.c ./src/http.c ./src/http_parse.c ./src/http_request.c ./thread_pool/threadpool.c
	$(CC) $(FLAGS)  $^ -o $@ $(LDFLAGS) $(INCLUDE)
clean:
	rm -rf  *.o $(BIN)
	


