#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "chan.h"

#define ASSERT(cond, print) if (!(cond)) {\
            printf print;\
            chanDestroy(chan);\
            exit(0);\
        }
#define BUFF_SIZE 10000
#define mut pthread_mutex_t

int unbuffInt;

void *unbufferedIntSend(void *args) {
    chan_t *chan = args;
    int val = rand();
    unbuffInt = val;
    printf("\tSending value\n");
    chanSend(chan, &val);
    printf("\tValue sent\n");
}

void *unbufferedIntRecive(void *args) {
    chan_t *chan = args;
    int val;
    printf("\tRecieveing value in 2 seconds.\n");
    sleep(2);
    chanRecv(chan, &val);
    ASSERT(val == unbuffInt, ("testIntUnbuffered Failed --- %d != %d\n", val, unbuffInt));
    printf("\tValue recieved\n");
}

void testIntUnbuffered() {
    printf("Starting unbuffered int test\n");
    chan_t chan;
    chanInit(&chan, sizeof(int), 0);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, unbufferedIntSend, &chan);
    pthread_create(&threads[1], NULL, unbufferedIntRecive, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Unbuffered int test complete\n");
}

void *bufferedIntSend(void *args) {
    chan_t *chan = args;
    printf("\tSending values\n");
    for (int i = 0; i < BUFF_SIZE; i++)
        chanSend(chan, &i);
    printf("\tValues sent\n");
}

void *bufferedIntRecieve(void *args) {
    chan_t *chan = args;
        int ret;
    printf("\tRecieveing values.\n");
    for (int i = 0; i < BUFF_SIZE; i++) {
        chanRecv(chan, &ret);
        ASSERT(ret == i, ("testIntBuffered Failed --- %d != %d\n", ret, i));
    }
    printf("\tValues recieved\n");
}

void testIntBuffered() {
    printf("Starting buffered int test\n");
    chan_t chan;
    chanInit(&chan, sizeof(int), BUFF_SIZE);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, bufferedIntSend, &chan);
    pthread_create(&threads[1], NULL, bufferedIntRecieve, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Buffered int test complete\n");
}

void testInt() {
    printf("\n");
    testIntUnbuffered();
    testIntBuffered();
}

double unbuffDouble;

void *unbufferedDoubleSend(void *args) {
    chan_t *chan = args;
    double val = ((double)rand() / RAND_MAX) * rand();
    unbuffDouble = val;
    printf("\tSending value\n");
    chanSend(chan, &val);
    printf("\tValue sent\n");
}

void *unbufferedDoubleRecive(void *args) {
    chan_t *chan = args;
    double val;
    printf("\tRecieveing value in 2 seconds.\n");
    sleep(2);
    chanRecv(chan, &val);
    ASSERT(val == unbuffDouble, ("testDoubleUnbuffered Failed --- %f != %f\n", val, unbuffDouble));
    printf("\tValue recieved\n");
}

void testDoubleUnbuffered() {
    printf("Starting unbuffered double test\n");
    chan_t chan;
    chanInit(&chan, sizeof(double), 0);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, unbufferedDoubleSend, &chan);
    pthread_create(&threads[1], NULL, unbufferedDoubleRecive, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Unbuffered double test complete\n");
}

void *bufferedDoubleSend(void *args) {
    chan_t *chan = args;
    printf("\tSending values\n");
    for (int i = 1; i < BUFF_SIZE; i++) {
        double val = i + 1.0 / i;
        chanSend(chan, &val);
    }
    printf("\tValues sent\n");
}

void *bufferedDoubleRecieve(void *args) {
    chan_t *chan = args;
    double ret;
    printf("\tRecieveing values.\n");
    for (int i = 1; i < BUFF_SIZE; i++) {
        chanRecv(chan, &ret);
        double val = i + 1.0 / i;
        ASSERT(ret == val, ("testDoubleBuffered Failed --- %lf != %lf\n", ret, val));
    }
    printf("\tValues recieved\n");
}

void testDoubleBuffered() {
    printf("Starting buffered double test\n");
    chan_t chan;
    chanInit(&chan, sizeof(double), BUFF_SIZE);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, bufferedDoubleSend, &chan);
    pthread_create(&threads[1], NULL, bufferedDoubleRecieve, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Buffered double test complete\n");
}

void testDouble() {
    printf("\n");
    testDoubleUnbuffered();
    testDoubleBuffered();
}

char *testStr;

