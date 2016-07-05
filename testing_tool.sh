#!/bin/bash
cat "memory:"
free -m | awk {'print  $2 '} | sed -n '1,2p'

echo "cat /proc/sys/fs/file-nr "
cat /proc/sys/fs/file-nr | awk '{print $3}'

echo "cat /etc/sysctl.conf | grep fs.file-max"
cat /etc/sysctl.conf | grep fs.file-max

echo "cat /etc/security/limits.conf"
cat /etc/security/limits.conf  | grep "nofile"

echo "ulimit -n:" 
ulimit -n

echo "open_file_num  PID"
lsof -n|awk '{print $2}'|sort|uniq -c|sort -nr|more| grep `ps -ef | grep "./myhttpd" | awk '{print $2}' | sed -n '1p'`

TIME_WAIT_NUM=`netstat -n | awk '/^tcp/{++S[$NF]}END {for(a in S) print a,S[a]}' | grep 'TIME_WAIT' | awk '{print $2}'`

echo "timeout num:"
echo $TIME_WAIT_NUM

cat "after memory:"
free -m | awk {'print  $2 '} | sed -n '1,2p'

echo "tcp_wmen:"
cat /proc/sys/net/ipv4/tcp_wmem
echo "tcp_rmen:"
cat /proc/sys/net/ipv4/tcp_rmem