#include "helper.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * The exam's helper functions, with two changes:
 *  - they sleep 0-2 milliseconds, not 0-4 seconds, so that a test takes a moment;
 *  - they check the rules of the problem, and write down the first rule that
 *    was broken. tests.c shows it.
 * Don't change this file: on the exam you may change only dj_school.c.
 */

#define MAX_STUDENTS 100
#define MAX_TEACHERS 10

enum StudentState {
  NEEDS_SET,      // at the start, or after a BAD evaluation: has to record a set
  RECORDING,      // in StudentRecordSet
  SET_READY,      // recorded: waits for a teacher
  EVALUATING,     // a teacher is in TeacherEvaluateStudent with its set
  EVALUATED,      // the teacher is done: the student has to report it
  REPORTING,      // in StudentWasEvaluatedByTeacher
  GOT_GOOD,       // reported GOOD: has to call StudentDone
  FINISHING,      // in StudentDone
  FINISHED
};

enum TeacherState { WORKING, T_FINISHING, T_FINISHED };

static pthread_mutex_t helperLock = PTHREAD_MUTEX_INITIALIZER;   // for everything below
static int numStudents, numTeachers, maxSleepMs;
static bool printing;
static pthread_t schoolThread;    // the thread that called StartSchool

static enum StudentState student[MAX_STUDENTS];
static int teacherOf[MAX_STUDENTS];       // the teacher who evaluates (evaluated) its last set
static bool result[MAX_STUDENTS];         // what that teacher said
static int sets[MAX_STUDENTS];            // how many sets it recorded
static pthread_t studentThread[MAX_STUDENTS];
static bool studentThreadKnown[MAX_STUDENTS];

static enum TeacherState teacher[MAX_TEACHERS];
static int evaluating[MAX_TEACHERS];      // whose set it evaluates right now, -1: nobody's
static pthread_t teacherThread[MAX_TEACHERS];
static bool teacherThreadKnown[MAX_TEACHERS];

static int studentsFinished, teachersFinished, schoolEnded, calls;
static unsigned seed;

static char problem[300];                 // the first rule that was broken, "" if none
#define LOG 8
static char events[LOG][100];             // the last LOG things that happened
static int numEvents;
static char eventsBeforeProblem[LOG][100];
static int numEventsBeforeProblem;

/* ---- under helperLock ---- */

static int Random(int n) {
  seed = seed * 1103515245 + 12345;
  return (seed >> 16) % n;
}

static void Problem(const char *format, ...) {
  if (problem[0] != '\0') return;   // only the first one
  va_list ap;
  va_start(ap, format);
  vsnprintf(problem, sizeof(problem), format, ap);
  va_end(ap);
  numEventsBeforeProblem = numEvents < LOG ? numEvents : LOG;
  for (int i = 0; i < numEventsBeforeProblem; i++) {
    strcpy(eventsBeforeProblem[i], events[(numEvents - numEventsBeforeProblem + i) % LOG]);
  }
}

static void Event(const char *format, ...) {
  char *line = events[numEvents % LOG];
  va_list ap;
  va_start(ap, format);
  vsnprintf(line, 100, format, ap);
  va_end(ap);
  numEvents++;
  if (printing) printf("         %s\n", line);
}

/* where student s is, in words */
static void StudentWhere(int s, char *buf) {
  switch (student[s]) {
    case NEEDS_SET:
      strcpy(buf, sets[s] == 0 ? "hasn't recorded a set yet" : "got BAD and hasn't recorded a new set yet");
      break;
    case RECORDING: strcpy(buf, "is recording a set"); break;
    case SET_READY: strcpy(buf, "has a set ready, and no teacher has taken it"); break;
    case EVALUATING: sprintf(buf, "is being evaluated by teacher %d", teacherOf[s]); break;
    case EVALUATED:
      sprintf(buf, "was evaluated by teacher %d and hasn't called StudentWasEvaluatedByTeacher", teacherOf[s]);
      break;
    case REPORTING: strcpy(buf, "is in StudentWasEvaluatedByTeacher"); break;
    case GOT_GOOD: strcpy(buf, "got GOOD and hasn't called StudentDone"); break;
    case FINISHING: strcpy(buf, "is in StudentDone"); break;
    case FINISHED: strcpy(buf, "is done"); break;
  }
}

