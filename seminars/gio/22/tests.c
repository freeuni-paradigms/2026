#include "robot.h"
#include "competition.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Each test plays one competition (one test plays two at the same time):
 * CompetitionInit, CompetitionStart, CompetitionAddParticipant for each robot,
 * then CompetitionStop. It runs in a thread of its own, and the test waits.
 *
 * The robots' fight function is here. Each robot has a strength (not its id),
 * and the stronger one wins. A fight sleeps 0-2 ms (on the exam up to 39
 * seconds). The fight function checks:
 *   - both robots are in the competition, and neither one is in another fight
 *     right now or has lost already;
 *   - the fight runs in the competition's own threads, not inside
 *     CompetitionAddParticipant or CompetitionStop;
 *   - no fight starts after CompetitionStop returned.
 * When CompetitionStop returns, every fight has to be over, and it has to
 * return the one robot that hasn't lost.
 *
 * A test that breaks a rule shows the first broken rule and what happened just
 * before it. If nothing happens for 2 seconds the test says "hangs" and shows
 * where the test is stuck. Then the next tests are skipped: that
 * competition's threads would mix with the next one's.
 */

#define LOG 8

enum State { NOT_ADDED, FREE, FIGHTING, LOST, REJECTED };
enum Mode { NORMAL, WAIT_FOR_FIGHT, ADD_DURING_STOP };

typedef struct {
  const char *label;          // "" or "competition 1: "
  int n;                      // robots to add
  int fightMs;                // a fight sleeps 0..fightMs ms
  bool fixedFight;            // ... or exactly fightMs ms
  int addPauseMs;             // a pause of 0..addPauseMs ms after each CompetitionAddParticipant
  bool fixedAddPause;         // ... or exactly addPauseMs ms
  bool print;
  enum Mode mode;
  Competition *comp;
  Robot *robots;              // n + 1: the last one is for ADD_DURING_STOP
  int *strength;
  enum State *state;
  int *opponent;              // FIGHTING: whom it fights. LOST: who beat it
  bool *accepted;             // CompetitionAddParticipant returned true
  int fightsNow, maxFightsNow, fightsStarted;
  int progress;               // goes up with every call. If it stops, nothing is happening
  bool stopCalled, stopReturned, driverDone;
  const Robot *returned;      // what CompetitionStop returned
  char where[100];            // where the test's thread is
  pthread_t driver, stopper;
  bool stopperKnown;
  unsigned seed;
  char problem[300];          // the first broken rule, "" if none
  char events[LOG][100];
  int numEvents;
  char eventsBeforeProblem[LOG][100];
  int numEventsBeforeProblem;
} Arena;

static pthread_mutex_t testLock = PTHREAD_MUTEX_INITIALIZER;   // for every Arena
static pthread_cond_t testCond = PTHREAD_COND_INITIALIZER;     // a competition finished
static int passed = 0, total = 0;
static bool stopped = false;   // a competition didn't finish: the next tests are skipped

/* ---- helpers ---- */

static double Now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

static void Pause(int ms) {
  struct timespec ts = {ms / 1000, (ms % 1000) * 1000000L};
  nanosleep(&ts, NULL);
}

/* under testLock */
static int Random(Arena *a, int n) {
  a->seed = a->seed * 1103515245 + 12345;
  return (a->seed >> 16) % n;
}

/* under testLock */
static void Problem(Arena *a, const char *format, ...) {
  if (a->problem[0] != '\0') return;   // only the first one
  int len = sprintf(a->problem, "%s", a->label);
  va_list ap;
  va_start(ap, format);
  vsnprintf(a->problem + len, sizeof(a->problem) - len, format, ap);
  va_end(ap);
  a->numEventsBeforeProblem = a->numEvents < LOG ? a->numEvents : LOG;
  for (int i = 0; i < a->numEventsBeforeProblem; i++) {
    strcpy(a->eventsBeforeProblem[i], a->events[(a->numEvents - a->numEventsBeforeProblem + i) % LOG]);
  }
}

/* under testLock */
static void Event(Arena *a, const char *format, ...) {
  char *line = a->events[a->numEvents % LOG];
  va_list ap;
  va_start(ap, format);
  vsnprintf(line, 100, format, ap);
  va_end(ap);
  a->numEvents++;
  if (a->print) printf("         %s\n", line);
}

/* ---- the robots' fight ---- */

