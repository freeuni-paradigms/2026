#ifndef GL_LUNIT_LUNIT_H_
#define GL_LUNIT_LUNIT_H_

#include <stdbool.h>

#include "gl/containers/list.h"
#include "gl/logging/logging.h"
#include "gl/lunit/asserts.h"
#include "gl/lunit/cmp.h"

#define ERROR_MAX_LEN 1000
#define NO_WEIGHT -1

typedef enum {
	      RUN = 0,
	      LIST = 1,
	      SCORE = 2
} LUnitMode;

typedef struct {
  LUnitMode mode;
  bool crash_on_failure;
  char* run_test;
  char* results_file;
} LUnitOpts;

const LUnitOpts* LUnitDefaults();
void LUnitOptsInit(LUnitOpts* opts, int argc, char* argv[]);

typedef struct {
  const char* name;
  bool success;
  bool memory;
  char error[ERROR_MAX_LEN];
  void* test_fn;
} Test;

typedef void(*TestFn)(Test*);  
void TestInit(Test* test, const char* name, TestFn test_fn);

typedef struct {
  const char* name;
  double weight;
  List tests;
} TestSuite;

void TestSuiteInit(TestSuite* suite, const char* name, double weight);
void TestSuiteDispose(TestSuite* suite);
void TestSuiteAddTest(TestSuite* suite, Test* test);

void RunTest(Test* test, const LUnitOpts* opts);

void ProcessTestSuites(int n, TestSuite* suites[], const LUnitOpts* opts);

typedef struct {
  int num_tests;
  int num_passed;
  double score;
} Score;

Score CalculateScore(int n, TestSuite* suites[]);

#define TEST(name) \
  void Test##name(Test* test)

#define TEST_SUITE_WITH_WEIGHT(name, weight) TestSuite name;	\
  TestSuiteInit(&name, #name, weight);

#define TEST_SUITE(name) TEST_SUITE_WITH_WEIGHT(name, NO_WEIGHT);

#define LOG_RESULTS(test, opts)						\
  if ((test)->success) {						\
    LOG_INFO("----- TEST %s: SUCCESS -----", (test)->name);				\
  } else if ((opts)->crash_on_failure) {				\
    LOG_FATAL("----- TEST %s: FAILURE -----", (test)->name);			\
  } else {								\
    LOG_ERROR("----- TEST %s: FAILURE -----", (test)->name);			\
  }

#define RUN_TEST(name, opts) {					\
    Test test;							\
    TestInit(&test, #name, Test##name);				\
    RunTest(&test, opts);					\
  }

#define ADD_TEST(suite, name) {				\
    Test test;						\
    TestInit(&test, #name, Test##name);			\
    TestSuiteAddTest(&suite, &test);			\
  }

#endif // GL_LUNIT_LUNIT_H_
