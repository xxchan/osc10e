#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "buffer.h"

#define true 1

#define gettid() syscall(__NR_gettid)

buffer_item buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;

pthread_mutex_t mutex;
sem_t empty;
sem_t full;


int insert_item(buffer_item item) {
    /* insert item into buffer
     return 0 if successful, otherwise
     return -1 indicating an error condition */

    buffer[tail++] = item;
    tail %= BUFFER_SIZE;
    return 0;
}

int remove_item(buffer_item *item) {
    /* remove and object from buffer
     placing it in item
     return 0 if successful, otherwise
     return -1 indicating an error condition */
    
    *item = buffer[head++];
    head %= BUFFER_SIZE;
    return 0;
}


void *producer(void *param) {
    buffer_item item;

    while (true) {
        /* sleep for a random period of time */
        sleep(rand() % 5);
        /* generate a random number */
        item = rand() % 100;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        if (insert_item(item))
            fprintf(stderr, "insert item failed");
        else
            printf("producer(tid=%lu) produced %d\n", gettid(), item);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *param) {
    buffer_item item;

    while (true) {
        /* sleep for a random period of time */
        sleep(rand() % 5);

        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        if (remove_item(&item)) 
            fprintf(stderr, "remove item failed");
        else
            printf("consumer(tid=%lu) consumed %d\n", gettid(), item);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main(int argc, char *argv[]) {
    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    /* 2. Initialize buffer */
    /* 3. Create producer thread(s) */
    /* 4. Create consumer thread(s) */
    /* 5. Sleep */
    /* 6. Exit */
    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);
    int i;
    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    head = tail = 0;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    for (i = 0; i < num_producers; ++i)
        pthread_create(producers + i, NULL, &producer, NULL);
    for (i = 0; i < num_consumers; ++i)
        pthread_create(consumers + i, NULL, &consumer, NULL);

    sleep(sleep_time);
    
    return 0;
}
