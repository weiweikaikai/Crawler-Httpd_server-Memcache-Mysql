/*************************************************************************
	> File Name: memcache.cpp
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Mon 09 May 2016 06:20:35 PM CST
 ************************************************************************/

#include"memcache.h" 
int main()
{
     memcache mem;
   string key ="%6E%B5%B7%E5%8F%A3";
        
         string value="value";
         //mem.MemcacheSet(key,value);
         string result;
     mem.MemcacheGet(key,result);
     cout<<result<<endl;
    return 0;
}

