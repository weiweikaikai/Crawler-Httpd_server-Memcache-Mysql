/*************************************************************************
	> File Name: list.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Sun 26 Jun 2016 10:46:18 AM CST
 ************************************************************************/

#ifndef _LIST_H
#define _LIST_H

#ifndef NULL
#define NULL 0
#endif

//list_head, reference linux kernel implementation 参考linux内核的双向链表实现

typedef struct list_head
{
	struct list_head *prev;
	struct list_head *next;
}list_head;

#define INIT_LIST_HEAD(ptr) do {\
    struct list_head *_ptr = (struct list_head *)ptr;   \
    (_ptr)->next = (_ptr); (_ptr->prev) = (_ptr);       \
} while(0)

//插入新的元素节点
static inline void _list_add(list_head*_new,list_head*_prev,list_head*_next)
{
  _new->next = _next;
  _next->prev = _new;
  _prev->next = _new;
  _new->prev = _prev;
}

static inline void list_add(list_head*_new,list_head *head)
{
   _list_add(_new,head,head->next);
}

static inline void list_add_tail(list_head*_new,list_head *head)
{
   _list_add(_new,head->prev,head);
}

static inline _list_del(list_head *prev,list_head *next)
{
    prev->next = next;
	next->prev = prev;
}
static inline list_del(list_head *entry)
{
   _list_del(entry->prev,entry->next);
}


#define list_for_each(pos,head)\
        for(pos = (head)->next; pos != (head); pos = pos->next)


#define offsetof(type,member) ((size_t)&((type*)0)->member)

//得出数据中的地址
#define container_of(ptr,type,member) ({    \
         const typeof(((type*)0)->member) * mptr =(ptr);\
	    (type*)((unsigned char*)mptr - offsetof(type,member));\
})

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#endif