/* under testLock: may robot i fight robot other? */
static void CheckFighter(Arena *a, int i, int other) {
  switch (a->state[i]) {
    case NOT_ADDED: Problem(a, "robot %d fights robot %d, but robot %d wasn't added", i, other, i); break;
    case REJECTED: Problem(a, "robot %d fights robot %d, but CompetitionAddParticipant rejected robot %d", i, other, i); break;
    case FIGHTING: Problem(a, "robot %d fights robot %d, but robot %d is fighting robot %d right now", i, other, i, a->opponent[i]); break;
    case LOST: Problem(a, "robot %d fights robot %d, but robot %d already lost to robot %d", i, other, i, a->opponent[i]); break;
    case FREE: break;
  }
}

static bool Ours(Arena *a, const Robot *r) {
  return r >= a->robots && r <= a->robots + a->n;
}

static bool FightRobots(const Robot *x, const Robot *y) {
  Arena *a = x->data;
  pthread_mutex_lock(&testLock);
  a->progress++;
  if (y->data != x->data) {
    Problem(a, "robot %d fights robot %d of another competition. Is something global?", x->id, y->id);
    pthread_mutex_unlock(&testLock);
    return true;
  }
  if (!Ours(a, x) || !Ours(a, y)) {
    Problem(a, "RobotFightWith got a pointer that isn't one of the robots");
    pthread_mutex_unlock(&testLock);
    return true;
  }
  int i = x->id, j = y->id;
  if (i == j) {
    Problem(a, "robot %d fights itself", i);
  } else {
    CheckFighter(a, i, j);
    CheckFighter(a, j, i);
  }
  pthread_t self = pthread_self();
  if (pthread_equal(self, a->driver) && !a->driverDone) {
    Problem(a, "robots %d and %d fight inside %s, in the test's thread. The fights belong in the competition's threads", i, j, a->where);
  }
  if (a->stopperKnown && pthread_equal(self, a->stopper) && !a->stopReturned) {
    Problem(a, "robots %d and %d fight inside CompetitionStop. The fights belong in the competition's threads", i, j);
  }
  if (a->stopReturned) Problem(a, "robots %d and %d fight after CompetitionStop returned", i, j);
  a->state[i] = a->state[j] = FIGHTING;
  a->opponent[i] = j;
  a->opponent[j] = i;
  a->fightsNow++;
  a->fightsStarted++;
  if (a->fightsNow > a->maxFightsNow) a->maxFightsNow = a->fightsNow;
  Event(a, "robot %d vs robot %d", i, j);
  int ms = a->fixedFight ? a->fightMs : Random(a, a->fightMs + 1);
  pthread_mutex_unlock(&testLock);

  Pause(ms);

  pthread_mutex_lock(&testLock);
  bool xWins = a->strength[i] > a->strength[j];
  int winner = xWins ? i : j, loser = xWins ? j : i;
  a->state[winner] = FREE;
  a->state[loser] = LOST;
  a->opponent[loser] = winner;
  a->fightsNow--;
  a->progress++;
  Event(a, "robot %d beats robot %d", winner, loser);
  pthread_mutex_unlock(&testLock);
  return xWins;
}

/* ---- one competition ---- */

static Arena *NewArena(const char *label, int n, int fightMs, bool print, enum Mode mode, unsigned seed) {
  Arena *a = calloc(1, sizeof(Arena));
  a->label = label;
  a->n = n;
  a->fightMs = fightMs;
  a->print = print;
  a->mode = mode;
  a->seed = seed;
  a->comp = malloc(sizeof(Competition));
  memset(a->comp, 0xAB, sizeof(Competition));   // garbage, like a local on the stack: CompetitionInit sets it up
  a->robots = calloc(n + 1, sizeof(Robot));
  a->strength = calloc(n + 1, sizeof(int));
  a->state = calloc(n + 1, sizeof(enum State));
  a->opponent = calloc(n + 1, sizeof(int));
  a->accepted = calloc(n + 1, sizeof(bool));
  for (int i = 0; i <= n; i++) {
    a->robots[i].id = i;
    a->robots[i].fight_fn = FightRobots;
    a->robots[i].data = a;
    a->strength[i] = i;
  }
  for (int i = n; i > 0; i--) {   // the strengths in a random order
    int j = Random(a, i + 1), t = a->strength[i];
    a->strength[i] = a->strength[j];
    a->strength[j] = t;
  }
  return a;
}