void *unbufferedStringSend(void *args) {
    chan_t *chan = args;
    char *str = "Da adga davit, mefe apkhazta... da iyo Didgori, dzlevai sakvirveli!";
    testStr = str;
    printf("\tSending a string\n");
    chanSend(chan, &str);
    printf("\tA string sent\n");
}

void *unbufferedStringRecieve(void *args) {
    chan_t *chan = args;
    char *str;
    printf("\tRecieveing a string in 2 seconds.\n");
    sleep(2);
    chanRecv(chan, &str);
    ASSERT(!(strcmp(str, testStr)), ("testStringUnbuffered Failed --- %s != %s\n", str, testStr));
    printf("\tA string recieved\n");
}

void testStringUnbuffered() {
    printf("Starting unbuffered string test\n");
    chan_t chan;
    chanInit(&chan, sizeof(char *), 0);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, unbufferedStringSend, &chan);
    pthread_create(&threads[1], NULL, unbufferedStringRecieve, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Unbuffered string test complete\n");
}

void *bufferedStringSend(void *args) {
    chan_t *chan = args;
    char *str = "Naxes ucxo moyme vinme, jda mtirali wylisa piras, shavi cxeni sadavita...";
    testStr = str;
    printf("\tSending a string\n");
    chanSend(chan, &str);
    printf("\tA string sent\n");
}

void *bufferedStringRecieve(void *args) {
    chan_t *chan = args;
    char *str;
    printf("\tRecieveing a string in 2 seconds.\n");
    sleep(2);
    chanRecv(chan, &str);
    ASSERT(!(strcmp(str, testStr)), ("testStringUnbuffered Failed --- %s != %s\n", str, testStr));
    printf("\tA string recieved\n");
}

void testStringBuffered() {
    printf("Starting buffered string test\n");
    chan_t chan;
    chanInit(&chan, sizeof(char *), 1);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, bufferedStringSend, &chan);
    pthread_create(&threads[1], NULL, bufferedStringRecieve, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Buffered string test complete\n");
}

void testString() {
    printf("\n");
    testStringUnbuffered();
    testStringBuffered();
}

mut testMutex;

void *unbufferedStructSend(void *args) {
    chan_t *chan = args;
    mut lock;
    pthread_mutex_init(&lock, NULL);
    testMutex = lock;
    printf("\tSending a struct\n");
    chanSend(chan, &lock);
    printf("\tA struct sent\n");
}

void *unbufferedStructRecieve(void *args) {
    chan_t *chan = args;
    mut lock;
    printf("\tRecieveing a struct in 2 seconds.\n");
    sleep(2);
    chanRecv(chan, &lock);
    ASSERT(!(strcmp(lock.__size, testMutex.__size)) && lock.__align == testMutex.__align, 
            ("testStringUnbuffered Failed\n"));
    printf("\tA struct recieved\n");
}

void testStructUnbuffered() {
    printf("Starting unbuffered struct test\n");
    chan_t chan;
    chanInit(&chan, sizeof(mut), 1);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, bufferedStringSend, &chan);
    pthread_create(&threads[1], NULL, bufferedStringRecieve, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Unbuffered struct test complete\n");
}

sem_t testSem;

void *bufferedStructSend(void *args) {
    chan_t *chan = args;
    sem_t sem;
    testSem = sem;
    sem_init(&sem, 0, rand());
    testSem = sem;
    printf("\tSending a struct\n");
    chanSend(chan, &sem);
    printf("\tA struct sent\n");
}

void *bufferedStructRecieve(void *args) {
    chan_t *chan = args;
    sem_t sem;
    int semVal, testSemVal;
    sem_getvalue(&testSem, &testSemVal);
    printf("\tRecieveing a struct in 2 seconds.\n");
    sleep(2);
    chanRecv(chan, &sem);
    sem_getvalue(&sem, &semVal);
    ASSERT(!(strcmp(sem.__size, testSem.__size)) && semVal == testSemVal, 
            ("testStructbuffered Failed\n"));
    printf("\tA struct recieved\n");
}

void testStructBuffered() {
    printf("Starting buffered struct test\n");
    chan_t chan;
    chanInit(&chan, sizeof(sem_t), 1);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, bufferedStructSend, &chan);
    pthread_create(&threads[1], NULL, bufferedStructRecieve, &chan);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    chanDestroy(&chan);
    printf("Buffered struct test complete\n");
}


void testStruct() {
    printf("\n");
    testStructUnbuffered(); // testing on mutexes
    testStructBuffered(); /// testing on semaphores
}

int main() {
    srand(time(0));

    testInt();
    testDouble();
    testString();
    // testStruct();

    printf("\nTest Complete\n");
    return 0;
}

