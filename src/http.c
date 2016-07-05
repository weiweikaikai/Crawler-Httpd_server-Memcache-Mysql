/*************************************************************************
	> File Name: http.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 01 Jul 2016 05:31:07 PM CST
 ************************************************************************/



#include"http.h"

int get_line(int sock,char*buf,size_t max_len)
{
    if(buf == NULL || max_len <0) 
	{
		return 0;
	}
	int i=0;
	char c ='\0'; 
	int n=0;
	while(i<max_len-1 && c!= '\n')
	{
         n= recv(sock,&c,1,0);
		 if(n>0)//succ
		 {
			 if(c == '\r')
			 {
				 n=recv(sock,&c,1,MSG_PEEK);
				 if(n>0 && c=='\n')
				 {
                    recv(sock,&c,1,0);
				 }
				 else
				 {
                    c='\n';
				 }
			 }
            buf[i++] =c;
		 }else//failed
		 {
            c='\n';
		 }
	}
	buf[i]='\0';
	return i;
}



 void echo_html(int client,const char* path,unsigned int file_size)
{
     if(path == NULL)
	 {
         return;
	 }
	 printf("path = %s\n",path);
	 int in_fd = open(path,O_RDONLY);
	 if(in_fd < 0)
	 {
		// echo_error_to_client();
		 return ;
	 }
	 char echo_line[1024];
	 memset(echo_line,'\0',sizeof(echo_line));
     strncpy(echo_line,SERVICE_VERSION,strlen(SERVICE_VERSION)+1);
	 strcat(echo_line," 200 OK\r\n");
	 strcat(echo_line,"content-Type:text/html;Charset=utf-8\r\n\r\n");
     send(client,echo_line,strlen(echo_line),0);
     printf("service say: %s\n",echo_line);
	 if(sendfile(client,in_fd,NULL,file_size) < 0)
	 {
          close(in_fd);
		  return;
	 }
	 printf("ahhhahah\n");
	 close(in_fd);
}

 void clear_header(int client)
{
    char buf[1024];
	memset(buf,'\0',sizeof(buf));
     int ret=0;
   do
   {
       ret=get_line(client,buf,sizeof(buf));
   }while(ret>0 && strcmp(buf,"\n" ) != 0);
}


void exe_cgi(int sock_client,const char*path,const char*method,const char*query_string)
{
	char buf[BUF_SIZE];
	int content_length=0;
    int numchars = 0;
	int cgi_input[2]={0,0};
	int cgi_output[2]={0,0};

   if(strcasecmp(method,"GET") == 0)//GET
   {
	   clear_header(sock_client);
   }
   if(strcasecmp(method,"POST") == 0)//POST 
   {
        do
		{
			memset(buf,'\0',sizeof(buf));
			numchars =get_line(sock_client,buf,sizeof(buf));
			if(strncasecmp(buf,"Content-Length:",15) == 0)
			{
             content_length = atoi(&buf[16]);
			}
		}while(numchars>0 && strcmp(buf,"\n") != 0);
   }
   if(content_length == -1)
   {
	//echo_error_to_client();
	return;
   }
   memset(buf,'\0',sizeof(buf));
   strcpy(buf,SERVICE_VERSION);
    strcat(buf," 200 OK\r\n");
   strcat(buf,"content-Type:text/html;Charset=utf-8");
	 strcat(buf,"\r\n\r\n");
   send(sock_client,buf,strlen(buf),0);
    
   if(pipe(cgi_input) == -1)
   {
	   return ;
   }
   if(pipe(cgi_output) == -1)
   {
	   close(cgi_input[0]);
	   close(cgi_input[1]);
	   return ;
   }
   pid_t id = fork();
   if(id <0)
   {
	   close(cgi_input[0]);
	   close(cgi_input[1]);
	   close(cgi_output[0]);
	   close(cgi_output[1]);
	   return ;
   }else if(id == 0)
   {
          char query_env[BUF_SIZE/10];
          char method_env[BUF_SIZE];
          char content_len_env[BUF_SIZE];
		  memset(method_env,'\0',sizeof(method_env));
		  memset(query_env,'\0',sizeof(query_env));
		  memset(content_len_env,'\0',sizeof(content_len_env));

		  close(cgi_input[1]);
		  close(cgi_output[0]);

		  dup2(cgi_input[0],0);
		  dup2(cgi_output[1],1);

		  sprintf(method_env,"REQUEST_METHOD=%s",method);
		  putenv(method_env);
		  if(strcasecmp("GET",method) == 0) //GET
		  {
           sprintf(query_env,"QUERY_STRING=%s",query_string);
		   putenv(query_env);
		  }else //POST
		  { 
           sprintf(content_len_env,"QUERY_STRING=%s",content_length);
           putenv(content_len_env);
		  }
     execl(path,path,NULL);
	 exit(1);
   }
   else //father
   {
      close(cgi_input[0]);
	  close(cgi_output[1]);
      
	  int i=0;
	  char c='\0';
	  if(strcasecmp("POST",method) == 0)
	  {
		  for(;i<content_length;++i)
		  {
	        recv(sock_client,&c,1,0);
			write(cgi_input[1],&c,1);
		  }
	  }
	  while(read(cgi_output[0],&c,1) > 0)
	  {
		  send(sock_client,&c,1,0);
	  }
     close(cgi_input[1]);
	 close(cgi_output[0]);
     
	 waitpid(id,NULL,0);
   }

}

