#include "dj_school.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Each test calls StartSchool(students, teachers) in a thread of its own and
 * waits for it to return. helper.c checks every call of the helper functions
 * against the rules (who may call what, and when); a test that breaks one
 * shows the first broken rule and what happened just before it.
 *
 * The helper functions sleep 0-2 ms (on the exam 0-4 seconds). The first
 * test prints what happened, like the exam's tests.c.
 *
 * If nothing happens for 2 seconds the test says "hangs" and shows who isn't
 * done and where they are. Then the next tests are skipped: the threads of a
 * school that didn't finish would mix with the next school's.
 */

/* in helper.c */
void CheckStart(int num_students, int num_teachers, int max_sleep_ms, bool print, pthread_t school);
void CheckProblem(char *buf);
bool CheckThreadsDone(void);
bool CheckSchoolEnded(void);
int CheckCalls(void);
void CheckShowNotDone(void);
void CheckShowEventsBeforeProblem(void);

static pthread_mutex_t testLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t testCond = PTHREAD_COND_INITIALIZER;
static int passed = 0, total = 0;
static bool stopped = false;   // a school didn't finish: the next tests are skipped

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

typedef struct {
  int students, teachers, sleepMs;
  bool print;
  bool returned;   // StartSchool returned. Under testLock
  bool mayExit;    // the test is over. Under testLock
} Run;

static void *RunSchool(void *a) {
  Run *run = a;
  CheckStart(run->students, run->teachers, run->sleepMs, run->print, pthread_self());
  StartSchool(run->students, run->teachers);
  pthread_mutex_lock(&testLock);
  run->returned = true;
  // StartSchool gave its threads pointers into its stack (&teacher_id[i]). If it
  // returned too early, they may still be reading them: the stack stays until
  // the test is over
  while (!run->mayExit) pthread_cond_wait(&testCond, &testLock);
  pthread_mutex_unlock(&testLock);
  free(run);
  return NULL;
}

static bool Returned(Run *run) {
  pthread_mutex_lock(&testLock);
  bool returned = run->returned;
  pthread_mutex_unlock(&testLock);
  return returned;
}

typedef struct {
  bool returned;      // StartSchool returned
  bool hangs;         // it didn't, and nothing happened for 2 seconds
  bool tooSlow;       // it didn't return in 30 seconds, but things were still happening
  bool allDone;       // every student and every teacher is done
  bool ended;         // SchoolEnd was called
  char problem[300];  // the first broken rule, "" if none
} Outcome;

/* has the number of helper calls changed in the last ms milliseconds? */
static bool StillGoing(int ms) {
  int before = CheckCalls();
  Pause(ms);
  return CheckCalls() != before;
}

static Outcome School(int students, int teachers, int sleepMs, bool print) {
  Run *run = calloc(1, sizeof(Run));
  run->students = students;
  run->teachers = teachers;
  run->sleepMs = sleepMs;
  run->print = print;
  pthread_t thread;
  pthread_create(&thread, NULL, RunSchool, run);
  pthread_detach(thread);   // nobody joins it: School waits for run->returned

  Outcome o = {0};
  double start = Now(), lastCall = start;
  int calls = CheckCalls();
  while (!(o.returned = Returned(run))) {
    Pause(5);
    int c = CheckCalls();
    if (c != calls) {
      calls = c;
      lastCall = Now();
    }
    if (Now() - lastCall > 2) {
      o.hangs = true;
      break;
    }
    if (Now() - start > 30) {
      o.tooSlow = true;
      break;
    }
  }
  if (o.returned) {
    // StartSchool returned: everybody should be done. If somebody isn't, give it a moment
    for (int i = 0; i < 10 && !CheckThreadsDone() && StillGoing(200); i++) {
    }
    Pause(20);   // is anything called after SchoolEnd?
  }
  o.allDone = CheckThreadsDone();
  if (o.returned && o.allDone) {   // else its threads may still need StartSchool's stack
    pthread_mutex_lock(&testLock);
    run->mayExit = true;
    pthread_cond_broadcast(&testCond);
    pthread_mutex_unlock(&testLock);
  }
  o.ended = CheckSchoolEnded();
  CheckProblem(o.problem);
  return o;
}

static bool Ok(Outcome *o) {
  return o->returned && o->allDone && o->ended && o->problem[0] == '\0';
}

/* the lines under [FAIL] */
static void ShowWhy(Outcome *o) {
  if (o->problem[0] != '\0') {
    printf("         %s\n", o->problem);
    CheckShowEventsBeforeProblem();
  }
  if (o->hangs) {
    printf("         %s: StartSchool didn't return, and nothing has happened for 2 seconds. Not done:\n",
           o->problem[0] != '\0' ? "then it hangs" : "hangs");
    CheckShowNotDone();
  } else if (o->tooSlow) {
    printf("         too slow: StartSchool didn't return in 30 seconds\n");
  } else if (o->problem[0] == '\0') {
    if (!o->ended) printf("         StartSchool returned, but SchoolEnd wasn't called\n");
    if (!o->allDone) {
      printf("         StartSchool returned, but not everybody is done:\n");
      CheckShowNotDone();
    }
  }
  if (!o->returned || !o->allDone) {
    printf("         The next tests are skipped: this school's threads aren't done, and they would\n"
           "         mix with the next school's.\n");
    stopped = true;
  }
}

static bool Start(const char *name, bool print) {
  total++;
  if (stopped) {
    printf("  [SKIP] %s\n", name);
    return false;
  }
  printf(print ? "  [ .. ] %s\n" : "  [ .. ] %s", name);
  fflush(stdout);   // if it hangs, its name is already on the screen
  return true;
}

static void Result(const char *name, bool print, bool ok) {
  if (print) printf("  [%s] %s\n", ok ? " OK " : "FAIL", name);
  else printf("\r  [%s]\n", ok ? " OK " : "FAIL");
  if (ok) passed++;
}

/* one school */
static void Test(const char *name, int students, int teachers, bool print) {
  if (!Start(name, print)) return;
  Outcome o = School(students, teachers, 2, print);
  Result(name, print, Ok(&o));
  if (!Ok(&o)) ShowWhy(&o);
}

/* many small schools, one after another, and nobody sleeps */
static void TestMany(const char *name, int schools) {
  if (!Start(name, false)) return;
  for (int i = 0; i < schools; i++) {
    int students = 1 + i % 8, teachers = 1 + i % 3;
    Outcome o = School(students, teachers, 0, false);
    if (!Ok(&o)) {
      Result(name, false, false);
      printf("         school %d of %d (%d students, %d teachers):\n", i + 1, schools, students, teachers);
      ShowWhy(&o);
      return;
    }
  }
  Result(name, false, true);
}

int main(void) {
  printf("DJ School\n");
  Test("1 student, 1 teacher", 1, 1, true);
  Test("3 students, 5 teachers: more teachers than students", 3, 5, false);
  Test("5 students, 1 teacher", 5, 1, false);
  Test("20 students, 3 teachers", 20, 3, false);
  Test("100 students, 10 teachers: the most there can be", 100, 10, false);
  Test("10 students, 2 teachers: fewer teachers than the school before", 10, 2, false);
  TestMany("50 small schools in a row, and nobody sleeps", 50);
  printf("%d / %d tests passed\n", passed, total);
  return 0;
}
