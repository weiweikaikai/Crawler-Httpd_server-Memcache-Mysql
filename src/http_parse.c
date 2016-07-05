/*************************************************************************
	> File Name: http_parse.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 01 Jul 2016 10:48:06 PM CST
 ************************************************************************/

#include"http_parse.h"
int  http_parse_method_line(http_request_t *r) //处理客户端方法请求行
{
	int pos_method=0;
	int pos_url=0;
	int pos_query=0;
    enum
	{
	  start=0,
	  method,
	  url,
	  query,
	  http,
	  http_version_digit,
	  crlf
	}state;

   state = r->state;
  unsigned char *cur=r->pos;
  int flag = 1;
  for(;cur<r->last&&flag;++cur)
  {
	 unsigned char tmp_char = *cur;
     switch(state) //状态机编程
	 {
	    case start:
			r->request_start = cur;
		  state = method;
		  r->method[pos_method++]=*cur;
		  break;
		case method: //GET / HTTP/1.0
			if(tmp_char == ' ')
		    {
			  r->method_end = cur; //这里将' '也作为方法的一部分
			  switch(r->method_end - r->request_start)
				{
				  case 3:
				      if(str_cmp(r->request_start,'G','E','T',' '))
				       {
				        r->method_flag = HTTP_GET;
						break;
			           }
				  case 4:
					    if(str_cmp(r->request_start,'P','O','S','T'))
				       {
				        r->method_flag = HTTP_POST;
						break;
			           }
					    if(str_cmp(r->request_start,'H','E','A','D'))
				       {
				        r->method_flag = HTTP_HEAD;
						break;
			           }
					   break;
				default:
                   break;
				}
		        state = url;
			    break;
		 	}
			//不等于空格继续向后移动指针
			r->method[pos_method++]=*cur;
		    break;
	    case url:
			if(tmp_char == '/')
		    {
			  r->url_start = cur;
		    }
			else if(tmp_char == '?')
		     {
			 r->url_end = cur;
			 r->query_start = cur+1;
			 state = query;
			  break;
			}
			else if(tmp_char == ' ')
		    {
			  r->query_end = cur;
			  state = http;
			  break;
			} 
			r->url[pos_url++]=*cur;
			break;
		case query:
			if(tmp_char == ' ')
		   {
		      r->query_end = cur;
			  state = http;
			  break;
		   }
		   r->query[pos_query++]=*cur;
		   break;
		case http:
            if(tmp_char == 'H')
		    {
			  r->http_start=cur;
		    }
			else if(tmp_char == 'P')
		     {
			  if(str_cmp(r->http_start,'H','T','T','P'))
				 {
				  r->http_end= cur;
				  state = http_version_digit;
			    }
			 }
			break;
		case http_version_digit:
			 if(tmp_char == '.')
		     {
			    state = crlf;
			     break;
			 }
			 break;
		 case crlf:
			 if(tmp_char == 0xa)//LF
			 {
			  flag = 0;
		      r->pos = cur+1;
			  break;
			 }
		    else if(tmp_char == 0xd)  //CR
		   {
             if(*(cur+1) == 0xa)//LF
		     {
			  flag =0;
			  r->pos = cur+2;
			  break;
			 }
		   }
		   break;
	 }
  }
  return 0;

}
int http_parse_field_line(http_request_t *r) //处理客户端的头部域
{
	http_head_t *head;
	enum{
	 body_start=0,
	 key,
	 value,
	 crlfcrlf
	}state;
	 state = r->state;
    unsigned char *cur=r->pos;
	int flag = 1;
  for(;cur<r->last&&flag;++cur)
	{
	  char tmp_char =*cur;
       switch(state)
		{
		   case body_start:
                 if(tmp_char == 0xa)
		         {
			      break;
			     }
				 r->key_start =cur;
				state = key;
				break;
		    case key:
					if(tmp_char == ':')
		            {
				      r->key_end=cur;
					  r->value_start = cur+1;
					  state = value;
					  break;
				    }
					break;
			case value:
				   if(tmp_char == 0xa)
		            {
                       r->value_end=cur;
					   r->crcf_start = (cur+1);
					   break;
					}
					else if(tmp_char == 0xd )
			          {
					   if(*(cur+1) == 0xa)
						  {
					        r->value_end=cur;
							 r->crcf_start = (cur+1);
					         if(str_cmp( r->crcf_start,'0x0d','0x0a','0x0d','0x0a'))
			                    {
				                   flag =0;
								   break;
				                }
							else
							  {
								state = body_start;
							  }
							       head = (http_head_t *)malloc(sizeof(http_head_t));
							  	   head->key_start=r->key_start;
								   head->key_end=r->key_end;
								   head->value_start=r->value_start;
								   head->value_end=r->value_end;
								    list_add(&(head->list), &(r->list));
                   			   break;
					   }
					 }
					break;	 
			default:
					  break;
		}

  }
  
  return 0;
}