void* accept_request(void *arg)
{
	http_request_t *r = (http_request_t*)arg;
    int sock_client = r->fd;
    int num =readn(sock_client,(void*)r->buf,BUF_SIZE);
     r->last += num;
	 printf("buf= %s\n",r->buf);
	 http_parse_method_line(r);
	 http_parse_field_line(r);
	 http_out_t *out = (http_out_t *)malloc(sizeof(http_out_t));
	 init_http_out(out,sock_client);
	 http_header_handle(r,out);
	 printf("method: %s\n",r->method);
	 printf("url %s\n",r->url);
	 printf("query: %s\n",r->query);
	 if(strcmp(r->url,"/") == 0)
	{
	 sprintf(r->url, "htdocs/");
    }
	if(r->url[strlen(r->url)-1] == '/')
    {
      strcat(r->url,MAIN_PAGE);
    }
	printf("url %s\n",r->url);
    struct stat st;
	if(stat(r->url,&st) <0)//failed return not zero
	{
		clear_header(sock_client);
		// echo_error_to_client();
		 return NULL;  
	}else //file exist
	{
		printf("cun zai\n");
           if(S_ISDIR(st.st_mode))
		   {
		      strcat(r->url,"/");
              strcat(r->url,MAIN_PAGE);
		   }
	   	   else if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH)
	      {
           r->cgi_flag =1;
	      }
		  if(r->cgi_flag)
		   {
		     exe_cgi(sock_client,r->url,r->method,r->query);
		   }
		   else
		   {
		        //clear_header(sock_client);
                 echo_html(sock_client,r->url,st.st_size);
		   }
	} 

	close(sock_client);
	return NULL;
//	 int i=0;
//	 int j=0;//buf line index
//
//	 //get method
//     while(!isspace(buf[j]) && i<sizeof(method)-1 && j<sizeof(buf)  )
//	 {
//		 method[i] = buf[j];
//		 i++;
//		 j++;
//	 }
//	 //clear space 
//	 while(isspace(buf[j]) && j < sizeof(buf))
//	 {
//		 ++j;
//	 }
//	 //get url
//	 i=0;
//	 while(!isspace(buf[j]) && i<sizeof(url)-1 && j<sizeof(buf))
//	 {
//            url[i] = buf[j];
//			i++;
//			j++;
//	 }
//	 //printf("method %s  url %s\n",method,url);
//	 if(strcasecmp(method,"GET")<0 && strcasecmp(method,"POST")<0)
//	 {
//		// echo_error_to_client();
//		 return NULL;
//	 }
//	 if(strcasecmp(method,"POST") == 0)
//	 {
//		 cgi = 1;
//	 }
//	 if(strcasecmp(method,"GET") == 0)
//	 {
//           query_string = url;
//           while(*query_string != '?' && *query_string != '\0')
//		   {
//			   query_string++;
//		   }
//		   if(*query_string == '?') //url = /add/index?val=0
//		   {
//              *query_string = '\0';
//			   query_string++;
//			   cgi = 1;
//		   }
//	 }
//	 sprintf(path,"htdocs%s",url);
//     if(path[strlen(path)-1] == '/')
//	 {
//		 strcat(path,MAIN_PAGE);
//	 }
//
//    struct stat st;
//	if(stat(path,&st) <0)//failed return not zero
//	{
//		clear_header(sock_client);
//		// echo_error_to_client();
//		 return NULL;  
//	}else //file exist
//	{
//           if(S_ISDIR(st.st_mode))
//		   {
//		      strcat(path,"/");
//              strcat(path,MAIN_PAGE);
//		   }
//		   else if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH)
//		   {
//               cgi =1;
//		   }
//		   else
//		   {
//
//		   }
//		   if(cgi == 1)
//		   {
//              exe_cgi(sock_client,path,method,query_string);
//		   }else
//		   {
//		        clear_header(sock_client);
//                echo_html(sock_client,path,st.st_size);
//		   }
//	}
//	close(sock_client);
//	return NULL;
}

