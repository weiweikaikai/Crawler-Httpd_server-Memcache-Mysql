#!/bin/bash

WORK_PATH=$(pwd)

WHOAMI=`whoami`
PID=`ps -u $WHOAMI | grep memcached | awk '{print $1}'`

if (test "$#" = 0); then
	echo "Usage: $0 [stop] [start] [status]"
	exit 0
fi

if (test "$1" = "start"); then
	if (test "$PID" = ""); then
         /usr/local/memcache/bin/memcached -d -u root -m 256 -l 127.0.0.1 -p 11211 -c 4096 -VV 
	else
		echo "memcached is running"
	fi
	exit 0
fi

if (test "$1" = "stop"); then
	if (test "$PID" != ""); then
		kill -s 2 $PID
	fi
	exit 0
fi

if (test "$1" = "status"); then
	if (test "$PID" = ""); then
		echo "memcached is not run"
	else
		echo "memcached is running"
	fi
	exit 0
fi

echo "Usage: $0 [stop] [start] [status]"


