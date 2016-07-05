/*************************************************************************
	> File Name: http_request.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 01 Jul 2016 10:47:53 PM CST
 ************************************************************************/

#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H
#include"comm.h"
#include"list.h"

//标志method的类型
#define HTTP_UNKNOWN                     0x00000001
#define HTTP_GET                         0x00000002
#define HTTP_HEAD                        0x00000004
#define HTTP_POST                        0x00000008

#define BUF_SIZE 4096
#define METHOD_SIZE 8
#define URL_SIZE  128
#define QUERY_SIZE 128

typedef struct http_request
{
    int fd;
	int epfd;

    int cgi_flag; //是否执行cgi程序

    int method_flag;
	char buf[BUF_SIZE];
	unsigned char *pos;
	unsigned char *last;

    char method[METHOD_SIZE];
	unsigned char*request_start;
	unsigned char* method_end;

    char url[URL_SIZE];
	unsigned char *url_start;
	unsigned char *url_end;

    char query[QUERY_SIZE];
	unsigned char*query_start;
	unsigned char *query_end;

	unsigned char *http_start;
	unsigned char *http_end;
	int state;

    unsigned char *crcf_start;
	unsigned char *crcf_end;
    list_head list;  /* store http header */

	unsigned char *key_start;
	unsigned char *key_end;
	unsigned char *value_start;
	unsigned char *value_end;

}http_request_t;

int init_request(http_request_t *r, int fd, int epfd);

typedef struct http_out{
    int fd;
    int keep_alive;
    time_t mtime;       /* the modified time of the file*/
    int modified;       /* compare If-modified-since field with mtime to decide whether the file is modified since last time*/

    int status;
}http_out_t;


typedef void (*http_header_ptr)(http_request_t *r,http_out_t *out,char *data,int len);

typedef struct head_handle_map
{
	char*name;
    http_header_ptr handle;
}http_header_handle_t;



int init_http_out(http_out_t *out,int fd);

void  htto_header_handle(http_request_t *r);
typedef struct http_head
{
	unsigned char * key_start;
    unsigned char * key_end;
    unsigned char * value_start;
    unsigned char * value_end;
	list_head list;
}http_head_t;




#endif
