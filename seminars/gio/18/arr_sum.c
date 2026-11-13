#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * The sum of an array, computed by num_threads threads at once.
 *
 *   pthread_create(&thread, NULL, fn, arg)   starts a thread that runs fn(arg)
 *                                           fn must be  void *fn(void *arg)
 *   pthread_join(thread, NULL)               waits until that thread is done
 *
 *   pthread_mutex_t lock;
 *   pthread_mutex_init(&lock, NULL)          before the first use
 *   pthread_mutex_lock(&lock)                only one thread at a time gets past it;
 *   pthread_mutex_unlock(&lock)              the others wait for the unlock
 *   pthread_mutex_destroy(&lock)             after the last use
 */

// TODO: a struct for what each thread needs to know, and the function the threads run
typedef struct{
  int *arr;
  int start;
  int end;
  int *sum;
  pthread_mutex_t *lock;
} Args;



void *AddNoLock(void* a){
  Args *args = a;
  for(int i = args-> start; i < args->end; i++){
    *args->sum += args->arr[i];
  }
  return NULL;
}

void *AddLockEach(void* a){
  Args *args = a;
  for(int i = args-> start; i < args->end; i++){
    pthread_mutex_lock(args->lock);
    *args->sum += args->arr[i];
    pthread_mutex_unlock(args->lock);
  }
  return NULL;
}

void *AddPart(void* a){

  Args *args = a;
  int local = 0;
  for(int i = args-> start; i < args->end; i++){
    local += args->arr[i];
  }

  pthread_mutex_lock(args->lock);
  *args->sum += local;
  pthread_mutex_unlock(args->lock);
  return NULL;
}


int SumInParts(int *arr, int n, int num_threads, void*(*add)(void *)){
  int sum = 0;
  pthread_mutex_t lock;
  pthread_mutex_init(&lock,  NULL);

  pthread_t threads[num_threads];
  Args args[num_threads];
  int part = n / num_threads;
  for(int i = 0; i < num_threads; i++){
    args[i].arr = arr;
    args[i].start = i * part;
    args[i].end = (i == num_threads - 1) ? n : args[i].start + part;
    args[i].sum = &sum;
    args[i].lock = &lock;
    pthread_create(&threads[i], NULL, add, &args[i]);
  }
  for (int i = 0; i < num_threads; i++){
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&lock);
  return sum;
}



/* arr[0] + arr[1] + ... + arr[n - 1], computed by num_threads threads */
int ArraySumNoLock(int *arr, int n, int num_threads) {
  return SumInParts(arr, n, num_threads, AddNoLock);
}

int ArraySumLockEach(int *arr, int n, int num_threads) {
  return SumInParts(arr, n, num_threads, AddLockEach);
}

int ArraySum(int *arr, int n, int num_threads) {
   return SumInParts(arr, n, num_threads, AddPart);
}

/* ---------------- tests (don't touch) ---------------- */

typedef int (*SumFn)(int *arr, int n, int num_threads);

/* ThreadSanitizer (make tsan) makes every lock much slower. It finds races
   in small arrays too, so with it the two big tests are 100 times smaller. */
#ifdef __SANITIZE_THREAD__
#define BIG 10000
#else
#define BIG 1000000
#endif

static int passed = 0, total = 0;

/* a simple random number generator, the same numbers on every computer */
static unsigned seed = 12345;

static int Random(int n) {
  seed = seed * 1103515245 + 12345;
  return (seed >> 16) % n;
}

/* the time in seconds */
static double Now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* the answer, with one thread and a plain loop */
static int PlainSum(int *arr, int n) {
  int sum = 0;
  for (int i = 0; i < n; i++) sum += arr[i];
  return sum;
}

/* calls sum(arr, n, num_threads) `times` times: a race doesn't show up every time */
static void Check(SumFn sum, const char *name, int *arr, int n, int num_threads, int times) {
  printf("  [ .. ] %s", name);
  fflush(stdout);   // if a test hangs, its name is already on the screen
  total++;
  int expected = PlainSum(arr, n);
  int right = 0, wrong = 0;
  double best = 1e9;   // the fastest call, in ms
  for (int t = 0; t < times; t++) {
    double start = Now();
    int got = sum(arr, n, num_threads);
    double ms = (Now() - start) * 1000;
    if (ms < best) best = ms;
    if (got == expected) right++;
    else wrong = got;
  }

  printf("\r  [%s]\n", right == times ? " OK " : "FAIL");
  if (right == times) passed++;
  else if (times == 1) printf("         returned %d, should be %d\n", wrong, expected);
  else printf("         right %d times out of %d. Once it returned %d, should be %d\n",
              right, times, wrong, expected);

  if (n >= BIG) {   // the big ones: how long did it take?
    double plainBest = 1e9;
    for (int t = 0; t < times; t++) {
      double start = Now();
      PlainSum(arr, n);
      double ms = (Now() - start) * 1000;
      if (ms < plainBest) plainBest = ms;
    }
    printf("         the fastest call: %.1f ms. A plain loop: %.1f ms\n", best, plainBest);
  }
}

static int *RandomArray(int n) {
  int *arr = malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) arr[i] = Random(100);
  return arr;
}

static void RunTests(const char *title, SumFn sum) {
  printf("%s\n", title);
  passed = total = 0;
  seed = 12345;

  int none[1] = {0};
  Check(sum, "n = 0, 4 threads: the sum is 0", none, 0, 4, 1);

  int ones[10];
  for (int i = 0; i < 10; i++) ones[i] = 1;
  Check(sum, "ten 1s, 1 thread", ones, 10, 1, 1);

  int upTo100[100];
  for (int i = 0; i < 100; i++) upTo100[i] = i + 1;
  Check(sum, "1 + 2 + ... + 100, 4 threads", upTo100, 100, 4, 1);

  int *arr = RandomArray(1003);
  Check(sum, "n = 1003, 10 threads: 1003 isn't a multiple of 10", arr, 1003, 10, 10);
  free(arr);

  int seven[7] = {1, 2, 3, 4, 5, 6, 7};
  Check(sum, "n = 7, 10 threads: more threads than numbers", seven, 7, 10, 10);

  char name[100];
  arr = RandomArray(BIG);
  sprintf(name, "n = %d, 10 threads, 10 times", BIG);
  Check(sum, name, arr, BIG, 10, 10);
  free(arr);

  arr = RandomArray(10 * BIG);
  sprintf(name, "n = %d, 10 threads, 3 times", 10 * BIG);
  Check(sum, name, arr, 10 * BIG, 10, 3);
  free(arr);

  printf("%d / %d tests passed\n", passed, total);
}

int main(void) {
  RunTests("ArraySum", ArraySum);
  return 0;
}
