/*************************************************************************
	> File Name: memorypool.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Fri 03 Jun 2016 09:05:35 AM CST
 ************************************************************************/

#include"memorypool.h"
#include<stdlib.h>
 

static void *palloc_block(k_pool_t*pool,size_t size);
static void*pool_alloc_large(k_pool_t*pool,size_t size);
static void *my_memalign(size_t alignment, size_t size);
//static void *alloc(size_t size);
//static void *calloc(size_t size);

 

k_pool_t *create_pool(size_t size)//创建内存池
{
   k_pool_t *p = my_memalign(POOL_ALIGNMENT,size+sizeof(k_pool_t));
   if(p == NULL)
	{
      return NULL; 
    }
	p->d.last = (u_char*)p+sizeof(k_pool_t); //内存池中内存空闲空间的开始地址
	p->d.end = (u_char*)p+size+sizeof(k_pool_t);//内存池结束地址
	p->d.next = NULL;
	p->d.failed =0;

	size= p->d.end - p->d.last;
	p->max = size < MAX_ALLOC_FROM_POOL ? size : MAX_ALLOC_FROM_POOL;

	p->current =p;
	p->large = NULL;
	p->cleanup=NULL;
	
	return p; //指向空间最顶部头部
}
void destroy_pool(k_pool_t*pool)//销毁pool
{
   k_pool_t *p,*n;
   k_pool_large_t* large;
   k_pool_cleanup_t*cur;
   for(cur=pool->cleanup;cur!=NULL;cur=cur->next)
	{
      if(cur->handler)
		{
	      cur->handler(cur->data);
	  }
    }
	for(large=pool->large;large!=NULL;large=large->next)
	{
	   if(large->alloc)
		{
	       free(large->alloc);
	   }
	}
	p=pool;
	n=pool->d.next;
	while(n!=NULL)
	{
	   free(p);
	 	p=n;
		n = n->d.next;
	}
}
void reset_pool(k_pool_t *pool) //重置pool中的部分数据或者全部内存池中数据
{
   k_pool_t *p;
   k_pool_large_t*l;

   for(l=pool->large;l!=NULL;l=l->next) //主要是释放大块内存数据，同时可以重新使用内存池中数据
	{
      if(l->alloc)
		{
	    free(l->alloc);
	  }
   }
   for(p=pool;p!=NULL;p=p->d.next) //重置内存池中内存块中的数据，从而可以继续使用
	{
       p->d.last = (u_char*)p+sizeof(k_pool_t);
	   p->d.failed = 0;
   }
   pool->current=pool; //当前可用的内存是pool
   pool->large=NULL;
}
  
//从内存池中返回size大小的内存首地址
void *palloc(k_pool_t *pool, size_t size) //内存对齐的 
{
    if(size <= pool->max)
	{
		k_pool_t *p = pool->current; //小于max值，则从current节点开始遍历pool链表
	   do
	   {
		 u_char* m =k_align_ptr(p->d.last,POOL_ALIGNMENT);//地址对齐
         if((size_t)(p->d.end - m) >= size) //判断pool中剩余内存是否足够
		{
		   p->d.last = m+size;
		   return m;
		}
		p=p->d.next;//如果当前内存不足就在下一个内存块中分配空间
	   } while (p); 
	   
	   return palloc_block(pool,size);//链表里没有能分配size大小内存的节点，则生成一个新的节点并在其中分配内存  
	}
  
    //当所需要的内存大于内存池pool所能提供的内存大小的时候就分配大块的内存来满足用户要求
    return pool_alloc_large(pool,size);
}

 
void *pnalloc(k_pool_t *pool, size_t size)//内存不对齐的
{
  
    if(size <= pool->max)
	{
         k_pool_t *p = pool->current;
		 do
		 {
			 u_char*m=p->d.last;
	      if((size_t)(p->d.end - m) >= size) //判断pool中剩余内存是否足够
		   {
		     p->d.last = m+size;
		     return m;
		   }
		   p=p->d.next;//如果当前内存不足就在下一个内存块中分配空间
		 } while (p);
		  
		 return palloc_block(pool,size);//链表里没有能分配size大小内存的节点，则生成一个新的节点并在其中分配内存  
     }
    //当所需要的内存大于内存池pool所能提供的内存大小的时候就分配大块的内存来满足用户要求
    return pool_alloc_large(pool,size);
} 
 
 //如果前面开辟的pool内存块用完了，就重新开辟新的内存块
 //该函数分配一块内存后，last指针指向的是ngx_pool_data_t结构体(大小16B)之后数据区的起始位置。
//而创建内存池时时，last指针指向的是ngx_pool_t结构体(大小40B)之后数据区的起始位置
 static void *palloc_block(k_pool_t*pool,size_t size)
 {
	 //原来整个内存块的大小
   size_t newpool_size = pool->d.end-(u_char*)pool; 
   //重新分配一个和k_pool_t结构体一样大的节点，内存对齐
   u_char *m=(u_char*)my_memalign(POOL_ALIGNMENT,newpool_size+sizeof(k_pool_data_t));
   if(m == NULL)
	 {
       return NULL;
     }
   k_pool_t* new_pool=(k_pool_t*)m;
   //初始化节点
   new_pool->d.end = m+newpool_size;
   new_pool->d.next = NULL;
   new_pool->d.failed = 0;

   m += sizeof(k_pool_data_t);//让m指向该块内存ngx_pool_data_t结构体之后数据区起始位
   m = k_align_ptr(m,POOL_ALIGNMENT);//将m对齐到内存对齐地址
   new_pool->d.last = m+size;//在数据区分配size大小的内存并设置last指针
    
	 k_pool_t *p =pool->current;
	 for(;p->d.next;p = p->d.next)
	 {
	   if(p->d.failed++ > 4) //failed的值只在此处被修改
	   {
	      pool->current = p->d.next;//申请失败4次以上移动current指针 
	   }
	 }
	 p->d.next = new_pool;
      return m;
 }
 
//void *alloc(size_t size)
//{
//   void *p = malloc(size);
//   if(p == NULL)
//	{
//      //报错
//    }
//	return p;
//}
//void *calloc(size_t size)
//{
//    void *p = alloc(size);
//	if(p)
//	{
//	  memset(p,'\0',size);
//	}
//	return p;
//}

void*pool_alloc_large(k_pool_t*pool,size_t size)
{
   
   k_pool_large_t *large;

    void*p=malloc(size);
	if(p == NULL)
	{
	  return NULL;
	}

	for(large=pool->large;large;large=large->next)
	{
	   if(large->alloc == NULL)  //找到一个alloc为空的指针
		{
	      large->alloc = p;
		  return p;
	   }
	}
}

int  pfree(k_pool_t*pool,void*p)
{
  k_pool_large_t *large = pool->large;
  for(;large;large=large->next)
	{
       if(p == large->alloc)
		{
	     free(large->alloc);
         large->alloc=NULL;
		 return 0;
	    }
  }
  return -1;
}

void* pmemalign(k_pool_t*pool,size_t size,size_t alignment)
{
  void *p = my_memalign(alignment,size);
  if(p==NULL)
	{
      return NULL;
   }
    k_pool_large_t *large=palloc(pool,sizeof(k_pool_large_t));
	if(large == NULL)
	{
	  free(p);
	  return NULL;
	}
	large->alloc=p;
	large->next = pool->large;
	pool->large = large;
	return p;
}
//linux平台

 static void *my_memalign(size_t alignment, size_t size)
 {

	 void *p=NULL;
	 int err = posix_memalign(&p,alignment,size);
	 if(err)
	 {
	   p=NULL;
	 }
     return p;
 }

 

