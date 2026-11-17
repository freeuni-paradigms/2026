#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 10
#define ARR_SIZE 10000
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
    int *curIdx;
    int * arr;
    int * sum;
    int threadId;
    int contribution;
    sem_t* sem;
} Args;

void* sum(void * args) {
    Args * a = (Args*)args;

    while(true) {
        sem_wait(a->sem);

        if(*a->curIdx >= ARR_SIZE) {
            sem_post(a->sem);
            break;
        }

        *a->sum += a->arr[*a->curIdx];
        (*a->curIdx)++;
        a->contribution++;
        sem_post(a->sem);

        printf("Thread %d added %d to the sum\n", a->threadId, a->arr[*a->curIdx]);
    }

    return NULL;
}


int main() {

    int arr[ARR_SIZE];
    generateArr(arr);

    pthread_t threads[NTHREADS];
    Args args[NTHREADS];

    sem_t sem;
    sem_init(&sem, 0, 1);

    int globalSum = 0;
    int curIdx = 0;
    for(int i = 0; i < NTHREADS; i++) {
        args[i].arr = arr;
        args[i].curIdx = &curIdx;
        args[i].sum = &globalSum;
        args[i].threadId = i;
        args[i].sem = &sem;
        args[i].contribution = 0;
        pthread_create(&threads[i], NULL, sum, &args[i]);
    }

    for(int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for(int i = 0; i < NTHREADS; i++) {
        printf("Thread %d contributed %d times\n", i, args[i].contribution);
    }

    printf("Final sum of threads: %d\n", globalSum);

}
