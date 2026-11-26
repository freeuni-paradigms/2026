#include "helper.h"

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_NUM_TEACHERS 10
#define MAX_NUM_STUDENTS 100

void* Teacher(void* args) {
  int id = *(int*)args;
  // IMPLEMENT
  while (true) {
  }
  return NULL;
}

void* Student(void* args) {
  int id = *(int*)args;
  // IMPLEMENT
  while (true) {
  }
  return NULL;
}

void StartSchool(int num_students, int num_teachers) {
  // IMPLEMENT: Initialize semaphores and mutexes if needed.
  int teacher_id[MAX_NUM_TEACHERS];
  int student_id[MAX_NUM_STUDENTS];
  for (int i = 0; i < num_teachers; ++i) {
    teacher_id[i] = i;
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
  SchoolEnd();
}
