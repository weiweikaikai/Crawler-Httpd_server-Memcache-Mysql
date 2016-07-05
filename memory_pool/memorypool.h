/*************************************************************************
	> File Name: memorypool.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 03 Jun 2016 09:05:12 AM CST
 ************************************************************************/

#ifndef _MEMORYPOOL_H
#define _MEMORYPOOL_H
#include<stdlib.h>

#define POOL_ALIGNMENT sizeof(unsigned long) //对齐字节数一般是16
#define MAX_ALLOC_FROM_POOL (4096-1) //X86就是4095 为一页大小
typedef unsigned char u_char;

//这种计算地址或者长度对齐，取整的宏还是很有用的。
//cpu访问对齐的数据较快，不对齐的的int之类的，有可能要多次内存访问才能取到值。
#define k_align(num,align)    (((num)+(align-1))&~(align-1))//align 为2的幂  数字的对齐
#define k_align_ptr(ptr,align)    (u_char*)  ((((unsigned int)ptr)+((unsigned int)align-1))& ~((unsigned int)align-1))


typedef void (*k_pool_cleanup_ptr)(void*data);

typedef struct k_pool_data_s k_pool_data_t;
typedef  struct k_pool_s k_pool_t;
typedef struct k_pool_large_s   k_pool_large_t;
typedef struct k_pool_cleanup_s  k_pool_cleanup_t;    

struct k_pool_cleanup_s
{
   k_pool_cleanup_ptr   handler;//当前cleanup数据的回调函数 k_destroy_pool中执行
   void *data;//内存真正的地址，回调时候将此数据传入回到函数

   k_pool_cleanup_t *next;//指向下一块cleanup内存的指针
};


//大块内存结构体，链表结构
struct k_pool_large_s
{
    k_pool_large_t *next;
	void      *alloc;//申请的内存块地址
};

//内存块数据
struct k_pool_data_s 
{
   u_char      *last;  //当前内存池分配到此处，即下一次分配从此处开始  
   u_char      *end;   //内存池结束位置  
   k_pool_t    *next;  //内存池里面有很多块内存，这些内存块就是通过该指针连成链表的  
   unsigned int         failed; //内存池分配失败次数  
};

//内存池
//为了减少内存碎片的数量，并通过统一管理来减少代码中出现的内存泄漏的可能性
struct k_pool_s
{
    k_pool_data_t     d;       //内存池的数据块  
    size_t            max;     //内存池数据块的最大值  
    k_pool_t          *current; //指向当前内存池  
    void             *large;   //大块内存链表，即分配空间超过max的内存  
    k_pool_cleanup_t  *cleanup;//pool中指向k_pool_cleanup_t数据块中的指针
	 //k_chain_t      *chain; //缓冲区结构体链表指针
     //日志信息  
};



//void *pool_alloc(k_pool_t *pool,size_t size); 
//void *pool_calloc(k_pool_t *pool,size_t size);


k_pool_t *create_pool(size_t size); //创建pool  
void destroy_pool(k_pool_t*pool);//销毁pool
void reset_pool(k_pool_t *pool); //重置pool中的部分数据


void *palloc(k_pool_t *pool, size_t size); //内存对齐的  
void *pnalloc(k_pool_t *pool, size_t size);//内存不对齐的
void *pcalloc(k_pool_t*pool,size_t size);
void* pmemalign(k_pool_t*pool,size_t size,size_t alignment); //分配大块内存时候使用
int  pfree(k_pool_t*pool,void*p); //删除大块内存
 
 
k_pool_cleanup_t pool_cleanup_add(k_pool_t*p,size_t size); //给pool中添加cleanup数据



#endif