/* only if its threads are done with it */
static void FreeArena(Arena *a) {
  free(a->comp);
  free(a->robots);
  free(a->strength);
  free(a->state);
  free(a->opponent);
  free(a->accepted);
  free(a);
}

/* under testLock: when CompetitionStop returns */
static void CheckWinner(Arena *a, const Robot *w) {
  int added = 0, standing = 0;
  char list[60] = "";
  for (int i = 0; i <= a->n; i++) {
    if (!a->accepted[i]) continue;
    added++;
    if (a->state[i] != LOST) {
      standing++;
      if (standing <= 5) sprintf(list + strlen(list), "%s%d", standing > 1 ? ", " : "", i);
    }
  }
  if (standing > 5) strcat(list, ", ...");
  if (a->fightsNow > 0) {
    int i = 0;
    while (a->state[i] != FIGHTING) i++;
    Problem(a, "CompetitionStop returned while robots %d and %d were still fighting", i, a->opponent[i]);
  } else if (added == 0) {
    if (w != NULL) Problem(a, "CompetitionStop returned a robot, but no robot was added. It should return NULL");
  } else if (w == NULL) {
    Problem(a, "CompetitionStop returned NULL, but robots were added");
  } else if (!Ours(a, w)) {
    Problem(a, "CompetitionStop returned a pointer that isn't one of the robots");
  } else if (standing > 1) {
    Problem(a, "CompetitionStop returned robot %d, but %d robots haven't lost yet: %s", w->id, standing, list);
  } else if (a->state[w->id] == LOST) {
    Problem(a, "CompetitionStop returned robot %d, but it lost to robot %d", w->id, a->opponent[w->id]);
  } else if (!a->accepted[w->id]) {
    Problem(a, "CompetitionStop returned robot %d, but it wasn't in the competition", w->id);
  }
}

static void CallStop(Arena *a) {
  pthread_mutex_lock(&testLock);
  a->stopCalled = true;
  if (a->mode != ADD_DURING_STOP) strcpy(a->where, "CompetitionStop");
  pthread_mutex_unlock(&testLock);
  const Robot *w = CompetitionStop(a->comp);
  pthread_mutex_lock(&testLock);
  a->stopReturned = true;
  a->returned = w;
  a->progress++;
  pthread_cond_broadcast(&testCond);
  CheckWinner(a, w);
  if (w != NULL && Ours(a, w)) Event(a, "CompetitionStop returned robot %d", w->id);
  else if (w == NULL) Event(a, "CompetitionStop returned NULL");
  pthread_mutex_unlock(&testLock);
}

static void *Stopper(void *arg) {
  CallStop(arg);
  return NULL;
}

/* under testLock */
static void SetWhere(Arena *a, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vsnprintf(a->where, sizeof(a->where), format, ap);
  va_end(ap);
}

static bool Add(Arena *a, int i) {
  pthread_mutex_lock(&testLock);
  a->state[i] = FREE;   // it may fight before CompetitionAddParticipant returns
  SetWhere(a, "CompetitionAddParticipant(robot %d)", i);
  pthread_mutex_unlock(&testLock);
  bool ok = CompetitionAddParticipant(a->comp, &a->robots[i]);
  pthread_mutex_lock(&testLock);
  a->progress++;
  if (ok) {
    a->accepted[i] = true;
    if (a->print) Event(a, "robot %d added", i);
  } else if (a->state[i] == FREE) {
    a->state[i] = REJECTED;
  } else {
    Problem(a, "CompetitionAddParticipant rejected robot %d, but it fought", i);
  }
  pthread_mutex_unlock(&testLock);
  return ok;
}

