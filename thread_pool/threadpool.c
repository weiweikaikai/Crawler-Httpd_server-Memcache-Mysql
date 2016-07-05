/*************************************************************************
	> File Name: threadpool.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Thu 02 Jun 2016 11:47:35 AM CST
 ************************************************************************/

#include "threadpool.h"

typedef enum {

    immediate_shutdown = 1,
    graceful_shutdown = 2

}threadpool_sd_t;

static int threadpool_free(threadpool_t *pool);
static void *threadpool_worker(void *arg);
/*
:完成线程池的初始化工作。主要工作有：
初始化任务队列，初始化锁，初始化条件锁，
初始化线程，并给线程注册threadpool_worker函数，
使各线程执行threadpool_worker函数。
*/
threadpool_t *threadpool_init(int thread_num) {
    if (thread_num <= 0) {
        printf("the arg of threadpool_init must greater than 0");
        return NULL;
    }

    threadpool_t *pool;
    if ((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) {
        goto err;
    }

    pool->thread_count = 0;
    pool->queue_size = 0; //队列中文件描述符的个数
    pool->shutdown = 0;
    pool->start_size = 0;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    pool->head = (task_t *)malloc(sizeof(task_t));    /* dummy head */

    if ((pool->threads == NULL) || (pool->head == NULL)) {
        goto err;
    }

    pool->head->func = NULL;
    pool->head->arg = NULL;
    pool->head->next = NULL;

    if (pthread_mutex_init(&(pool->lock), NULL) != 0) {  
        goto err;
    }

    if (pthread_cond_init(&(pool->cond), NULL) != 0) {
        pthread_mutex_destroy(&(pool->lock)); //没成功就将上边的初始化的锁销毁
        goto err;
    }
    
    int i;
    for (i=0; i<thread_num; ++i) {
        if (pthread_create(&(pool->threads[i]), NULL, threadpool_worker, (void *)pool) != 0) {
            threadpool_destroy(pool, 0);
            return NULL;
        }
        printf("thread: %08x started\n", (uint32_t) pool->threads[i]);

        pool->thread_count++;
        pool->start_size++;
    }

    return pool;

err:
    if (pool) {
        threadpool_free(pool);
    }

    return NULL;
}
/*
此函数是向任务队列中添加任务，
即添加线程实际执行的函数指针
以及该函数指针需要的参数，任务数加一
*/

int threadpool_add(threadpool_t *pool, void (*func)(void *), void *arg) {

    int rc, err = 0;
    if (pool == NULL || func == NULL) {
        printf("pool == NULL or func == NULL");
        return -1;
    }
    
    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        printf("pthread_mutex_lock");
        return -1;
    }

    if (pool->shutdown) {
        err = tp_already_shutdown;
        goto out;
    }
    
    // TODO: use a memory pool
    task_t *task = (task_t *)malloc(sizeof(task_t));
    if (task == NULL) {
        printf("malloc task fail");
        return -1;
    }
    
    // TODO: use a memory pool
    task->func = func;
    task->arg = arg;
    task->next = pool->head->next;
    pool->head->next = task;

    pool->queue_size++;
    
    rc = pthread_cond_signal(&(pool->cond));
     if(rc !=0 )
	{
	   printf("pthread_cond_signal");
	}
	printf("add: %d\n", pool->queue_size);
out:
    if(pthread_mutex_unlock(&pool->lock) != 0) {
        printf("pthread_mutex_unlock");
        return -1;
    }
    
    return err;
}

int threadpool_free(threadpool_t *pool) {
    if (pool == NULL || pool->start_size > 0) {
        return -1;
    }

    if (pool->threads) {
        free(pool->threads);
    }

    task_t *old;
    /* pool->head is a dummy head */ 
    while (pool->head->next) {//头删
        old = pool->head->next;
        pool->head->next = pool->head->next->next;
        free(old);
    }

    return 0;
}

int threadpool_destroy(threadpool_t *pool, int graceful) {
    int err = 0;

    if (pool == NULL) {
        printf("pool == NULL");
        return tp_invalid;
    }
    
    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        return tp_lock_fail;
    }
    
    do {
        // set the showdown flag of pool and wake up all thread    
        if (pool->shutdown) {
            err = tp_already_shutdown;
            break;
        }

        pool->shutdown = (graceful)? graceful_shutdown: immediate_shutdown;
        
        if (pthread_cond_broadcast(&(pool->cond)) != 0) {
            err = tp_cond_broadcast;
            break;
        }

        if (pthread_mutex_unlock(&(pool->lock)) != 0) {
            err = tp_lock_fail;
            break;
        }
        
        int i;
        for (i=0; i<pool->thread_count; i++) {
            if (pthread_join(pool->threads[i], NULL) != 0) {
                err = tp_thread_fail;
            }
            printf("thread %08x exit\n", (uint32_t) pool->threads[i]);
        }
             
    } while(0);

    if (!err) {
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->cond));
        threadpool_free(pool);
    }

    return err;
}

/*
各个线程创建后执行此函数，
此函数中是一个死循环，线程进入循环后，
先判断是否有任务，
有则将任务从任务队列中取出执行
(任务中含有要执行的函数指针即void *woke()和相关参数)
，任务数减一，没有任务则阻塞住，
等待有任务时触发条件锁然后继续执行该任务
*/

static void *threadpool_worker(void *arg) {
    if (arg == NULL)
	{
        printf("arg should be type zv_threadpool_t*");
        return NULL;
    }

    threadpool_t *pool = (threadpool_t *)arg;
    task_t *task;

    while (1) {
        pthread_mutex_lock(&(pool->lock));
        
        /*  Wait on condition variable, check for spurious wakeups. */
        while ((pool->queue_size == 0) && !(pool->shutdown)){
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }

        if (pool->shutdown == immediate_shutdown) {
            break;
        } else if ((pool->shutdown == graceful_shutdown) && pool->queue_size == 0) {
            break;
        }

        task = pool->head->next;
        if (task == NULL) {
            pthread_mutex_unlock(&(pool->lock));
            continue;
        }

        pool->head->next = task->next;
        pool->queue_size--;

        pthread_mutex_unlock(&(pool->lock));

        (*(task->func))(task->arg);
		
        /* TODO: memory pool */
        free(task);
    }

    pool->start_size--;
    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);

    return NULL;
}
