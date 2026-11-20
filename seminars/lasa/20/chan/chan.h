#ifndef __CHAN_H__
#define __CHAN_H__

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

#include "myQueue/myQueue.h"

typedef struct
{
    Queue *queue;
    int buffer_size;
    sem_t sem_recv;
    sem_t sem_send;
    pthread_mutex_t lock;
} chan_t; // (same as blocking queue)

// BufferSize = 0 for unbuffered channel
void chanInit(chan_t *chan, int elemSize, int buffer_size);

/*
 * Function: chanSend
 * ------------------                       
 * Thread Sends a new value to the channel.
 * Works Like FIFO enqueue, but can not push
 * while buffer is full.
 */
void chanSend(chan_t *chan, void *elem);

/*
 * Function: chanRecv
 * ------------------
 * Thread recieves a value from the channel.
 * Works like FIFO dequeue.
 * If the channel is empty, thread waits until
 * other thread sends a value to the channel.
 */
void chanRecv(chan_t *chan, void *elem);

void chanDestroy(chan_t *chan);

#endif  // __CHAN_H__
