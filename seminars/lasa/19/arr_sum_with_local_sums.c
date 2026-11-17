#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10
#define ARR_SIZE 1000000
#define MAX_NUM 1000
#define STEP_SIZE (ARR_SIZE / NTHREADS)

void generateArr(int * arr) {
    int sum = 0;
    for(int i = 0; i < ARR_SIZE; i++) {
        arr[i] = rand() % MAX_NUM;
        sum += arr[i];
    }
    printf("Sum of arr is: %d\n", sum);
}

typedef struct {
    int startIndex;
    int * arr;
    int * sum;
    int threadId;
    pthread_mutex_t* lock;
} Args;

void* sum(void * args) {
    Args * a = (Args*)args;

    int localSum = 0;
    for(int i = a->startIndex; i < a->startIndex + STEP_SIZE; i++) {
        localSum += a->arr[i];
        //printf("Thread %d added %d to the sum\n", a->threadId, a->arr[i]);
    }

    pthread_mutex_lock(a->lock);
    *(a->sum) += localSum;
    pthread_mutex_unlock(a->lock);

    return NULL;
}


int main() {

    int arr[ARR_SIZE];
    generateArr(arr);

    pthread_t threads[NTHREADS];
    Args args[NTHREADS];

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    int globalSum = 0;
    for(int i = 0; i < NTHREADS; i++) {
        args[i].arr = arr;
        args[i].startIndex = i * STEP_SIZE;
        args[i].sum = &globalSum;
        args[i].threadId = i;
        args[i].lock = &lock;
        pthread_create(&threads[i], NULL, sum, &args[i]);
    }

    for(int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Final sum of threads: %d\n", globalSum);

}


