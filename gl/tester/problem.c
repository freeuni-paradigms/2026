#include "gl/tester/problem.h"

#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "gl/logging/logging.h"

#define MAX_PATH 1000
#define MAX_CMD 1000

static void StrFree(void* str) {
  free(*(char**)str);
}

void ProblemInfoInit(ProblemInfo* p) {
  assert(p != NULL);
  ListInit(&p->files_to_preserve, sizeof(char*), StrFree);
  p->check_memory = true;
  p->individual_tests = false;
}

void ProblemInfoDispose(ProblemInfo* p) {
  assert(p != NULL);
  free(p->id);
  free(p->src_dir);
  free(p->cmd_make_lib);
  free(p->cmd_make_test);
  free(p->test_binary);
  ListDispose(&p->files_to_preserve);
}

static void GenericProblemInfoDispose(void* pt) {
  ProblemInfoDispose(pt);
}

void ProblemSetInit(ProblemSet* ps) {
  ListInit(ps, sizeof(ProblemInfo), GenericProblemInfoDispose);
}

void ProblemSetDispose(ProblemSet* ps) {
  ListDispose(ps);
}

void ProblemSetAdd(ProblemSet* ps, ProblemInfo* problem) {
  ListAdd(ps, problem);
}

ProblemInfo* ProblemSetGet(ProblemSet* ps, int index) {
  return ListGet(ps, index);
}

void TestResultInit(TestResult* res, char* name) {
  res->name = name;
  res->succeeded = false;
  res->memory = false;
}

void TestResultDispose(TestResult* res) {
  free(res->name);
}

void GenericTestResultDispose(void* pt) {
  TestResultDispose(pt);
}

void ProblemResultInit(ProblemResult* res, const char* id) {
  assert(res != NULL);
  res->id = id;
  res->solution_found = false;
  res->test_compiled = false;
  ListInit(&res->tests, sizeof(TestResult), GenericTestResultDispose);
  res->score = 0;
}

void ProblemResultDispose(ProblemResult* res) {
  ListDispose(&res->tests);
}

void ListProblemSet(const char* problems_dir, ProblemSet* problems) {
  ProblemSetInit(problems);  
  DIR* dir = opendir(problems_dir);
  if (dir == NULL) {
    LOG_FATAL("Could not find %s problems directory.", problems_dir);
  }
  while (1) {
    struct dirent* d = readdir(dir);
    if (d == NULL) {
      break;
    }
    const char* name = d->d_name;
    if (name[0] == '.' || name[0] == '_') {
      continue;
    }
    ProblemInfo p;
    ProblemInfoInit(&p);
    p.id = strdup(name);
    char path[MAX_PATH];
    sprintf(path, "%s/%s", problems_dir, name);
    p.src_dir = strdup(path);
    sprintf(path, "%s/%s/problem_config", problems_dir, name);
    FILE* config = fopen(path, "r");
    if (config == NULL) {
      continue;
    }
    char line[MAX_CMD];
    fgets(line, MAX_CMD, config);
    line[strlen(line) - 1] = '\0';
    p.cmd_make_lib = strdup(line);
    fgets(line, MAX_CMD, config);
    line[strlen(line) - 1] = '\0';
    p.cmd_make_test = strdup(line);
    fgets(line, MAX_CMD, config);
    line[strlen(line) - 1] = '\0';
    p.test_binary = strdup(line);
    while (fgets(path, MAX_PATH, config) != NULL) {
      path[strlen(path) - 1] = '\0';
	  if (0 == strcmp(path, "! memory=0")) {
		p.check_memory = false;
	  } else if (0 == strcmp(path, "! individual_tests=1")) {
		p.individual_tests = true;
	  } else {
		char* ch = strdup(path);
		ListAdd(&p.files_to_preserve, &ch);
	  }
    }
    fclose(config);
    ProblemSetAdd(problems, &p);
  }
  closedir(dir);
}
