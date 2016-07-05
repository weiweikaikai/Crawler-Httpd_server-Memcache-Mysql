#include<string>  
#include<iostream>  
#include<vector>  
#include<time.h>  
#include<queue>  
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<algorithm>
#include<unistd.h>
#include<signal.h>
#include<map>
#include"sql_connect.h"
using namespace std;  


class data
{
public:
    data(vector<string>&d,char* city_name,vector<string> &we,vector<string> &l_degree,vector<string>&hig_degree):date(d),name(city_name),weather(we),low_degree(l_degree),high_degree(hig_degree)
	{}
    void SaveToSql()
    {
    for(int i=0;i<14;++i)
    {
    cout<<date[i]<<" "<<name<<endl;
    }
    }
    vector<string> date;
	string name;
    vector<string> weather;
    vector<string> low_degree;
	vector<string> high_degree;
};

bool GetHttpResponse(string &resource,char * &pageBuf)
{
string host = "tianqi.114la.com";
struct hostent *hp;
hp = gethostbyname(host.c_str());

if(hp == NULL)
{
cout<<"can't find host address\n";
return false;
}
int sock = socket(PF_INET,SOCK_STREAM,0);

if(sock < 0)
{
 cout<<"create socket error\n";
  return false;
}
struct sockaddr_in address;
bzero(&address,sizeof(address));
address.sin_family = AF_INET;
memcpy(&address.sin_addr.s_addr,hp->h_addr_list[0],hp->h_length);
address.sin_port = htons(80);

int ret = connect(sock,(struct sockaddr*)&address,sizeof(address));

if(ret < 0)
{
  printf("error: connect\n");
  close(sock);
  return false;
}

string request = "GET " + resource+ " HTTP/1.1\r\nHost:"+host+"\r\nConnection:close\r\n\r\n";

if(send(sock,request.c_str(),request.size(),0) < 0)
{
printf("send error\n");
close(sock);
return false;
}

int SZ=4096;
pageBuf = (char*)malloc(SZ);
memset(pageBuf,0,SZ);

ret = 1;
int bytesread =0;
while(ret > 0)
{
ret = recv(sock,pageBuf+bytesread,SZ-bytesread,0);
if(ret > 0)
{
bytesread +=ret;
}

if(SZ-bytesread < 100)
{
    SZ*=2;
   pageBuf = (char*)realloc(pageBuf,SZ);

}
}

pageBuf[bytesread] = '\0';

close(sock);
return true;
}


void UncodeTostr(char in[],char out[] )
{
   ssize_t len =strlen(in);
   char *p=in;
     char buf[20][30];
   memset(buf,'\0',sizeof(buf));
    
	 for(int i=0;i<len;++i)
	{
      sprintf(buf[i],"%x",p[i]);
    }
        int cur=0;
	 for(int i=0;i<20;++i)
	{
		 if(strlen(buf[i]) >= 8)
		{
	        for(int j=0;j<9;++j)
	        {
                if(buf[i][j] != 'f' && buf[i][j] != '\0')
		         {
			           out[cur++]=buf[i][j];
		         }
	         }
			// cout<<mm[i]<<" ";
		}
	}
	for(int i=0;i<strlen(out);++i)
	{
		if(out[i] > '9')
	      out[i] &= ~0x20;
	}
}

void find_resource(char *resp,map<string,string> &city_name_url)
{
char *tag1 = "inlandCity";
char *tag2 = "href=\"";
char *tag3 = "self\">";
char *pos = strstr(resp,tag1);
if(pos == NULL)
{
cout<<"can't find inlandCity"<<endl;
}

int count=0;
char url[200]={'\0'};
char city_name[50]={'\0'};
char cityname[50]={'\0'};

while(pos)
{
 memset(url,'\0',200);
 memset(city_name,'\0',50);
pos = strstr(pos,tag2);
if(pos == NULL)
{
cout<<"can't find "<<tag2<<endl;
}
pos += strlen(tag2);
int readnum = sscanf(pos,"%[^\"]",url);
//cout<<url<<"-->";
   pos = strstr(pos,tag3);
   if(pos == NULL)
  {
    cout<<"can't find "<<tag3<<endl;
  }
   pos += strlen(tag3);
    readnum = sscanf(pos,"%[^<]",city_name);
    UncodeTostr(city_name,cityname);

//cout<<cityname<<endl;
//cout<<count<<endl;
city_name_url.insert(std::pair<string,string>(cityname,url));

    if(++count== 2564)
   {
     break;
   }
}
}

