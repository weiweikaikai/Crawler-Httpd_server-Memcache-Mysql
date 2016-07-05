/*************************************************************************
    > File Name: httpd.c
    > Author: WK
    > Mail:18402927708@163.com 
    > Created Time: Thu 03 Dec 2015 03:07:53 PM CST
 ************************************************************************/

#include"http.h"

void usage(const char* proc)
{
      printf("usage %s [ip][port]\n",proc);
}
int main(int argc,char* argv[])
{
	if(argc != 3)
	{
    usage(argv[0]);
	 exit(1);
	}
    short port = atoi(argv[2]);
    const char* ip=argv[1];
    // daemon(1,1);
     int listenfd = Server_init(ip,port);
	 if(listenfd < 0)
	 {
		 printf("func Server_init  err\n");
		 return -1;
	 }
	 activate_nonblock(listenfd);
	 int epfd=My_epoll_create(0);//这里可以加类似于EPOLL_CLOEXEC的fd属性值
	 struct epoll_event event;

     http_request_t *request =(http_request_t*)malloc(sizeof(http_request_t));
	 //后期使用内存池分配内存
     init_request(request,listenfd,epfd);
	 event.data.ptr= (void*)request;
	 event.events = EPOLLIN | EPOLLET;
	 My_epoll_add(epfd,listenfd,&event);
      
    threadpool_t *tp = threadpool_init(5);

     int timeout=-1;
      int count=0;
	while(1)
	{
		
		int activenum=My_epoll_wait(epfd,events,MAXEVENTS,timeout);
		if(activenum < 0)
		{
		     printf("epoll_wait error\n");
			 break;
		}
		if(activenum == 0)
		{
		    printf("epoll_wait timeout\n");
			continue;
		}
		int i=0;
		for(;i<activenum;++i)
		{
		  http_request_t *r =events[i].data.ptr;
		  int fd = r->fd;
		  if(listenfd == fd)
			{
		       int conn=0;
			   while(1)
				{
				     struct sockaddr_in clientaddr;
					 socklen_t len = sizeof(clientaddr);
	                 memset(&clientaddr, 0, sizeof(clientaddr));
			          conn = accept(listenfd, (struct sockaddr *)&clientaddr, &len);
                    if (conn < 0) 
						{
                           if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
							{
							   //处理了所有输入连接
                               break;
                            } else{
								printf("accept error\n");
                                break;
                            }   
			             }
						 ++count;
					printf("%d  a client online ip= %s port= %d\n ",count, inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
				    activate_nonblock(conn);
					  http_request_t *new_request =(http_request_t*)malloc(sizeof(http_request_t));
	                 //后期使用内存池分配内存
                    init_request(request,conn,epfd);
					event.data.ptr = (void*)new_request;
					event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
					My_epoll_add(epfd, conn, &event);
		         }
				 
		     }else{
			          if ((events[i].events & EPOLLERR) ||
                          (events[i].events & EPOLLHUP) ||
                          (!(events[i].events & EPOLLIN))) 
					    {
                          printf("error fd\n");
                          close(fd);
                          continue;
                        }
			           if(threadpool_add(tp,accept_request,(void*)events[i].data.ptr) != 0)
				         {
					          printf("threadpool_add error\n");
					     }
			  }
	     }
	}
       if (threadpool_destroy(tp,0) < 0)
		   {
              printf("destroy threadpool failed");
            }

        return  0;
}