static void TeacherWhere(int t, char *buf) {
  if (teacher[t] == T_FINISHED) strcpy(buf, "is done");
  else if (teacher[t] == T_FINISHING) strcpy(buf, "is in TeacherDone");
  else if (evaluating[t] >= 0) sprintf(buf, "is evaluating student %d", evaluating[t]);
  else strcpy(buf, "hasn't called TeacherDone");
}

/* every helper call starts with it */
static void Enter(const char *function) {
  calls++;
  if (schoolEnded > 0) Problem("%s was called after SchoolEnd", function);
}

/* A thread that's done may exit, and a new thread can get its id. So the
   threads of students and teachers that are done aren't compared. */
static void CheckThread(bool isStudent, int id, const char *call) {
  pthread_t self = pthread_self();
  for (int s = 0; s < numStudents; s++) {
    if (isStudent && s == id) continue;
    if (studentThreadKnown[s] && student[s] != FINISHED && pthread_equal(self, studentThread[s])) {
      Problem("%s was called by student %d's thread", call, s);
    }
  }
  for (int t = 0; t < numTeachers; t++) {
    if (!isStudent && t == id) continue;
    if (teacherThreadKnown[t] && teacher[t] != T_FINISHED && pthread_equal(self, teacherThread[t])) {
      Problem("%s was called by teacher %d's thread", call, t);
    }
  }
  if (pthread_equal(self, schoolThread)) Problem("%s was called by StartSchool's thread", call);
  pthread_t *mine = isStudent ? &studentThread[id] : &teacherThread[id];
  bool *known = isStudent ? &studentThreadKnown[id] : &teacherThreadKnown[id];
  if (!*known) {
    *mine = self;
    *known = true;
  } else if (!pthread_equal(*mine, self)) {
    Problem("%s %d's functions were called by two different threads", isStudent ? "student" : "teacher", id);
  }
}

/* is there such a student? */
static bool StudentExists(int s, const char *call) {
  if (s < 0 || s >= numStudents) {
    Problem("%s: there is no student %d", call, s);
    return false;
  }
  return true;
}

/* is there such a student, and is it its thread that calls? */
static bool StudentOk(int s, const char *call) {
  if (!StudentExists(s, call)) return false;
  CheckThread(true, s, call);
  return true;
}

static bool TeacherOk(int t, const char *call) {
  if (t < 0 || t >= numTeachers) {
    Problem("%s: there is no teacher %d", call, t);
    return false;
  }
  CheckThread(false, t, call);
  return true;
}

/* sleeps a random 0..maxSleepMs milliseconds, without the lock */
static void Sleep(void) {
  int ms = Random(maxSleepMs + 1);
  pthread_mutex_unlock(&helperLock);
  if (ms > 0) {
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000000L};
    nanosleep(&ts, NULL);
  }
  pthread_mutex_lock(&helperLock);
}

/* ---- the exam's functions ---- */

bool TeacherEvaluateStudent(int teacher_id, int student_id) {
  pthread_mutex_lock(&helperLock);
  Enter("TeacherEvaluateStudent");
  char call[60], where[100];
  sprintf(call, "TeacherEvaluateStudent(%d, %d)", teacher_id, student_id);
  bool ok = TeacherOk(teacher_id, call) && StudentExists(student_id, call);
  if (ok) {
    if (teacher[teacher_id] != WORKING) {
      TeacherWhere(teacher_id, where);
      Problem("%s: teacher %d %s", call, teacher_id, where);
    } else if (evaluating[teacher_id] >= 0) {
      Problem("%s: teacher %d is already evaluating student %d", call, teacher_id, evaluating[teacher_id]);
    } else if (student[student_id] != SET_READY) {
      StudentWhere(student_id, where);
      Problem("%s: student %d %s", call, student_id, where);
    }
    student[student_id] = EVALUATING;
    teacherOf[student_id] = teacher_id;
    evaluating[teacher_id] = student_id;
  }
  Sleep();
  bool good = Random(10) > 6;
  if (ok) {
    result[student_id] = good;
    student[student_id] = EVALUATED;
    evaluating[teacher_id] = -1;
  }
  Event("Teacher %d is evaluating student %d with result %s", teacher_id, student_id, good ? "GOOD" : "BAD");
  pthread_mutex_unlock(&helperLock);
  return good;
}

