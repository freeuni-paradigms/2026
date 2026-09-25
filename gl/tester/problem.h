#ifndef GL_TESTER_PROBLEM_H_
#define GL_TESTER_PROBLEM_H_

#include <stdbool.h>

#include "gl/containers/list.h"

typedef struct {
  char* id;
  char* src_dir;
  char* cmd_make_lib;
  char* cmd_make_test;
  char* test_binary;
  bool check_memory;
  bool individual_tests;
  List files_to_preserve;
} ProblemInfo;

void ProblemInfoInit(ProblemInfo* p);
void ProblemInfoDispose(ProblemInfo* p);

typedef List ProblemSet;

void ProblemSetInit(ProblemSet* ps);
void ProblemSetDispose(ProblemSet* ps);
void ProblemSetAdd(ProblemSet* ps, ProblemInfo* problem);
ProblemInfo* ProblemSetGet(ProblemSet* ps, int index);

typedef struct {
  char* name;
  bool succeeded;
  bool memory;
  double score;
} TestResult;

void TestResultInit(TestResult* res, char* name);
void TestResultDispose(TestResult* res);

typedef struct {
  const char* id;
  bool solution_found;
  bool test_compiled;
  bool individual_tests;
  List tests;
  double score;
} ProblemResult;

// Given id must outlive the result object.
void ProblemResultInit(ProblemResult* res, const char* id);
void ProblemResultDispose(ProblemResult* res);

void ListProblemSet(const char* problems_dir, ProblemSet* ps);

#endif // GL_TESTER_PROBLEM_H_
