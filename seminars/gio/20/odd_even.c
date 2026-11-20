#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Print 1, 2, ..., n in order, with two threads: one prints the odd
 * numbers, the other one the even numbers.
 *
 *   Print(x)                        use it instead of printf. It's in the tests: it writes
 *                                   x down, so they can check the order, and which thread
 *                                   printed it. The first test shows what was printed
 *
 *   pthread_create(&t, NULL, fn, arg), pthread_join(t, NULL)    as in seminars 18 and 19
 *
 *   sem_t s;                        a semaphore: a counter that is never below 0
 *   sem_init(&s, 0, value)          the counter starts at value (the 0: only threads
 *                                   of this program use it)
 *   sem_wait(&s)                    waits until the counter is above 0, then counter--
 *   sem_post(&s)                    counter++: if a thread waits, it can go on
 *   sem_destroy(&s)                 after the last use
 */

void Print(int x);

// TODO: what each thread needs to know, and the function the threads run
typedef struct{
  int n;
  int first; // 1: this thread prints 1, 3, 5, ... 2: this thread prints: 2, 4, 6, 8 ...
  sem_t *myTurn;
  sem_t *otherTurn;

}Args;


void *PrintEveryOther(void *a){
  Args *args = a;
  for(int i = args->first; i <= args->n; i += 2){
    sem_wait(args->myTurn);
    Print(i);
    sem_post(args->otherTurn);
  }
  return NULL;

}



/* prints 1, 2, ..., n in order: the odd numbers from one thread, the even ones from another */
void PrintNumbers(int n) {
  sem_t oddTurn, evenTurn;
  sem_init(&oddTurn, 0, 1);
  sem_init(&evenTurn, 0, 0);
  Args odd = {n, 1, &oddTurn, &evenTurn};
  Args even = {n, 2, &evenTurn, &oddTurn};
  pthread_t oddThread, evenThread;
  pthread_create(&oddThread, NULL, PrintEveryOther, &odd);
  pthread_create(&evenThread, NULL, PrintEveryOther, &even);
  pthread_join(oddThread, NULL);
  pthread_join(evenThread, NULL);
  sem_destroy(&oddTurn);
  sem_destroy(&evenTurn);
}

/* ---------------- tests (don't touch) ---------------- */

typedef void (*PrintFn)(int n);

static int passed = 0, total = 0;

/* the tests' own lock, for everything their threads share */
static pthread_mutex_t testLock = PTHREAD_MUTEX_INITIALIZER;

#define MAX_PRINTED 100000
static int printed[MAX_PRINTED];          // the numbers Print got, in order
static pthread_t printedBy[MAX_PRINTED];  // and the thread that called Print
static int numPrinted = 0;

void Print(int x) {
  pthread_mutex_lock(&testLock);
  if (numPrinted < MAX_PRINTED) {
    printed[numPrinted] = x;
    printedBy[numPrinted] = pthread_self();
  }
  numPrinted++;
  pthread_mutex_unlock(&testLock);
}

/* the time in seconds */
static double Now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

static void Pause(int ms) {
  struct timespec ts = {ms / 1000, (ms % 1000) * 1000000L};
  nanosleep(&ts, NULL);
}

/* reads an int that other threads change */
static int Get(int *x) {
  pthread_mutex_lock(&testLock);
  int value = *x;
  pthread_mutex_unlock(&testLock);
  return value;
}

static void Add(int *x, int d) {
  pthread_mutex_lock(&testLock);
  *x += d;
  pthread_mutex_unlock(&testLock);
}

/* waits until *x >= target, at most ms milliseconds. Did it get there? */
static bool WaitFor(int *x, int target, int ms) {
  double end = Now() + ms / 1000.0;
  while (Get(x) < target) {
    if (Now() > end) return false;
    Pause(1);
  }
  return true;
}

/* the printed numbers from `from` up to `to`, with "..." where some are left out */
static void ShowPrinted(char *buf, int from, int to) {
  int count = numPrinted < MAX_PRINTED ? numPrinted : MAX_PRINTED;
  if (from < 0) from = 0;
  if (to > count) to = count;
  buf[0] = '\0';
  if (count == 0) {
    strcat(buf, "nothing");
    return;
  }
  if (from > 0) strcat(buf, "... ");
  for (int i = from; i < to; i++) sprintf(buf + strlen(buf), "%d ", printed[i]);
  if (to < count) strcat(buf, "...");
}

/* One test: fn(n) is called `times` times, in a thread of its own,
   so that if it hangs, the test can say so and go on */
typedef struct {
  PrintFn fn;
  int n, times;
  int calls;          // how many calls returned
  int done;           // 1: all the calls returned, or one was wrong
  int abandoned;      // 1: the test gave up waiting, no more calls
  char error[300];    // what was wrong, "" if nothing
  double best;        // the fastest call, in ms
} Run;

