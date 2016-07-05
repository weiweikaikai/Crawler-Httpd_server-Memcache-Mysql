/*************************************************************************
	> File Name: threadpool.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Thu 02 Jun 2016 11:47:30 AM CST
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#define THREAD_NUM 8

typedef struct task_s {
    void (*func)(void *);
    void *arg;
    struct task_s *next;
}task_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t *threads;
    task_t *head;
    int thread_count;
    int queue_size;
	int shutdown;
	int start_size;
}threadpool_t;

typedef enum {
    tp_invalid           = -1,
    tp_lock_fail         = -2,
    tp_already_shutdown  = -3,
    tp_cond_broadcast    = -4,
    tp_thread_fail       = -5, 
}threadpool_error_t;


threadpool_t *threadpool_init(int thread_num);

int threadpool_add(threadpool_t *pool, void (*func)(void *), void *arg);

int threadpool_destroy(threadpool_t *pool,int graceful);

#ifdef __cplusplus
}

#endif
#endif


