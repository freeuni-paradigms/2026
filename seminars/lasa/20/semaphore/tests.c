#include <pthread.h>
#include <stdio.h>
#include "my_semaphore.h"

#define PRINT_MAX 100

typedef pthread_mutex_t mut_t;

typedef struct
{
    my_sem_t *odd_sem;
    my_sem_t *even_sem;
} thread_args_t;

void *odd_fn(void *args)
{
    my_sem_t *odd_sem = ((thread_args_t *)args)->odd_sem;
    my_sem_t *even_sem = ((thread_args_t *)args)->even_sem;

    for (int i = 1; i <= PRINT_MAX; i += 2)
    {
        sem_wait(odd_sem);

        printf("odd: %d\n", i);

        sem_post(even_sem);
    }

    return 0;
}

void *even_fn(void *args)
{
    my_sem_t *odd_sem = ((thread_args_t *)args)->odd_sem;
    my_sem_t *even_sem = ((thread_args_t *)args)->even_sem;

    for (int i = 2; i <= PRINT_MAX; i += 2)
    {
        sem_wait(even_sem);

        printf("even: %d\n", i);

        sem_post(odd_sem);
    }

    return 0;
}

int main()
{
    pthread_t odd_thread, even_thread;
    my_sem_t odd_sem, even_sem;

    sem_init(&odd_sem, 1);
    sem_init(&even_sem, 0);

    thread_args_t args;
    args.odd_sem = &odd_sem;
    args.even_sem = &even_sem;

    pthread_create(&odd_thread, NULL, odd_fn, &args);
    pthread_create(&even_thread, NULL, even_fn, &args);

    pthread_join(odd_thread, NULL);
    pthread_join(even_thread, NULL);

    sem_destroy(&even_sem);
    sem_destroy(&odd_sem);
    
    return 0;
}
