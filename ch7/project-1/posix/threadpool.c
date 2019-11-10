/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
static task workqueue[QUEUE_SIZE];
static int head = 0;
static int tail = 0;
pthread_mutex_t mutex;
sem_t full;

// the worker bee
pthread_t bees[NUMBER_OF_THREADS];


// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    int num_works;
    int flag = 0;
    pthread_mutex_lock(&mutex);
    sem_getvalue(&full, &num_works);
    if (num_works == QUEUE_SIZE) 
        flag = 1;
    else
    {
        workqueue[tail++] = t;
        tail %= QUEUE_SIZE;
        sem_post(&full);
        flag = 0;
    }
    pthread_mutex_unlock(&mutex);
    return flag;
}

// remove a task from the queue
task dequeue() 
{
    task worktodo;
    pthread_mutex_lock(&mutex);
    worktodo = workqueue[head++];
    head %= QUEUE_SIZE;
    pthread_mutex_unlock(&mutex);
    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    task worktodo;
    while(TRUE)
    {
        sem_wait(&full);
        // execute the task
        worktodo = dequeue();
        execute(worktodo.function, worktodo.data);
    }
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task worktodo;
    worktodo.function = somefunction;
    worktodo.data = p;
    return enqueue(worktodo);
}

// initialize the thread pool
void pool_init(void)
{
    int i;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    for (i = 0; i < NUMBER_OF_THREADS; ++i)
        pthread_create(bees+i,NULL,worker,NULL);
}

// shutdown the thread pool
void pool_shutdown(void)
{
    int i;
    for (i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        pthread_cancel(bees[i]);
        pthread_join(bees[i], NULL);
    }
}
