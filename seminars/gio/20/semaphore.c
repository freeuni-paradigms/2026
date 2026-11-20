#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Our own semaphore, made of a mutex and a condition variable.
 * It works like sem_t in odd_even.c:
 *
 *   SemInit(&s, value)              like sem_init(&s, 0, value): the counter starts at value
 *   SemWait(&s)                     like sem_wait: waits until the counter is above 0,
 *                                   then counter--
 *   SemPost(&s)                     like sem_post: counter++, and a waiting thread can go on
 *   SemDestroy(&s)                  like sem_destroy: after the last use
 *
 *   pthread_mutex_init(&lock, NULL), pthread_mutex_lock / unlock / destroy(&lock)
 *                                   as in seminars 18 and 19
 *
 *   pthread_cond_t c;               a condition variable: threads sleep on it until
 *                                   another thread wakes them up
 *   pthread_cond_init(&c, NULL)
 *   pthread_cond_wait(&c, &lock)    call it holding lock: unlocks it and sleeps. Locks
 *                                   it again before it returns
 *   pthread_cond_signal(&c)         wakes up one thread sleeping on c (if there is one)
 *   pthread_cond_broadcast(&c)      wakes up all of them
 *   pthread_cond_destroy(&c)        after the last use
 */

typedef struct {
  int value;
  pthread_mutex_t lock;
  pthread_cond_t cond;
} Semaphore;

void SemInit(Semaphore *s, int value) {
  s->value = value;
  pthread_mutex_init(&s->lock, NULL);
  pthread_cond_init(&s->cond, NULL);
}

void SemWait(Semaphore *s) {
  pthread_mutex_lock(&s->lock);
  while(s->value == 0){
    pthread_cond_wait(&s->cond, &s->lock);
  }
  s->value--;
  pthread_mutex_unlock(&s->lock);
}

void SemPost(Semaphore *s) {
  pthread_mutex_lock(&s->lock);
  s->value++;
  pthread_cond_signal(&s->cond);
  pthread_mutex_unlock(&s->lock);
}

void SemDestroy(Semaphore *s) {
  pthread_mutex_destroy(&s->lock);
  pthread_cond_destroy(&s->cond);
}

/* ---------------- tests (don't touch) ---------------- */

static int passed = 0, total = 0;

/* the tests' own lock, for everything their threads share */
static pthread_mutex_t testLock = PTHREAD_MUTEX_INITIALIZER;

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

/* One semaphore test's semaphore, and what its threads did */
typedef struct {
  Semaphore s;
  int through;   // how many SemWaits got through
  int posts;     // for Poster: how many SemPosts to do
  int posted;    // how many SemPosts returned
  int stop;      // for Looper: stop after the next SemWait
  int stopped;   // how many Loopers stopped
} SemTest;

static SemTest *NewSemTest(int value) {
  SemTest *t = calloc(1, sizeof(SemTest));
  memset(&t->s, 0xAB, sizeof(t->s));   // garbage, like a local on the stack: SemInit sets it up
  SemInit(&t->s, value);
  return t;
}

/* only when no thread uses it any more. If one may still be waiting, t isn't freed */
static void FreeSemTest(SemTest *t) {
  SemDestroy(&t->s);
  free(t);
}

/* SemWait once */
static void *Waiter(void *a) {
  SemTest *t = a;
  SemWait(&t->s);
  Add(&t->through, 1);
  return NULL;
}

/* SemPost t->posts times */
static void *Poster(void *a) {
  SemTest *t = a;
  int posts = t->posts;
  for (int i = 0; i < posts; i++) {
    SemPost(&t->s);
    Add(&t->posted, 1);
  }
  return NULL;
}

/* SemWait again and again, until t->stop */
static void *Looper(void *a) {
  SemTest *t = a;
  while (true) {
    SemWait(&t->s);
    pthread_mutex_lock(&testLock);
    t->through++;
    bool stop = t->stop;
    pthread_mutex_unlock(&testLock);
    if (stop) break;
  }
  Add(&t->stopped, 1);
  return NULL;
}

/* nobody joins these threads: the tests wait for their counts instead */
static void Start(void *(*fn)(void *), SemTest *t) {
  pthread_t thread;
  pthread_create(&thread, NULL, fn, t);
  pthread_detach(thread);
}

/* SemPost k times, from another thread. Did they all return within a second? */
static bool PostFromThread(SemTest *t, int k) {
  int before = Get(&t->posted);
  t->posts = k;
  Start(Poster, t);
  return WaitFor(&t->posted, before + k, 1000);
}

/* Each test returns "" if all went well, or what was wrong. When a thread
   may still be waiting, the test returns without freeing its SemTest */

