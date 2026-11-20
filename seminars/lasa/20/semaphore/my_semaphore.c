#include "my_semaphore.h"

void sem_init(my_sem_t *s, int value)
{
    s->count = value;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);

    s->lock = lock;
    s->cond = cond;
}

void sem_wait(my_sem_t *s)
{
    pthread_mutex_lock(&s->lock);
    while(s->count == 0) {
        pthread_cond_wait(&s->cond, &s->lock);
    }
    s->count--;
    pthread_mutex_unlock(&s->lock);
}

void sem_post(my_sem_t *s)
{
    pthread_mutex_lock(&s->lock);
    s->count++;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}

void sem_destroy(my_sem_t *s)
{
    pthread_mutex_destroy(&s->lock);
    pthread_cond_destroy(&s->cond);
}