/* the test's thread: plays the competition */
static void *Drive(void *arg) {
  Arena *a = arg;
  pthread_mutex_lock(&testLock);
  a->driver = pthread_self();
  SetWhere(a, "CompetitionInit");
  pthread_mutex_unlock(&testLock);
  CompetitionInit(a->comp);
  pthread_mutex_lock(&testLock);
  SetWhere(a, "CompetitionStart");
  a->progress++;
  pthread_mutex_unlock(&testLock);
  CompetitionStart(a->comp);
  pthread_mutex_lock(&testLock);
  a->progress++;
  if (a->print) {
    char order[100] = "";
    for (int s = a->n; s >= 0; s--) {
      for (int i = 0; i < a->n; i++) {
        if (a->strength[i] == s) sprintf(order + strlen(order), " %d", i);
      }
    }
    Event(a, "robots, the strongest first:%s", order);
  }
  pthread_mutex_unlock(&testLock);

  for (int i = 0; i < a->n; i++) {
    if (!Add(a, i)) {
      pthread_mutex_lock(&testLock);
      Problem(a, "CompetitionAddParticipant(robot %d) returned false, but CompetitionStop wasn't called", i);
      pthread_mutex_unlock(&testLock);
    }
    if (a->addPauseMs > 0 && i < a->n - 1) {
      pthread_mutex_lock(&testLock);
      int ms = a->fixedAddPause ? a->addPauseMs : Random(a, a->addPauseMs + 1);
      pthread_mutex_unlock(&testLock);
      Pause(ms);
    }
  }

  if (a->mode == WAIT_FOR_FIGHT) {
    pthread_mutex_lock(&testLock);
    SetWhere(a, "a pause of 1 second, before CompetitionStop");
    pthread_mutex_unlock(&testLock);
    bool fought = false;
    double start = Now();
    while (!fought && Now() - start < 1) {
      Pause(5);
      pthread_mutex_lock(&testLock);
      fought = a->fightsStarted > 0;
      a->progress++;
      pthread_mutex_unlock(&testLock);
    }
    if (!fought) {
      pthread_mutex_lock(&testLock);
      Problem(a, "in 1 second after robot 1 was added, no fight started. Two free robots should fight right away, not when CompetitionStop is called");
      pthread_mutex_unlock(&testLock);
    }
  }

  if (a->mode == ADD_DURING_STOP) {
    pthread_t stopper;
    pthread_create(&stopper, NULL, Stopper, a);
    pthread_mutex_lock(&testLock);
    a->stopper = stopper;
    a->stopperKnown = true;
    pthread_mutex_unlock(&testLock);
    pthread_detach(stopper);
    Pause(20);
    pthread_mutex_lock(&testLock);
    bool returned = a->stopReturned;
    pthread_mutex_unlock(&testLock);
    if (!returned) {
      if (Add(a, a->n)) {
        pthread_mutex_lock(&testLock);
        Problem(a, "CompetitionAddParticipant(robot %d) returned true while CompetitionStop was waiting for the winner", a->n);
        pthread_mutex_unlock(&testLock);
      }
    }
  } else {
    CallStop(a);
  }
  pthread_mutex_lock(&testLock);
  a->driverDone = true;
  pthread_cond_broadcast(&testCond);
  pthread_mutex_unlock(&testLock);
  return NULL;
}

typedef struct {
  bool finished;   // CompetitionStop returned, and the test's thread is done
  bool hangs;      // it didn't, and nothing happened for 2 seconds
  bool tooSlow;    // it didn't in 30 seconds, but things were still happening
} Outcome;

static bool Finished(Arena *a) {
  return a->stopReturned && a->driverDone;
}

/* plays count competitions at the same time, and waits for them. grace: then waits
   30 ms more, to see whether a fight starts after CompetitionStop returned */
static Outcome Play(Arena **arenas, int count, bool grace) {
  for (int k = 0; k < count; k++) {
    pthread_t t;
    pthread_create(&t, NULL, Drive, arenas[k]);
    pthread_detach(t);   // nobody joins it: Play waits for driverDone
  }
  Outcome o = {0};
  double start = Now(), lastProgress = start;
  int progress = -1;
  pthread_mutex_lock(&testLock);
  while (true) {
    int p = 0;
    bool all = true;
    for (int k = 0; k < count; k++) {
      p += arenas[k]->progress;
      all = all && Finished(arenas[k]);
    }
    if (all) break;
    if (p != progress) {
      progress = p;
      lastProgress = Now();
    }
    if (Now() - lastProgress > 2) {
      o.hangs = true;
      break;
    }
    if (Now() - start > 30) {
      o.tooSlow = true;
      break;
    }
    struct timespec until;   // wake up in 50 ms, or when a competition finishes
    clock_gettime(CLOCK_REALTIME, &until);
    until.tv_nsec += 50 * 1000000L;
    if (until.tv_nsec >= 1000000000L) {
      until.tv_sec++;
      until.tv_nsec -= 1000000000L;
    }
    pthread_cond_timedwait(&testCond, &testLock, &until);
  }
  pthread_mutex_unlock(&testLock);
  if (o.hangs || o.tooSlow) return o;
  if (grace) Pause(30);
  o.finished = true;
  return o;
}

