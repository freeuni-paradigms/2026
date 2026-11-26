#include "helper.h"

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_NUM_TEACHERS 10
#define MAX_NUM_STUDENTS 100

int numTeachers;
int numStudents;
pthread_mutex_t numStudentsLock;
sem_t studentFinished;
sem_t teacherFinished;

struct teacherInfo{
  int curStudentId;
  bool isFree;
  bool studentPassed;
  sem_t setRecorded;
  sem_t setEvaluated;
  pthread_mutex_t internalLock;
} teachers[MAX_NUM_TEACHERS];

void* Teacher(void* args) {
  int id = *(int*)args;
  // IMPLEMENT
  while (true) {
    sem_wait(&teachers[id].setRecorded);
    pthread_mutex_lock(&numStudentsLock);
    if(numStudents == 0) {
      pthread_mutex_unlock(&numStudentsLock);
      break;
    }
    pthread_mutex_unlock(&numStudentsLock);
    
    pthread_mutex_lock(&teachers[id].internalLock);
    teachers[id].studentPassed = TeacherEvaluateStudent(id, teachers[id].curStudentId);
    pthread_mutex_unlock(&teachers[id].internalLock);

    sem_post(&teachers[id].setEvaluated);
  }

  TeacherDone(id);
  sem_post(&teacherFinished);
  return NULL;
}

void* Student(void* args) {
  int id = *(int*)args;
  // IMPLEMENT
  bool passed = false;
  while (!passed) {
    StudentRecordSet(id);

    // find fre Teacher and ask for evaluation
    int teacherId = -1;
    for(int i = 0; i < numTeachers; i++) {
      pthread_mutex_lock(&teachers[i].internalLock);
      if(teachers[i].isFree) {
        // ask to evaluate
        teacherId = i;
        teachers[i].isFree = false;
        teachers[i].curStudentId = id;
        sem_post(&teachers[i].setRecorded);
        pthread_mutex_unlock(&teachers[i].internalLock);
        break;
      }
      pthread_mutex_unlock(&teachers[i].internalLock);
    }

    if(teacherId == -1) {
      continue;
    }

    sem_wait(&teachers[teacherId].setEvaluated);

    pthread_mutex_lock(&teachers[teacherId].internalLock);
    passed = teachers[teacherId].studentPassed;
    teachers[teacherId].isFree = true;
    pthread_mutex_unlock(&teachers[teacherId].internalLock);

    StudentWasEvaluatedByTeacher(id, teacherId, passed);
  }

  pthread_mutex_lock(&numStudentsLock);
  numStudents--;
  pthread_mutex_unlock(&numStudentsLock);
  StudentDone(id);
  sem_post(&studentFinished);
  return NULL;
}

void StartSchool(int num_students, int num_teachers) {
  // IMPLEMENT: Initialize semaphores and mutexes if needed.
  int teacher_id[MAX_NUM_TEACHERS];
  int student_id[MAX_NUM_STUDENTS];

  numTeachers = num_teachers;
  numStudents = num_students;
  pthread_mutex_init(&numStudentsLock, NULL);
  sem_init(&studentFinished, 0, 0);
  sem_init(&teacherFinished, 0, 0);

  for (int i = 0; i < num_teachers; ++i) {
    teacher_id[i] = i;
    teachers[i].isFree = true;
    sem_init(&teachers[i].setRecorded, 0, 0);
    sem_init(&teachers[i].setEvaluated, 0, 0);
    pthread_mutex_init(&teachers[i].internalLock, NULL);

    pthread_t tid;
    pthread_create(&tid, NULL, Teacher, &teacher_id[i]);
    pthread_detach(tid);
  }
  for (int i = 0; i < num_students; ++i) {
    student_id[i] = i;
    pthread_t tid;
    pthread_create(&tid, NULL, Student, &student_id[i]);
    pthread_detach(tid);
  }
  // IMPLEMENT: Wait for the all students and teachers to finish.
  for(int i = 0; i < num_students; i++) {
    sem_wait(&studentFinished);
  }
  
  for(int i = 0; i < num_teachers; i++) {
    sem_post(&teachers[i].setRecorded);
    sem_wait(&teacherFinished);
  }

  SchoolEnd();
}