bool  prase(string &str,char *resp)
{
    if(str.size() != 0|| resp == NULL)
    {
    return true;
    }
   char*pos = NULL;
   pos=strstr(resp,"301");
   if(pos == NULL)
   {
//   cout<<"error: strstr\n";
   return true ;
   }
   pos = strstr(resp,"Location:");
   
   pos += 33;
   int i=0;
   while(*pos != '\r')
   {
   str[i++] = *pos++;
   }

   return false;
}
void deleSpace(char *buf)
{
      if(buf == NULL)
	  {
	      return ;
	  }

	  int index=0;
	  for(int i=0;i<strlen(buf)+1;++i)
	  {
		 if(buf[i] != 9 && buf[i] != 32 && buf[i] != 20 && buf[i] != 10 && buf[i] != 13)
		  {
			buf[index++]=buf[i];
	       // printf("%d ",buf[i]);
		  }
      }
	buf[30]='\0';
}
void deleSpaceforweather(char *buf)
{
      if(buf == NULL)
	  {
	      return ;
	  }

	  int index=0;
	  for(int i=0;i<strlen(buf)+1;++i)
	  {
		 if(buf[i] != 48 && buf[i] != 49 && buf[i] != 50 )
		  {
			buf[index++]=buf[i];
	       // printf("%d ",buf[i]);
		  }
      }
	buf[30]='\0';
}
void Parseforcityname(char *buf)
{

     char *cur = buf;
      while(*cur != '\0')
	  {
	      if(*cur == '7')
		  {
		    *cur='\0';
			break;
		  }
        cur++;
	  }
}


void Get_weather_info(char*resp,vector<data> &weather_data)
{
 char *tag1 = "currCity";
 char *tag2 = "_self\"";
 char *tag3 = "<span>";
 char *tag4 = "date\">";
 char *tag5 = "weather1\">";
 char *tag6 = "blue\">";
 char *tag7 = "orange\">";
 
  vector<string> d;
  vector<string> w;
  vector<string> l;
  vector<string> h;
char *pos = strstr(resp,tag1);
if(pos == NULL)
{
	cout<<"can't find "<<tag1<<endl;;
     return ;
}
pos = strstr(pos,tag2);
if(pos == NULL)
{
		cout<<"can't find "<<tag2<<endl;;
     return ;
}
pos = strstr(pos,tag3);
if(pos == NULL)
{
		cout<<"can't find "<<tag3<<endl;;
    return ;
}

pos += strlen(tag3);
char city_name[100]={'\0'};
sscanf(pos,"%[^[]",city_name);
Parseforcityname(city_name);
//printf("%s\n",city_name);

for(int i=0;i<7;++i)
{
pos = strstr(pos,tag4);
if(pos == NULL)
{
	cout<<"can't find "<<tag4<<endl;;
    return ;
}
pos +=strlen(tag4);
char date[100]={'\0'};
sscanf(pos,"%[^<]",date);
deleSpace(date);
//cout<<date<<endl;
d.push_back(date);

pos = strstr(pos,tag5);
if(pos == NULL)
{
		cout<<"can't find "<<tag5<<endl;;
   return ;
}
pos +=strlen(tag5);
char weath[100]={'\0'};
sscanf(pos,"%[^<]",weath);
deleSpace(weath);
deleSpaceforweather(weath);
//cout<<weath<<endl;
w.push_back(weath);

pos = strstr(pos,tag6);
if(pos == NULL)
{
	cout<<"can't find "<<tag6<<endl;;
   return ;
}
pos += strlen(tag6);
char low_degree[100]={'\0'};
sscanf(pos,"%[^<]",low_degree);
deleSpace(low_degree);
//cout<<low_degree<<endl;
l.push_back(low_degree);

pos = strstr(pos,tag7);
if(pos == NULL)
{
		cout<<"can't find "<<tag7<<endl;;
    return ;
}
pos +=strlen(tag7);
char high_degree[100]={'\0'};
sscanf(pos,"%[^<]",high_degree);
deleSpace(high_degree);
//cout<<high_degree<<endl;
h.push_back(high_degree);
}
//int num = atoi(high_degree);
//cout<<num<<endl;
data city_data(d,city_name,w,l,h);
weather_data.push_back(city_data);
}

bool wea_cmp(data n1,data n2)
{
 return (atoi(n1.high_degree[0].c_str()) > atoi(n2.high_degree[0].c_str()));
}

