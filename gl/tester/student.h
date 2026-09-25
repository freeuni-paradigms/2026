#ifndef GL_TESTER_STUDENT_H_
#define GL_TESTER_STUDENT_H_

#include <stdbool.h>
#include <stdio.h>

#include "gl/containers/list.h"

typedef struct {
  char* id;
  List problems;
} Student;

void StudentInit(Student* s, char* id);
void StudentDispose(Student* s);

typedef List StudentList;

void StudentListInit(StudentList* list);
void StudentListAdd(StudentList* list, Student* student);
Student* StudentListGet(StudentList* list, int i);
void StudentListLogInfo(StudentList* list);
void StudentListLogResults(StudentList* list, bool log_individual_tests);
void StudentListToCsv(StudentList* list, FILE* out);
void StudentListDispose(StudentList* list);

void ListStudents(const char* students_dir, StudentList* list);

#endif // GL_TESTER_STUDENT_H_
