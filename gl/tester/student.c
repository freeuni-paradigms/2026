#include "gl/tester/student.h"

#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "gl/logging/logging.h"
#include "gl/tester/problem.h"

static void GenericProblemResultDispose(void* pt) {
  ProblemResultDispose(pt);
}

void StudentInit(Student* s, char* id) {
  assert(id != NULL);
  s->id = strdup(id);
  ListInit(&s->problems, sizeof(ProblemResult), GenericProblemResultDispose);
}

void StudentDispose(Student* s) {
  free(s->id);
  ListDispose(&s->problems);
}

static void GenericStudentDispose(void* s) {
  StudentDispose(s);
}

void StudentListInit(StudentList* list) {
  assert(list != NULL);
  ListInit(list, sizeof(Student), GenericStudentDispose);
}

void StudentListAdd(StudentList* list, Student* student) {
  ListAdd(list, student);
}

Student* StudentListGet(StudentList* list, int i) {
  return ListGet(list, i);
}

void StudentListLogInfo(StudentList* list) {
  LOG_INFO("Found %d students:", list->size);
  for (int i = 0; i < list->size; ++i) {
    const Student* s = StudentListGet(list, i);
    LOG_INFO("* %s", s->id);
  }
}

void StudentListLogResults(StudentList* list, bool log_individual_tests) {
  LOG_INFO("*** Evaluation results ***");
  for (int i = 0; i < list->size; ++i) {
    Student* s = StudentListGet(list, i);
    double score = 0;
    for (int j = 0; j < s->problems.size; ++j) {
      score += ((ProblemResult*)ListGet(&s->problems, j))->score;
    }
    LOG_INFO("%s %lf", s->id, score);
    for (int j = 0; j < s->problems.size; ++j) {
      ProblemResult* result = ListGet(&s->problems, j);
      if (!result->solution_found) {
	LOG_INFO("  * %s: N/A", result->id);
	continue;
      }
      if (!result->test_compiled) {
	LOG_INFO("  * %s: NOT COMPILED", result->id);
	continue;
      }
      LOG_INFO("  * %s : %lf", result->id, result->score);
      if (!log_individual_tests) {
	continue;
      }
      for (int k = 0; k < result->tests.size; ++k) {
	const TestResult* test = ListGet(&result->tests, k);
	LOG_INFO("    - %s : TEST %d MEMORY %d ", test->name, test->succeeded,
		 test->memory);
      }      
    }
  }  
}

void StudentListToCsv(StudentList* list, FILE* out) {
  assert(out != NULL);
  for (int i = 0; i < list->size; ++i) {
    Student* s = StudentListGet(list, i);
    fprintf(out, "%s", s->id);
    for (int j = 0; j < s->problems.size; ++j) {
      ProblemResult* result = ListGet(&s->problems, j);
      if (!result->solution_found) {
	fprintf(out, ",N/A");
	continue;
      }
      if (!result->test_compiled) {
	fprintf(out, ",NOT COMPILED");
	continue;
      }
	  if (!result->individual_tests) {
		fprintf(out ,",%lf", result->score);
	  } else {
		for (int k = 0; k < result->tests.size; ++k) {
		  TestResult* test = ListGet(&result->tests, k);
		  fprintf(out ,",%lf", test->score);
		}
	  }
    }
    fprintf(out, "\n");          
  }  
}

void StudentListDispose(StudentList* list) {
  ListDispose(list);
}

void ListStudents(const char* students_dir, StudentList* list) {
  StudentListInit(list);
  DIR* dir = opendir(students_dir);
  if (dir == NULL) {
    LOG_FATAL("Could not find %s students directory.", students_dir);
  }
  while (1) {
    struct dirent* d = readdir(dir);
    if (d == NULL) {
      break;
    }
    if (d->d_name[0] == '.' || d->d_name[0] == '_') {
      continue;
    }
    // TODO(giolekva): skip any non directory files.
    Student student;
    StudentInit(&student, d->d_name);
    StudentListAdd(list, &student);
  }
  closedir(dir);
}