void TeacherDone(int teacher_id) {
  pthread_mutex_lock(&helperLock);
  Enter("TeacherDone");
  char call[40], where[100];
  sprintf(call, "TeacherDone(%d)", teacher_id);
  bool ok = TeacherOk(teacher_id, call);
  if (ok) {
    if (teacher[teacher_id] != WORKING) {
      TeacherWhere(teacher_id, where);
      Problem("%s: teacher %d %s", call, teacher_id, where);
    } else if (studentsFinished < numStudents) {
      int s = 0;
      while (student[s] == FINISHED) s++;
      StudentWhere(s, where);
      Problem("%s before every student was done: student %d %s", call, s, where);
    }
    teacher[teacher_id] = T_FINISHING;
  }
  Sleep();
  if (ok) {
    teacher[teacher_id] = T_FINISHED;
    teachersFinished++;
  }
  Event("Teacher %d: DONE", teacher_id);
  pthread_mutex_unlock(&helperLock);
}

void StudentRecordSet(int student_id) {
  pthread_mutex_lock(&helperLock);
  Enter("StudentRecordSet");
  char call[40], where[100];
  sprintf(call, "StudentRecordSet(%d)", student_id);
  bool ok = StudentOk(student_id, call);
  if (ok) {
    if (student[student_id] != NEEDS_SET) {
      StudentWhere(student_id, where);
      Problem("%s: student %d %s", call, student_id, where);
    }
    student[student_id] = RECORDING;
    sets[student_id]++;
  }
  Sleep();
  if (ok) student[student_id] = SET_READY;
  Event("Student %d recorded new set", student_id);
  pthread_mutex_unlock(&helperLock);
}

void StudentDone(int student_id) {
  pthread_mutex_lock(&helperLock);
  Enter("StudentDone");
  char call[40], where[100];
  sprintf(call, "StudentDone(%d)", student_id);
  bool ok = StudentOk(student_id, call);
  if (ok) {
    if (student[student_id] != GOT_GOOD) {
      StudentWhere(student_id, where);
      Problem("%s: student %d %s", call, student_id, where);
    }
    student[student_id] = FINISHING;
  }
  Sleep();
  if (ok) {
    student[student_id] = FINISHED;
    studentsFinished++;
  }
  Event("Student %d: DONE", student_id);
  pthread_mutex_unlock(&helperLock);
}

void StudentWasEvaluatedByTeacher(int student_id, int teacher_id, bool set_was_good) {
  pthread_mutex_lock(&helperLock);
  Enter("StudentWasEvaluatedByTeacher");
  char call[80], where[100];
  sprintf(call, "StudentWasEvaluatedByTeacher(%d, %d, %s)", student_id, teacher_id, set_was_good ? "true" : "false");
  bool ok = StudentOk(student_id, call);
  if (ok) {
    if (student[student_id] != EVALUATED) {
      StudentWhere(student_id, where);
      Problem("%s: student %d %s", call, student_id, where);
    } else if (teacherOf[student_id] != teacher_id) {
      Problem("%s: but teacher %d evaluated it", call, teacherOf[student_id]);
    } else if (result[student_id] != set_was_good) {
      Problem("%s: but the teacher said %s", call, result[student_id] ? "GOOD" : "BAD");
    }
    student[student_id] = REPORTING;
  }
  Sleep();
  if (ok) student[student_id] = result[student_id] ? GOT_GOOD : NEEDS_SET;
  Event("Student %d got evaluation result from teacher %d: %s", student_id, teacher_id, set_was_good ? "GOOD" : "BAD");
  pthread_mutex_unlock(&helperLock);
}

