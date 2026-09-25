#include "gl/lunit/lunit.h"

#include <assert.h>

#define FLAG_CRASH_ON_FAILURE "--crash_on_failure"
#define FLAG_LIST_TESTS "--list_tests"
#define FLAG_RUN_TEST "--run_test="
#define FLAG_RESULTS_FILE "--results_file="

#define PENALTY_ON_MEMORY_FAILURE 0.15

const LUnitOpts* LUnitDefaults() {
  static LUnitOpts opts;
  opts.mode = RUN;
  opts.crash_on_failure = false;
  opts.run_test = NULL;
  opts.results_file = NULL;
  return &opts;
}

void LUnitOptsInit(LUnitOpts* opts, int argc, char* argv[]) {
  *opts = *LUnitDefaults();
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], FLAG_CRASH_ON_FAILURE) == 0) {
      opts->crash_on_failure = true;
    } else if (strcmp(argv[i], FLAG_LIST_TESTS) == 0) {
      opts->mode = LIST;
    } else if (strncmp(argv[i], FLAG_RUN_TEST, strlen(FLAG_RUN_TEST)) == 0) {
      opts->run_test = argv[i] + strlen(FLAG_RUN_TEST);
    } else if (strncmp(argv[i], FLAG_RESULTS_FILE,
		       strlen(FLAG_RESULTS_FILE)) == 0) {
      opts->mode = SCORE;
      opts->results_file = argv[i] + strlen(FLAG_RESULTS_FILE);
    }
  }
}

void TestInit(Test* test, const char* name, TestFn test_fn) {
  test->name = name;
  test->success = false;
  test->success = false;
  test->error[0] = '\0';
  test->test_fn = test_fn;
}

void RunTest(Test* test, const LUnitOpts* opts) {
  LOG_INFO("----- RUNNING TEST: %s ------", test->name);
  test->success = true;
  test->memory = true;
  ((TestFn)test->test_fn)(test);
  LOG_RESULTS(test, opts);
  printf("\n");
}

void TestSuiteInit(TestSuite* suite, const char* name, double weight) {
  suite->name = name;
  suite->weight = weight;
  ListInit(&suite->tests, sizeof(Test), /*free_fn=*/NULL);
}

void TestSuiteDispose(TestSuite* suite) {
  ListDispose(&suite->tests);
}

void TestSuiteList(TestSuite* suite) {
  for (int i = 0; i < suite->tests.size; ++i) {
    printf("%s\n", ((Test*)ListGet(&suite->tests, i))->name);
  }
}

void TestSuiteProcess(TestSuite* suite, const LUnitOpts* opts) {
  for (int i = 0; i < suite->tests.size; ++i) {
    Test* test = ListGet(&suite->tests, i);
    if (opts->run_test == NULL || strcmp(opts->run_test, test->name) == 0) {
      RunTest(test, opts);
    }
  }
}

void ProcessTestSuiteWeights(int n, TestSuite* suites[]) {
  double weight = 0;
  int num_no_weight = 0;
  for (int i = 0; i < n; ++i) {
    if (suites[i]->weight == NO_WEIGHT) {
      ++num_no_weight;
    } else {
      weight += suites[i]->weight;
    }
  }
  CHECK(weight <= 1);
  if (num_no_weight == 0) {
    return;
  }
  double weight_left = 1 - weight;
  for (int i = 0; i < n; ++i) {
    if (suites[i]->weight == NO_WEIGHT) {
      suites[i]->weight = weight_left / num_no_weight;
    }
  }
}

void TestSuitesReadResults(int n, TestSuite* suites[], FILE* inp) {
  char test_name[1000];
  int succeeded;
  int memory;
  while (fscanf(inp, "%s %d %d", test_name, &succeeded, &memory) != EOF) {
    bool found = false;
    for (int i = 0; i < n && !found; ++i) {
      for (int j = 0; j < suites[i]->tests.size && !found; ++j) {
	Test* test = ListGet(&suites[i]->tests, j);
	if (strcmp(test->name, test_name) == 0) {
	  found = true;
	  test->success = succeeded;
	  test->memory = memory;
	}
      }
    }
  }
}

void ProcessTestSuites(int n, TestSuite* suites[], const LUnitOpts* opts) {
  ProcessTestSuiteWeights(n, suites);
  // TODO(giolekva): better structure this
  if (opts->mode == LIST) {
    for (int i = 0; i < n; ++i) {
      TestSuiteList(suites[i]);
    }
    return;
  }
  if (opts->mode == SCORE) {
    FILE* inp = fopen(opts->results_file, "r");
    assert(inp != NULL);
    TestSuitesReadResults(n, suites, inp);
    fclose(inp);
    Score score = CalculateScore(n, suites);
    printf("%lf\n", score.score);
    return;
  }
  for (int i = 0; i < n; ++i) {
    TestSuiteProcess(suites[i], opts);
  }
  if (opts->run_test == NULL) {
    Score score = CalculateScore(n, suites);
    printf("%d / %d PASSED\n", score.num_passed, score.num_tests);
  }
}

Score CalculateScore(int n, TestSuite* suites[]) {
  Score score;
  score.score = 0;
  score.num_tests = 0;
  score.num_passed = 0;
  for (int i = 0; i < n; ++i) {
    TestSuite* suite = suites[i];
    if (suite->tests.size == 0) {
      continue;
    }
    int num_succeeded = 0;
    int num_memory_failed = 0;
    for (int j = 0; j < suite->tests.size; ++j) {
      Test* test = ListGet(&suite->tests, j);
      ++score.num_tests;
      if (test->success) {
	++num_succeeded;
        ++score.num_passed;
	num_memory_failed += !test->memory;
      }
    }
    double score_per_test = suite->weight / suite->tests.size;
    score.score += score_per_test *
      (num_succeeded - PENALTY_ON_MEMORY_FAILURE * num_memory_failed);
  }
  return score;
}

void TestSuiteAddTest(TestSuite* suite, Test* test) {
  ListAdd(&suite->tests, test);
}
