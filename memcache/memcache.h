/*************************************************************************
	> File Name: memcache.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Mon 09 May 2016 06:07:22 PM CST
 ************************************************************************/

#ifndef _MEMCACHE_H
#define _MEMCACHE_H
#include<iostream>
#include<string>
#include <libmemcached/memcached.h>
using namespace std;


class memcache
{
    public:
      memcache(memcached_st *rhs=NULL, const char *ip="localhost",unsigned short port=11211):time(3600*24)
      {
     //connect server
        memc=memcached_create(rhs);
        server = memcached_server_list_append(NULL,ip,port,&rc);
        rc=memcached_server_push(memc,server);
        memcached_server_list_free(server);
   // cout<<" create memcahce success\n";
      }
     
      bool  MemcacheSet(string &key,string &value)
      {
         value_length = value.length();
         key_length = key.length(); 
     //Save data
     rc=memcached_set(memc,key.c_str(),key.length(),value.c_str(),value.length(),time,flags);
     if(rc==MEMCACHED_SUCCESS)
     {
      // cout<<"Save data sucessful!"<<endl;
       return true;
     } 
     return false;
    }
     bool  MemcacheGet(string &key,string &result)
     {
         //Get data
      char *ret = memcached_get(memc,key.c_str(),key.length(),&value_length,&flags,&rc);
    if(rc == MEMCACHED_SUCCESS)
    {
        //cout<<"Get value  sucessful!"<<endl;
        result=(const char *)ret;
        free(ret);
        return true;
    }
    //cout<<"mem Get value failed\n";
    return false;
    }
     bool MemcacheDel(string &key)
     {
     //Delete data
      rc=memcached_delete(memc,key.c_str(),key.length(),time);
    if(rc==MEMCACHED_SUCCESS)
     {
        // cout<<"Delete key:"<<key<<" sucessful!"<<endl;
        return true;
     }
       return false;
     }
    ~memcache()
    {  
     //free
     memcached_free(memc);
    }
     memcached_st *memc;
     memcached_return rc;
     memcached_server_st *server;
     size_t value_length;
     size_t key_length;
     time_t time;
     uint32_t  flags;

};
#endif
