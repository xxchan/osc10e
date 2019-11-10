/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data works[30];
    int i;

    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    for(i = 0; i < 30; ++i)
    {
        if (i==15 || i == 27) 
            sleep(1);
        works[i].a = i;
        works[i].b = 2*i;
        if (pool_submit(&add,works+i) == 1)
            printf("submit failure, i = %d\n", i);
    }

    // may be helpful 
    //sleep(3);

    pool_shutdown();

    return 0;
}
