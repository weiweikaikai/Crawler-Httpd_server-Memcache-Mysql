/*************************************************************************
	> File Name: epoll.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Thu 02 Jun 2016 11:50:00 AM CST
 ************************************************************************/

#ifndef _EPOLL_H
#define _EPOLL_H

#include <sys/epoll.h>
#include<stdlib.h>
#define MAXEVENTS 1024

struct epoll_event *events;

int  My_epoll_create(int flags);
void My_epoll_add(int epfd, int fs, struct epoll_event *event);
void My_epoll_mod(int epfd, int fs, struct epoll_event *event);
void My_epoll_del(int epfd, int fs, struct epoll_event *event);
int  My_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);


#endif