static const char *TwoWaits(char *why) {
  SemTest *t = NewSemTest(2);
  Start(Waiter, t);
  Start(Waiter, t);
  if (!WaitFor(&t->through, 2, 1000)) {
    sprintf(why, "after 1 second, %d of them got through", Get(&t->through));
    return why;
  }
  FreeSemTest(t);
  return "";
}

static const char *WaitsForPost(char *why) {
  SemTest *t = NewSemTest(0);
  Start(Waiter, t);
  Pause(100);
  if (Get(&t->through) > 0) {
    FreeSemTest(t);
    return "SemWait got through, but nobody called SemPost";
  }
  if (!PostFromThread(t, 1)) return "SemPost didn't return in 1 second";
  if (!WaitFor(&t->through, 1, 1000)) return "after SemPost, SemWait still waits (1 second later)";
  FreeSemTest(t);
  return "";
}

static const char *PostsAreCounted(char *why) {
  SemTest *t = NewSemTest(0);
  if (!PostFromThread(t, 3)) return "SemPost didn't return in 1 second";
  for (int i = 0; i < 3; i++) Start(Waiter, t);
  if (!WaitFor(&t->through, 3, 1000)) {
    sprintf(why, "after 1 second, %d of the 3 SemWaits got through", Get(&t->through));
    return why;
  }
  Start(Waiter, t);
  Pause(100);
  if (Get(&t->through) > 3) {
    FreeSemTest(t);
    return "the 4th SemWait got through too";
  }
  if (!PostFromThread(t, 1)) return "SemPost didn't return in 1 second";
  if (!WaitFor(&t->through, 4, 1000)) return "after one more SemPost, the 4th SemWait still waits (1 second later)";
  FreeSemTest(t);
  return "";
}

static const char *AllWakeUp(char *why) {
  for (int rep = 1; rep <= 20; rep++) {
    SemTest *t = NewSemTest(0);
    for (int i = 0; i < 5; i++) Start(Waiter, t);
    Pause(20);   // now they all wait
    if (Get(&t->through) > 0) {
      sprintf(why, "%d of them got through before any SemPost", Get(&t->through));
      return why;
    }
    if (!PostFromThread(t, 5)) return "SemPost didn't return in 1 second";
    if (!WaitFor(&t->through, 5, 1000)) {
      sprintf(why, "time %d: after the 5 SemPosts, %d of the 5 woke up (1 second later)", rep, Get(&t->through));
      return why;
    }
    FreeSemTest(t);
  }
  return "";
}

static const char *ExactlyAsMany(char *why) {
  SemTest *t = NewSemTest(0);
  for (int i = 0; i < 4; i++) Start(Looper, t);
  t->posts = 10000;
  Start(Poster, t);
  if (!WaitFor(&t->posted, 10000, 5000)) return "the 10000 SemPosts didn't return in 5 seconds";
  WaitFor(&t->through, 10000, 1000);
  Pause(100);   // in case more get through
  pthread_mutex_lock(&testLock);
  int through = t->through;
  t->stop = 1;   // each looper stops after one more SemWait
  pthread_mutex_unlock(&testLock);
  if (through < 10000) {
    sprintf(why, "after 10000 SemPosts, %d SemWaits got through (1 second later)", through);
    return why;
  }
  if (through > 10000) {
    sprintf(why, "10000 SemPosts, and %d SemWaits got through", through);
    return why;
  }
  if (!PostFromThread(t, 4) || !WaitFor(&t->stopped, 4, 1000)) return "the 4 threads didn't stop";
  FreeSemTest(t);
  return "";
}

static void CheckSem(const char *name, const char *(*test)(char *why)) {
  printf("  [ .. ] %s", name);
  fflush(stdout);
  total++;
  char why[200];
  const char *error = test(why);
  printf("\r  [%s]\n", error[0] == '\0' ? " OK " : "FAIL");
  if (error[0] == '\0') passed++;
  else printf("         %s\n", error);
}

static void TestSemaphore(void) {
  printf("Semaphore\n");
  passed = total = 0;
  CheckSem("SemInit(&s, 2): two SemWaits get through", TwoWaits);
  CheckSem("SemInit(&s, 0): SemWait waits until a SemPost", WaitsForPost);
  CheckSem("3 SemPosts: 3 SemWaits get through, the 4th one waits", PostsAreCounted);
  CheckSem("5 threads wait, then 5 SemPosts: all 5 wake up (20 times)", AllWakeUp);
  CheckSem("4 threads keep calling SemWait, 10000 SemPosts: exactly 10000 get through", ExactlyAsMany);
  printf("%d / %d tests passed\n", passed, total);
}

int main(void) {
  TestSemaphore();
  return 0;
}