void handle(int sig)
{
   exit(0);
}
int main()  
{  
	signal(SIGSEGV,handle);
	char* resp = NULL;
	vector<string> vec;
	vector<data> weather_data;
    map<string,string> city_name_url;
	
    string res_quanguo = "/?/Index/quanguo";
	GetHttpResponse(res_quanguo,resp);	
   // cout<<resp<<endl;	

      find_resource(resp,city_name_url);
   
     map<string,string>::iterator iter;
	 vector<string> city_name;
	 iter = city_name_url.begin();
	 int count=0;
    for(iter = city_name_url.begin(); iter != city_name_url.end(); iter++)
   {

     // cout<<count<<iter->first<<" "<<iter->second<<endl;
          // city_name.push_back(iter->first); 
		  // cout<<iter->second<<endl;
          GetHttpResponse(iter->second,resp);	
	      //cout<<resp<<endl;
	      Get_weather_info(resp,weather_data);
		   free(resp);
	       resp=NULL;
		   ++count;
		   if((count) > 150)
	           {
		           break;
		       }
    }
          

       string host = "127.0.0.1";
       string user = "root";
       string passwd = "WKyun123456";
       string db = "test";

       sql_connecter conn(host,user,passwd,db);
       conn.begin_connect();
       string lf="'";
       string rf="'";
       string d=",";
       string blank=" ";
       string bo="~";
       string jing="#";
       conn.delete_table();
      conn.creat_table(); 
     for(int j=0;j<(int)city_name.size();++j)
	{
             string data=lf+city_name[j]+rf+d;
             string info=lf+weather_data[j].name+jing;
               for(int  i=0;i<7;++i)
               {
                  info+=weather_data[j].date[i]+blank+weather_data[j].name+blank+weather_data[j].weather[i]+blank+weather_data[j].low_degree[i]+bo+weather_data[j].high_degree[i]+jing;
               }
              data +=info;
              data+=rf;
             conn.insert_sql(data);
    }
//sort(weather_data.begin(),weather_data.end(),wea_cmp);
cout<<"\
<!DOCTYPE html>\
<html>\
<head>\
<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\"/>\
<title> 欢迎来到凯微天气预报</title>\
<style type=\"text/css\" >\
.s_btn_wr{\
    width: 100px;\
    height: 36px;\
    color: white;\
    font-size: 15px;\
    letter-spacing: 1px;\
    background: #BD601B;\
    border-bottom: 1px solid #E62A67;\
    outline: medium;\
    -webkit-appearance: none;\
    -webkit-border-radius: 0;\
}\
.s_btn{\
   width: 100px;\
    height: 36px;\
    color: white;\
    font-size: 15px;\
    letter-spacing: 1px;\
    background: #3385ff;\
    margin-left: -10px;\
}\
.s_ipt_wr {\
    border-color: #7b7b7b #b6b6b6 #b6b6b6 #7b7b7b;\
    background: #fff;\
    display: inline-block;\
    vertical-align: top;\
    width: 539px;\
    margin-right: 0;\
    border-right-width: 0;\
    border-color: #b8b8b8 transparent #ccc #b8b8b8;\
    overflow: hidden;\
}\
.s_ipt{\
    width: 526px;\
    height: 30px;\
    font: 16px/18px arial;\
    line-height: 22px\9;\
    margin-left:9px ;\
}\
.word{\
width:500px;\
margin-top: 40px;\
margin-left: 40px;\
font-size: 20px;\
}\
.w1{\
	    margin-top: 50px;\
}\
.bg{\
	margin-left: 300px;\
}\
.s1{\
display: inline-block;\
margin-left: 170px;\
color: #3385ff;\
}\
</style>\
</head>\
<body>\
	<div  class=\"bg\">\
	<span class=\"s1\"><h1 >凯微天气早知道</h1></span>\
<form action=\"./cgi_bin/find_cgi\",method=\"GET\">\
	<input type = \"text\" name = \"cityname\"/>\
	<input type=\"submit\" id=\"su\" value=\"天气搜索\" class=\"s_btn\"></form>       <div class=\"word\">"<<endl;
      // for(int i=0;i<weather_data.size();++i)
       //{
      // cout<<"<p>"<<weather_data[i].date[0]<<": "<<weather_data[i].name<<" "<<weather_data[i].weather[0]<<" "<<weather_data[i].low_degree[0]<<"~"<<weather_data[i].high_degree[0]<<"</p>"<<endl;
      // }
//       cout<<"</div> </div> <body> </html>"<<endl;

      
    return 0;  
}        
         
