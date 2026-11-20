#include "chan.h"

#include <stdlib.h>

void chanInit(chan_t *chan, int elem_size, int buffer_size)
{
    Queue* queue = malloc(sizeof(Queue));
    queueInit(queue, elem_size);
    chan->queue = queue;

    sem_t sem_recv;
    sem_init(&sem_recv, 0, 0);

    sem_t sem_send;
    sem_init(&sem_send, 0, buffer_size);

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    chan->buffer_size = buffer_size;
    chan->sem_recv = sem_recv;
    chan->sem_send = sem_send;
    chan->lock = lock;
}

void chanSend(chan_t *chan, void *elem)
{
    sem_wait(&chan->sem_send);
    pthread_mutex_lock(&chan->lock);
    enQueue(chan->queue, elem);
    pthread_mutex_unlock(&chan->lock);
    sem_post(&chan->sem_recv);
}

void chanRecv(chan_t *chan, void *elem)
{
    if(chan->buffer_size == 0) {
        sem_post(&chan->sem_send);
    }

    sem_wait(&chan->sem_recv);
    pthread_mutex_lock(&chan->lock);
    deQueue(chan->queue, elem);
    pthread_mutex_unlock(&chan->lock);

    if(chan->buffer_size != 0) {
        sem_post(&chan->sem_send);
    }

}

void chanDestroy(chan_t *chan)
{
    sem_destroy(&chan->sem_recv);
    sem_destroy(&chan->sem_send);
    pthread_mutex_destroy(&chan->lock);

    queueDestroy(chan->queue);
    free(chan->queue);
}
