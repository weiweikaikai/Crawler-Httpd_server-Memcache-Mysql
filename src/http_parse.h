/*************************************************************************
	> File Name: http_parse.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 01 Jul 2016 10:48:10 PM CST
 ************************************************************************/

#ifndef _HTTP_PARSE_H
#define _HTTP_PARSE_H
#include"http_request.h"

#define CR '\r'
#define LF '\n'
#define CRLFCRLF "\r\n\r\n"

//将字符串转化为整数进行比较减少CPU的比较次数
#define str_cmp(m, c0, c1, c2, c3)                                       \
	*(unsigned int *)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)



int http_parse_method_line(http_request_t *r); //处理客户端方法请求行
int http_parse_field_line(http_request_t *r);  //处理客户端的头部域

#endif
