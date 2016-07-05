/*************************************************************************
	> File Name: http_request.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 01 Jul 2016 10:47:57 PM CST
 ************************************************************************/

#include"http_request.h"

static void http_process_ignore(http_request_t *r,http_out_t *out,char *data,int len);
static void http_process_connection(http_request_t *r,http_out_t *out,char *data,int len);
static void http_process_if_modified_since(http_request_t *r,http_out_t *out,char *data,int len);

http_header_handle_t http_headers_in[] = {
    {"Host", http_process_ignore},
    {"Connection", http_process_connection},
    {"If-Modified-Since", http_process_if_modified_since},
    {"", http_process_ignore}
};


int init_request(http_request_t *r, int fd, int epfd)
{
   r->fd = fd;
   r->epfd= epfd;
   r->cgi_flag=0;
   r->pos = r->last = r->buf;
   memset(r->buf,'\0',BUF_SIZE);
   memset(r->method,'\0',METHOD_SIZE);
   memset(r->url,'\0',URL_SIZE);
   memset(r->query,'\0',QUERY_SIZE);
   r->state = 0;

   INIT_LIST_HEAD(&r->list);
   return 0;

}

int init_http_out(http_out_t *out,int fd)
{
    out->fd = fd;
    out->keep_alive = 0;
    out->modified = 1;
    out->status = 0;
	return 0;
}

void  http_header_handle(http_request_t *r,http_out_t *out)
{
	list_head *pos;
	http_head_t *head;
    list_for_each(pos,&(r->list))
	{
	  head = list_entry(pos,http_head_t,list); 
	  http_header_handle_t *cur=http_headers_in; 
	  for(;strlen(cur->name) >0;++cur)
		{
		  char *c=r->key_start;
	      if(strncasecmp(head->key_start,cur->name,head->key_end-head->key_start) == 0)
			{
		     (*(cur->handle))(r,head->value_start,head->value_end-head->value_start,out);
		    }
	   }
	   list_del(pos);
	   free(head);
	}
}

void http_process_ignore(http_request_t *r,http_out_t *out,char *data,int len)
{
    printf("ignore\n");
}


 void http_process_connection(http_request_t *r,http_out_t *out,char *data,int len)
{
    printf("connection\n");
}
 void http_process_if_modified_since(http_request_t *r,http_out_t *out,char *data,int len)
{
    printf("http_process_if_modified_since\n");
}