void SchoolEnd() {
  pthread_mutex_lock(&helperLock);
  Enter("SchoolEnd");
  char where[100];
  if (!pthread_equal(pthread_self(), schoolThread)) {
    Problem("SchoolEnd was called by another thread, not by StartSchool");
  } else if (teachersFinished < numTeachers) {
    int t = 0;
    while (teacher[t] == T_FINISHED) t++;
    TeacherWhere(t, where);
    Problem("SchoolEnd before every teacher was done: teacher %d %s", t, where);
  }
  schoolEnded++;
  Event("School finished.");
  pthread_mutex_unlock(&helperLock);
}

/* ---- for tests.c ---- */

/* before each StartSchool: forget the last school */
void CheckStart(int num_students, int num_teachers, int max_sleep_ms, bool print, pthread_t school) {
  pthread_mutex_lock(&helperLock);
  numStudents = num_students;
  numTeachers = num_teachers;
  maxSleepMs = max_sleep_ms;
  printing = print;
  schoolThread = school;
  for (int s = 0; s < MAX_STUDENTS; s++) {
    student[s] = NEEDS_SET;
    teacherOf[s] = -1;
    sets[s] = 0;
    studentThreadKnown[s] = false;
  }
  for (int t = 0; t < MAX_TEACHERS; t++) {
    teacher[t] = WORKING;
    evaluating[t] = -1;
    teacherThreadKnown[t] = false;
  }
  studentsFinished = teachersFinished = schoolEnded = calls = 0;
  numEvents = 0;
  problem[0] = '\0';
  seed = 12345 + num_students * 100 + num_teachers;
  pthread_mutex_unlock(&helperLock);
}

/* the first rule that was broken: "" if none (copied into buf) */
void CheckProblem(char *buf) {
  pthread_mutex_lock(&helperLock);
  strcpy(buf, problem);
  pthread_mutex_unlock(&helperLock);
}

/* did every student and every teacher finish? */
bool CheckThreadsDone(void) {
  pthread_mutex_lock(&helperLock);
  bool done = studentsFinished == numStudents && teachersFinished == numTeachers;
  pthread_mutex_unlock(&helperLock);
  return done;
}

bool CheckSchoolEnded(void) {
  pthread_mutex_lock(&helperLock);
  bool ended = schoolEnded > 0;
  pthread_mutex_unlock(&helperLock);
  return ended;
}

/* how many helper calls so far: if it doesn't change, nobody is doing anything */
int CheckCalls(void) {
  pthread_mutex_lock(&helperLock);
  int c = calls;
  pthread_mutex_unlock(&helperLock);
  return c;
}

/* prints the students and teachers that aren't done, and where they are */
void CheckShowNotDone(void) {
  pthread_mutex_lock(&helperLock);
  char where[100];
  int shown = 0, notShown = 0;
  for (int s = 0; s < numStudents; s++) {
    if (student[s] == FINISHED) continue;
    if (shown == 6) {
      notShown++;
      continue;
    }
    StudentWhere(s, where);
    printf("           student %d %s\n", s, where);
    shown++;
  }
  for (int t = 0; t < numTeachers; t++) {
    if (teacher[t] == T_FINISHED) continue;
    if (shown == 9) {
      notShown++;
      continue;
    }
    TeacherWhere(t, where);
    printf("           teacher %d %s\n", t, where);
    shown++;
  }
  if (notShown > 0) printf("           and %d more\n", notShown);
  if (schoolEnded == 0) printf("           SchoolEnd wasn't called\n");
  pthread_mutex_unlock(&helperLock);
}

/* prints what happened just before the first broken rule */
void CheckShowEventsBeforeProblem(void) {
  pthread_mutex_lock(&helperLock);
  if (numEventsBeforeProblem > 0) printf("         what happened just before:\n");
  for (int i = 0; i < numEventsBeforeProblem; i++) printf("           %s\n", eventsBeforeProblem[i]);
  pthread_mutex_unlock(&helperLock);
}