/* after a call: were 1, 2, ..., n printed in order, by two threads that aren't `me`? */
static void CheckPrinted(Run *run, pthread_t me) {
  int n = run->n;
  char shown[200];
  int count = Get(&numPrinted);
  if (count < n) {   // maybe its threads are still printing
    Pause(50);
    if (Get(&numPrinted) > count) {
      sprintf(run->error, "it returned when %d of the %d numbers were printed. Does it join both threads?", count, n);
      return;
    }
  }
  pthread_mutex_lock(&testLock);
  if (numPrinted == 0 && n > 0) {
    sprintf(run->error, "nothing was printed: Print was never called");
  } else if (numPrinted != n) {
    ShowPrinted(shown, 0, 10);
    sprintf(run->error, "%d numbers were printed, should be %d. Printed: %s", numPrinted, n, shown);
  } else {
    for (int i = 0; i < n && run->error[0] == '\0'; i++) {
      if (printed[i] != i + 1) {
        ShowPrinted(shown, i < 4 ? 0 : i - 4, i + 6);
        sprintf(run->error, "the order is wrong. Printed: %s", shown);
      }
    }
    for (int i = 0; i < n && run->error[0] == '\0'; i++) {
      if (pthread_equal(printedBy[i], me)) {
        sprintf(run->error, "%d was printed by PrintNumbers itself, not by one of its threads", i + 1);
      } else if (i == 1 && pthread_equal(printedBy[1], printedBy[0])) {
        sprintf(run->error, "1 and 2 were printed by the same thread");
      } else if (i >= 2 && !pthread_equal(printedBy[i], printedBy[i % 2])) {
        sprintf(run->error, "%d and %d were printed by different threads", i % 2 + 1, i + 1);
      }
    }
  }
  pthread_mutex_unlock(&testLock);
}

static void *RunCalls(void *a) {
  Run *run = a;
  for (int t = 0; t < run->times && run->error[0] == '\0' && !Get(&run->abandoned); t++) {
    pthread_mutex_lock(&testLock);
    numPrinted = 0;
    pthread_mutex_unlock(&testLock);
    double start = Now();
    run->fn(run->n);
    double ms = (Now() - start) * 1000;
    if (ms < run->best) run->best = ms;
    Add(&run->calls, 1);
    CheckPrinted(run, pthread_self());
  }
  Add(&run->done, 1);
  return NULL;
}

/* is a call still printing? Checks for 0.2 seconds */
static bool StillPrinting(Run *run) {
  int before = Get(&numPrinted);
  Pause(200);
  return Get(&run->done) == 1 || Get(&numPrinted) != before;
}

static void Check(PrintFn fn, const char *name, int n, int times, int seconds) {
  printf("  [ .. ] %s", name);
  fflush(stdout);   // if a test hangs, its name is already on the screen
  total++;
  Run *run = calloc(1, sizeof(Run));
  run->fn = fn;
  run->n = n;
  run->times = times;
  run->best = 1e9;
  pthread_t thread;
  pthread_create(&thread, NULL, RunCalls, run);
  pthread_detach(thread);   // nobody joins it: the test waits for run->done
  if (!WaitFor(&run->done, 1, seconds * 1000)) {
    Add(&run->abandoned, 1);   // no more calls after the one it's in
    int calls = Get(&run->calls);
    bool printing = StillPrinting(run);
    printf("\r  [FAIL]\n         %s: ", printing ? "too slow" : "hangs");
    if (times == 1) printf("it didn't return in %d seconds\n", seconds);
    else printf("after %d seconds, %d of the %d calls had returned\n", seconds, calls, times);
    if (!printing) {
      char shown[200];
      pthread_mutex_lock(&testLock);
      ShowPrinted(shown, numPrinted - 10, numPrinted);
      pthread_mutex_unlock(&testLock);
      printf("         the call it's in printed: %s\n", shown);
    }
    // wait for the call it's in, so that it doesn't print in the next test
    while (printing && Get(&run->done) == 0) printing = StillPrinting(run);
    if (Get(&run->done) == 1) free(run);   // else its threads may still wake up and use run
    return;
  }
  bool ok = run->error[0] == '\0';
  printf("\r  [%s]\n", ok ? " OK " : "FAIL");
  if (ok) passed++;
  else if (times == 1) printf("         %s\n", run->error);
  else printf("         call %d of %d: %s\n", run->calls, times, run->error);
  if (ok && n == 10) {
    char shown[200];
    pthread_mutex_lock(&testLock);
    ShowPrinted(shown, 0, 10);
    pthread_mutex_unlock(&testLock);
    printf("         printed: %s\n", shown);
  }
  if (ok && n >= 10000) printf("         the fastest call: %.1f ms\n", run->best);
  free(run);
}

static void TestPrintNumbers(const char *title, PrintFn fn) {
  printf("%s\n", title);
  passed = total = 0;
  Check(fn, "n = 10", 10, 1, 3);
  Check(fn, "n = 1: the even thread prints nothing", 1, 1, 3);
  Check(fn, "n = 0: nothing is printed", 0, 1, 3);
  Check(fn, "n = 100, 100 times: a wrong order doesn't show up every time", 100, 100, 5);
  Check(fn, "n = 10000, 10 times", 10000, 10, 20);
  printf("%d / %d tests passed\n", passed, total);
}


int main(void) {
  TestPrintNumbers("PrintNumbers", PrintNumbers);
  return 0;
}