/* under testLock */
static void ShowStuck(Arena *a) {
  if (!a->stopReturned) {
    if (a->stopCalled) printf("         %sCompetitionStop hasn't returned\n", a->label);
    else printf("         %sthe test is stuck in %s\n", a->label, a->where);
  }
  if (!a->driverDone && a->stopCalled && a->mode == ADD_DURING_STOP) {
    printf("         %sthe test is stuck in %s\n", a->label, a->where);
  }
  if (a->fightsNow > 0) {
    printf("         %srobots fighting right now:", a->label);
    for (int i = 0; i <= a->n; i++) {
      if (a->state[i] == FIGHTING && i < a->opponent[i]) printf(" %d vs %d", i, a->opponent[i]);
    }
    printf("\n");
  }
  int shown = 0, standing = 0;
  for (int i = 0; i <= a->n; i++) standing += a->accepted[i] && a->state[i] == FREE;
  if (standing > 0) {
    printf("         %srobots that haven't lost and aren't fighting:", a->label);
    for (int i = 0; i <= a->n && shown < 10; i++) {
      if (a->accepted[i] && a->state[i] == FREE) {
        printf(" %d", i);
        shown++;
      }
    }
    printf(standing > 10 ? " ... (%d)\n" : "\n", standing);
  }
}

static bool Ok(Arena **arenas, int count, Outcome *o) {
  if (!o->finished) return false;
  for (int k = 0; k < count; k++) {
    if (arenas[k]->problem[0] != '\0') return false;
  }
  return true;
}

