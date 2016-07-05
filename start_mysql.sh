#!/bin/bash

if (test "$#" = 0); then
	echo "Usage: $0 [stop] [start] [status]"
	exit 0
fi

if (test "$1" = "start"); then
	service mysqld start
fi

if (test "$1" = "stop"); then
	service mysqld stop
fi

if (test "$1" = "status"); then
	service mysqld status
fi


