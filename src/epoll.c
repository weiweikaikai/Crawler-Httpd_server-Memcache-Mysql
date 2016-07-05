/*************************************************************************
	> File Name: epoll.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Thu 02 Jun 2016 11:49:55 AM CST
 ************************************************************************/

#include "epoll.h"



int My_epoll_create(int flags) {
    int fd = epoll_create1(flags);

    events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    return fd;
}

void My_epoll_add(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event);
    return;
}

void My_epoll_mod(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, event);
    return;
}

void My_epoll_del(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, event);
    return;
}

int My_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
    int n = epoll_wait(epfd, events, maxevents, timeout);
    return n;
}
