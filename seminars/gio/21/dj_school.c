#include "helper.h"

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

/*
 * The helper functions (helper.h). Each one sleeps a moment, then prints what happened.
 *
 *   StudentRecordSet(student_id)                     the student records a new set
 *   TeacherEvaluateStudent(teacher_id, student_id)   the teacher listens to it: true = GOOD
 *   StudentWasEvaluatedByTeacher(student_id, teacher_id, good)
 *                                                    the student says what it got
 *   StudentDone(student_id)                          the student got GOOD: it's done
 *   TeacherDone(teacher_id)                          the teacher is done (every student is done)
 *   SchoolEnd()                                      StartSchool calls it at the very end
 *
 *   sem_t s;              sem_init(&s, 0, value)   sem_wait(&s)   sem_post(&s)   sem_destroy(&s)
 *   pthread_mutex_t m;    pthread_mutex_init(&m, NULL)   pthread_mutex_lock(&m)   ..._unlock(&m)
 *
 * No pthread_join and no pthread_exit. Change only this file. Globals are allowed.
 */


/*
 *  Every wait until "somebody does something" is a semaphore that the somebody posts
 *  
 *  a student waits for a free Teacher        freeTeachers (a teacher posts it when it's free again)
 *  a teacher waits for student's record      request[t]  (the student who took teacher t)
 *  a student waits for the evaluation        answered[s] (the teacher)
 *  StartSchool waits for students            studentDone (each student, once)
 *  a teacher waits for go home               request[t]  (StartSchool after every student is done)
 *  StartSchool waits for the teachers        teacherDone (each teacher, once)
 *
*/


#define MAX_NUM_TEACHERS 10
#define MAX_NUM_STUDENTS 100


sem_t freeTeachers;
pthread_mutex_t lock; //for busy
bool busy[MAX_NUM_TEACHERS]; // a student took this teacher
int studentOf[MAX_NUM_TEACHERS]; // whose set the teacher evaluates 
sem_t request[MAX_NUM_TEACHERS]; // student -> teacher: "here is my set"
bool good[MAX_NUM_STUDENTS]; // what the teacher said 
sem_t answered[MAX_NUM_STUDENTS]; // teacher -> student "evaluated"
bool schoolOver;
sem_t studentDone; // student -> StartSchool
sem_t teacherDone; // teacher -> StartSchool




void* Teacher(void* args) {
  int id = *(int*)args;
  while (true) {
    sem_wait(&request[id]); //students set, or StartSchool: "go home"
    if(schoolOver) break;
    int student = studentOf[id];
    good[student] = TeacherEvaluateStudent(id, student);
    sem_post(&answered[student]);
    pthread_mutex_lock(&lock);
    busy[id] = false;
    pthread_mutex_unlock(&lock);
    sem_post(&freeTeachers);
  }
  TeacherDone(id);
  sem_post(&teacherDone);
  return NULL;
}

void* Student(void* args) {
  int id = *(int*)args;
  while (true) {
    StudentRecordSet(id);
    sem_wait(&freeTeachers);
    pthread_mutex_lock(&lock);
    int teacher = 0;
    while(busy[teacher]) teacher++;
    busy[teacher] = true;
    pthread_mutex_unlock(&lock);
    studentOf[teacher] = id;
    sem_post(&request[teacher]);
    sem_wait(&answered[id]);
    StudentWasEvaluatedByTeacher(id, teacher, good[id]);
    if(good[id]) break;
  }
  StudentDone(id);
  sem_post(&studentDone);
  return NULL;
}

void StartSchool(int num_students, int num_teachers) {
  
  sem_init(&freeTeachers, 0, num_teachers);
  pthread_mutex_init(&lock, NULL);
  for(int i = 0; i < num_teachers; i++){
    busy[i] = false;
    sem_init(&request[i], 0, 0);
  }

  for(int i = 0; i < num_students; i++){
    sem_init(&answered[i], 0, 0);
  }
  schoolOver = false;
  sem_init(&studentDone, 0, 0);
  sem_init(&teacherDone, 0, 0);

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
  for(int i = 0; i < num_students; i++){
    sem_wait(&studentDone);
  }
  schoolOver = true;
  for(int i = 0; i < num_teachers; i++){
    sem_post(&request[i]); // wake every teacher up
  }
  for(int i = 0; i < num_teachers; i++){
    sem_wait(&teacherDone);
  }
  SchoolEnd();
    
  sem_destroy(&freeTeachers);
  pthread_mutex_destroy(&lock);
  for(int i = 0; i < num_teachers; i++){
    sem_destroy(&request[i]);
  }

  for(int i = 0; i < num_students; i++){
    sem_destroy(&answered[i]);
  }

  sem_destroy(&studentDone);
  sem_destroy(&teacherDone);


}
