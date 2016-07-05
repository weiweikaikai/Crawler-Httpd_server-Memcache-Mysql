/*************************************************************************
	> File Name: test.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Thu 23 Jun 2016 10:10:10 PM CST
 ************************************************************************/

#include"memorypool.c"
#include<stdio.h>

int main()
{

k_pool_t *pool= create_pool(100);

int *p=palloc(pool,4);
*p=10;
printf("%d\n",*p);
destroy_pool(pool);

    return 0;
}