/* the lines under [FAIL] */
static void ShowWhy(Arena **arenas, int count, Outcome *o) {
  pthread_mutex_lock(&testLock);
  for (int k = 0; k < count; k++) {
    Arena *a = arenas[k];
    if (a->problem[0] == '\0') continue;
    printf("         %s\n", a->problem);
    if (a->numEventsBeforeProblem > 0) printf("         what happened just before:\n");
    for (int i = 0; i < a->numEventsBeforeProblem; i++) printf("           %s\n", a->eventsBeforeProblem[i]);
  }
  if (o->hangs || o->tooSlow) {
    bool problem = false;
    for (int k = 0; k < count; k++) problem = problem || arenas[k]->problem[0] != '\0';
    if (o->hangs) printf("         %s: nothing has happened for 2 seconds\n", problem ? "then it hangs" : "hangs");
    else printf("         too slow: not done in 30 seconds\n");
    for (int k = 0; k < count; k++) ShowStuck(arenas[k]);
    printf("         The next tests are skipped: this competition's threads aren't done, and they\n"
           "         would mix with the next one's.\n");
    stopped = true;
  } else {
    // CompetitionStop returned. Are robots still fighting? Give them a moment
    bool fighting = true;
    for (int t = 0; t < 30 && fighting; t++) {
      fighting = false;
      for (int k = 0; k < count; k++) fighting = fighting || arenas[k]->fightsNow > 0;
      if (fighting) {
        pthread_mutex_unlock(&testLock);
        Pause(10);
        pthread_mutex_lock(&testLock);
      }
    }
    if (fighting) {
      printf("         Robots are still fighting after CompetitionStop returned. The next tests are\n"
             "         skipped: those fights would mix with the next competition's.\n");
      stopped = true;
    }
  }
  pthread_mutex_unlock(&testLock);
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

/* plays the competitions; true if every rule held. Frees them if they're done */
static bool PlayAndCheck(const char *name, Arena **arenas, int count, bool print) {
  Outcome o = Play(arenas, count, true);
  bool ok = Ok(arenas, count, &o);
  if (!ok) {
    Result(name, print, false);
    ShowWhy(arenas, count, &o);
  }
  if (ok) {
    for (int k = 0; k < count; k++) FreeArena(arenas[k]);
  }   // else its threads may still use them
  return ok;
}

/* ---- the tests ---- */

static void Test(const char *name, int n, int fightMs, bool print) {
  if (!Start(name, print)) return;
  Arena *a = NewArena("", n, fightMs, print, NORMAL, 1000 + n);
  if (PlayAndCheck(name, &a, 1, print)) Result(name, print, true);
}

static void TestAddedWhileFighting(const char *name, int n) {
  if (!Start(name, false)) return;
  Arena *a = NewArena("", n, 2, false, NORMAL, 2000 + n);
  a->addPauseMs = 1;
  if (PlayAndCheck(name, &a, 1, false)) Result(name, false, true);
}

static void TestFightsRightAway(const char *name) {
  if (!Start(name, false)) return;
  Arena *a = NewArena("", 2, 2, false, WAIT_FOR_FIGHT, 3000);
  a->addPauseMs = 50;   // the manager is surely waiting when robot 1 comes
  a->fixedAddPause = true;
  if (PlayAndCheck(name, &a, 1, false)) Result(name, false, true);
}

static void TestAtTheSameTime(const char *name) {
  if (!Start(name, false)) return;
  Arena *a = NewArena("", 16, 20, false, NORMAL, 4000);
  a->fixedFight = true;
  Outcome o = Play(&a, 1, true);
  bool ok = Ok(&a, 1, &o);
  if (ok && a->maxFightsNow < 2) {
    Result(name, false, false);
    printf("         there was never more than 1 fight at a time. With 16 robots, 8 fights can run at\n"
           "         the same time: does each fight run in a thread of its own, without holding a lock?\n");
  } else if (!ok) {
    Result(name, false, false);
    ShowWhy(&a, 1, &o);
  } else {
    Result(name, false, true);
  }
  if (o.finished && a->problem[0] == '\0') FreeArena(a);   // else its threads may still use it
}

static void TestAddDuringStop(const char *name) {
  if (!Start(name, false)) return;
  Arena *a = NewArena("", 10, 50, false, ADD_DURING_STOP, 5000);
  a->fixedFight = true;
  if (PlayAndCheck(name, &a, 1, false)) Result(name, false, true);
}

static void TestTwoAtOnce(const char *name) {
  if (!Start(name, false)) return;
  Arena *a[2] = {NewArena("competition 1: ", 50, 2, false, NORMAL, 6001),
                 NewArena("competition 2: ", 50, 2, false, NORMAL, 6002)};
  a[0]->addPauseMs = a[1]->addPauseMs = 1;
  if (PlayAndCheck(name, a, 2, false)) Result(name, false, true);
}

static void TestMany(const char *name, int competitions) {
  if (!Start(name, false)) return;
  Arena **all = calloc(competitions, sizeof(Arena *));
  int played = 0;
  bool ok = true;
  for (int i = 0; i < competitions && ok; i++) {
    all[i] = NewArena("", 1 + i % 20, 0, false, NORMAL, 7000 + i);
    played++;
    Outcome o = Play(&all[i], 1, false);
    if (!Ok(&all[i], 1, &o)) {
      ok = false;
      Result(name, false, false);
      printf("         competition %d of %d (%d robots):\n", i + 1, competitions, all[i]->n);
      ShowWhy(&all[i], 1, &o);
    }
  }
  if (ok) {
    Pause(30);   // does a fight start after CompetitionStop returned?
    for (int i = 0; i < competitions && ok; i++) {
      Outcome o = {.finished = true};
      if (!Ok(&all[i], 1, &o)) {
        ok = false;
        Result(name, false, false);
        printf("         competition %d of %d (%d robots):\n", i + 1, competitions, all[i]->n);
        ShowWhy(&all[i], 1, &o);
      }
    }
  }
  if (ok) {
    Result(name, false, true);
    for (int i = 0; i < played; i++) FreeArena(all[i]);
    free(all);
  }   // else its threads may still use them
}

int main(void) {
  printf("Competition\n");
  Test("5 robots", 5, 2, true);
  Test("1 robot: it wins without a fight", 1, 2, false);
  TestFightsRightAway("2 robots, 50 ms apart: they fight right away, before CompetitionStop");
  TestAtTheSameTime("16 robots: the fights run at the same time");
  TestAddedWhileFighting("100 robots, added while the others fight", 100);
  Test("1000 robots: the most there can be", 1000, 2, false);
  TestAddDuringStop("CompetitionAddParticipant while CompetitionStop waits: false");
  TestTwoAtOnce("two competitions at the same time");
  TestMany("200 small competitions in a row, and the fights don't sleep", 200);
  Test("no robots: CompetitionStop returns NULL", 0, 2, false);
  printf("%d / %d tests passed\n", passed, total);
  return 0;
}
