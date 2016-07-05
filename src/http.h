/*************************************************************************
	> File Name: http.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 01 Jul 2016 05:31:13 PM CST
 ************************************************************************/

#ifndef _HTTP_H
#define _HTTP_H
#include"comm.h"
#include"threadpool.h"
#include"epoll.h"
#include"http_request.h"


#define MAIN_PAGE "index.html"

void* accept_request(void *arg);
void exe_cgi(int sock_client,const char*path,const char*method,const char*query_string);
void clear_header(int client);
void echo_html(int client,const char* path,unsigned int file_size);
int get_line(int sock,char*buf,size_t max_len);
#endif
