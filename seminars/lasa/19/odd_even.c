#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define MAX_NUM 100


typedef struct {
    sem_t* even;
    sem_t* odd;
} Args;

void* print_even(void* args) {
    Args* a = (Args*)args;

    for(int i = 2; i <= MAX_NUM; i+=2) {
        sem_wait(a->even);
        printf("%d\n", i);
        sem_post(a->odd);
    }
    return NULL;
}

void* print_odd(void* args) {
    Args* a = (Args*)args;

    for(int i = 1; i <= MAX_NUM; i+=2) {
        sem_wait(a->odd);
        printf("%d\n", i);
        sem_post(a->even);
    }
    return NULL;
}


int main() {

    pthread_t evenThread;
    pthread_t oddThread;

    sem_t even;
    sem_t odd;
    sem_init(&even, 0, 0);
    sem_init(&odd, 0, 1);

    Args arg;
    arg.even = &even;
    arg.odd = &odd;

    pthread_create(&evenThread, NULL, print_even, &arg);
    pthread_create(&oddThread, NULL, print_odd, &arg);

    pthread_join(evenThread, NULL);
    pthread_join(oddThread, NULL);

}