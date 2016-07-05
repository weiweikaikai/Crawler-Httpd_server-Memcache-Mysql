INCLUDE=-I../sql_connect -I../sql_connect/mysql_lib/include -I./ -I/usr/local/libmemcached/include -I../memcache/
LIB=-L../sql_connect/mysql_lib/lib -lmysqlclient -L/usr/local/libmemcached/lib -lmemcached
BIN=find_cgi
SRC=find_cgi.cpp  ../sql_connect/sql_connect.cpp
FLAG=-g

$(BIN):$(SRC)
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(FLAG) 

.PHONY:output
output:
	cp find_cgi ../htdocs/cgi_bin 
.PHONY:clean
clean:
	rm -rf  find_cgi
