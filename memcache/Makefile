BIN=mem
SRC=memcache.cpp
FLAG=-g
INCLUDE=-I/usr/local/libmemcached/include
LIB=-L/usr/local/libmemcached/lib -lmemcached

$(BIN):$(SRC)
	g++ $^ -o $@ $(INCLUDE) $(LIB)
.PHONY:clean
clean:
	rm -rf $(BIN)

