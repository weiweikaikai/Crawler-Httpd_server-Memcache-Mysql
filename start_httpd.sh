#!/bin/bash

WORK_PATH=$(pwd)


WHOAMI=`whoami`
PID=`ps -u $WHOAMI | grep myhttpd | awk '{print $1}'`

if (test "$#" = 0); then
	echo "Usage: $0 [stop] [start] [status]"
	exit 0
fi

if (test "$1" = "start"); then
	if (test "$PID" = ""); then
           ./start_memcache.sh start
           ./start_mysql.sh    start
          ./myhttpd 121.42.180.114 9000
	else
		echo "myhttpd is running"
	fi
	exit 0
fi

if (test "$1" = "stop"); then
	if (test "$PID" != ""); then
		kill -s 2 $PID
	   ./start_memcache.sh stop
           ./start_mysql.sh    stop
	fi
	exit 0
fi

if (test "$1" = "status"); then
	if (test "$PID" = ""); then
		echo "myhttpd is not running"
	else
		echo "myhttpd is running"
	fi
	./start_memcache.sh status
        ./start_mysql.sh    status
	exit 0
fi

echo "Usage: $0 [stop] [start] [status]"

TIME_WAIT_NUM=`netstat -n | awk '/^tcp/{++S[$NF]}END {for(a in S) print a,S[a]}' | grep 'TIME_WAIT' | awk '{print $2}'`

PATH=`pwd`
echo $TIME_WAIT_NUM >  $PATH/log/httpd.log

