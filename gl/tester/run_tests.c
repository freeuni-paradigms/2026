#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#include "gl/logging/logging.h"
#include "gl/tester/student.h"
#include "gl/tester/problem.h"
#include "gl/threads/pool.h"

#define MAX_PATH 1000
#define MAX_CMD 1000

#define MEM_CHECK_FLAGS "CFLAGS='-fsanitize=address -g' LDFLAGS='-fsanitize=address -g'"

#define STUDENTS_DIR "--students_dir="
#define PROBLEMS_DIR "--problems_dir="
#define MODE_CLASS "--class"
#define MODE_STUDENT "--student="
// TODO(giolekva): working
#define WORKING_DIR "--results_dir="

typedef enum {
	      STUDENT = 0,
	      CLASS = 1
} TesterMode;

typedef struct {
  TesterMode mode;
  char* student_name;
  char* students_dir;
  char* problems_dir;
  char* working_dir;
} TesterOpts;

#define STR_EQ(str, substr) (strcmp(str, substr) == 0)
#define STR_STARTS_WITH(str, substr) (strncmp(str, substr, strlen(substr)) == 0)

void TesterOptsInit(TesterOpts* opts, int argc, char* argv[]) {
  opts->mode = STUDENT;
  opts->students_dir = NULL;
  opts->problems_dir = NULL;
  opts->working_dir = NULL;
  bool mode_class = false;
  bool mode_student = false;
  for (int i = 1; i < argc; ++i) {
    if (STR_STARTS_WITH(argv[i], STUDENTS_DIR)) {
      opts->students_dir = argv[i] + strlen(STUDENTS_DIR);
    } else if (STR_STARTS_WITH(argv[i], PROBLEMS_DIR)) {
      opts->problems_dir = argv[i] + strlen(PROBLEMS_DIR);
    } else if(STR_STARTS_WITH(argv[i], WORKING_DIR)) {
      opts->working_dir = malloc(strlen(argv[i]) - strlen(WORKING_DIR) + 14);
      assert(opts->working_dir != NULL);
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      sprintf(opts->working_dir, "%s/%.2d%.2d%.2d%.2d%.2d%.2d",
	      argv[i] + strlen(WORKING_DIR), tm.tm_mday, tm.tm_mon + 1,
	      tm.tm_year - 100, tm.tm_hour, tm.tm_min, tm.tm_sec);
      CHECK(mkdir(opts->working_dir, 0777) == 0);
    } else if (STR_EQ(argv[i], MODE_CLASS)) {
      mode_class = true;
    } else if (STR_STARTS_WITH(argv[i], MODE_STUDENT)) {
      mode_student = true;
      opts->student_name = argv[i] + strlen(MODE_STUDENT);
    }
  }
  CHECK(opts->students_dir);
  CHECK(opts->problems_dir);
  CHECK(!(mode_class && mode_student));
  if (mode_class) {
    opts->mode = CLASS;
  }
}

void TesterOptsDispose(TesterOpts* opts) {
  if (opts->working_dir != NULL) {
    free(opts->working_dir);
  }
}

int ExecCmd(char* name, char* cmd, List* logs) {
  LOG_INFO("%s: %s", name, cmd);
  FILE* out = popen(cmd, "r");
  char buf[1024];
  while (fgets(buf, sizeof(buf), out) != NULL) {
    if (logs != NULL) {
      for (int i = 0; i < logs->size; ++i) {
	FILE** log = ListGet(logs, i);
	fputs(buf, *log);
      }
    }
  }
  /* while ((c = getc(out)) != EOF) { */
  /*   count++; */
  /*   if (count == 1000) { */
  /*     return 1; */
  /*   } */
  /*   if (logs != NULL) { */
  /*     for (int i = 0; i < logs->size; ++i) { */
  /* 	FILE** log = ListGet(logs, i); */
  /* 	putc(c, *log); */
  /*     } */
  /*   } */
  /* } */
  int status = pclose(out);
  // LOG_INFO("DONE %d %s: %s", status, name, cmd);
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return 1;
}

int ExecCmdInWorkingDir(char* name, char* cmd, List* logs,
			const char* working_dir) {
  char new_cmd[10000];
  sprintf(new_cmd, "cd %s && %s", working_dir, cmd);
  return ExecCmd(name, new_cmd, logs);
}

void PrepareWorkingDir(Student* student, ProblemInfo* problem,
		       const char* student_dir, const char* working_dir) {
  List logs;
  ListInit(&logs, sizeof(FILE**), /*free_fn=*/NULL);
  ListAdd(&logs, &stdout);
  char mkdir_cmd[MAX_CMD];
  sprintf(mkdir_cmd, "mkdir -p %s", working_dir);
  CHECK(!ExecCmd("Creating working directory", mkdir_cmd, &logs));
  sprintf(mkdir_cmd, "mkdir -p %s/logs", working_dir);
  CHECK(!ExecCmd("Creating test_out directory", mkdir_cmd, &logs));
  sprintf(mkdir_cmd, "mkdir -p %s/test_out", working_dir);
  CHECK(!ExecCmd("Creating mem_out directory", mkdir_cmd, &logs));
  sprintf(mkdir_cmd, "mkdir -p %s/mem_out", working_dir);
  CHECK(!ExecCmd("Creating logs directory", mkdir_cmd, &logs));
  char cmd[MAX_CMD];
  sprintf(cmd, "cp -R %s/%s/* %s", student_dir, problem->id,
	  working_dir);
  ExecCmd("Copying student files", cmd, &logs);
  for (int i = 0; i < problem->files_to_preserve.size; ++i) {
    char* f = *(char**)ListGet(&problem->files_to_preserve, i);
    bool overwrite = (f[0] == '+');
    f += 2;
    char* pos = strrchr(f, '/');
    if (pos != NULL) {
      char* mkd = strndup(f, pos - f);
      sprintf(cmd, "mkdir -p %s/%s", working_dir, mkd);
      CHECK(!ExecCmd("Creating directory", cmd, &logs));
      free(mkd);
    }
    if (overwrite) {
      sprintf(cmd, "cp -f %s/%s %s/%s", problem->src_dir, f, working_dir, f);
    } else {
      // Since student files are copied into working directory first they will
      // have newer modification date then files in problems directory.
      sprintf(cmd, "cp -u %s/%s %s/%s", problem->src_dir, f, working_dir, f);
    }
    CHECK(!ExecCmd("Preserving original file", cmd, &logs));
  }
  ListDispose(&logs);
}

bool DetermineListOfTestsToRun(ProblemInfo* problem, const char* working_dir, List* test_names) {
  char buf[10000];
  FILE* fd = fmemopen(buf, 10000, "w");
  List log_streams;
  ListInit(&log_streams, sizeof(FILE**), /*free_fn=*/NULL);
  ListAdd(&log_streams, &fd);
  char cmd[MAX_CMD];
  sprintf(cmd, "%s --list_tests", problem->test_binary);
  if (ExecCmdInWorkingDir("Determining list of tests to run", cmd, &log_streams, working_dir)) {
    fclose(fd);
    ListDispose(&log_streams);
    return false;
   }
  fclose(fd);
  ListDispose(&log_streams);
  printf("%s\n+++++++++++++++++\n", buf);
  for (int i = 0; buf[i] != '\0'; ) {
    int j = i;
    while (buf[j] != '\n' && buf[j] != '\0') {
      ++j;
    }
    char* name = strndup(buf + i, j - i);
    ListAdd(test_names, &name);
    i = j + 1;
  }
  return true;
}

void CalculateScore(ProblemInfo* problem, ProblemResult* result,
		    const char* working_dir) {
  #define RESULTS_FILE "../logs/results"
  char results_file[1000];
  sprintf(results_file, "%s/%s", working_dir, RESULTS_FILE);
  FILE* results_fd = fopen(results_file, "w");
  assert(results_fd != NULL);
  for (int i = 0; i < result->tests.size; ++i) {
    TestResult* test = ListGet(&result->tests, i);
    fprintf(results_fd, "%s %d %d\n", test->name, test->succeeded,
	    test->memory);
  }
  fclose(results_fd);
  char buf[10000];
  FILE* fd = fmemopen(buf, 10000, "w");
  List log_streams;
  ListInit(&log_streams, sizeof(FILE**), /*free_fn=*/NULL);
  ListAdd(&log_streams, &fd);
  char cmd[MAX_CMD];
  sprintf(cmd, "%s --results_file=%s", problem->test_binary, RESULTS_FILE);
  CHECK(!ExecCmdInWorkingDir("Calculating score", cmd, &log_streams, working_dir));
  fclose(fd);
  ListDispose(&log_streams);
  if (!problem->individual_tests) {
	sscanf(buf, "%lf", &result->score);
  } else {
	result->score = 0;
	int n = 0;
	for (int i = 0; i < result->tests.size; ++i) {
	  TestResult* test = ListGet(&result->tests, i);
	  int k;
	  sscanf(buf + n, "%lf%n", &test->score, &k);
	  n += k + 1;
	  result->score += test->score;
	}
  }
}

void StrFree(void* pt) {
  free(*(char**)pt);
}

bool SolutionExists(Student* student, ProblemInfo* problem,
		    const char* student_dir) {
  char path[MAX_PATH];
  sprintf(path, "%s/%s", student_dir, problem->id);
  DIR* d = opendir(path);
  if (d != NULL) {
    closedir(d);
    return true;
  }
  return false;
}

void EvaluateStudentOnProblem(Student* student, ProblemInfo* problem,
			      const char* working_dir, ProblemResult* result) {
  FILE* logs_fd = NULL;
  List log_streams;
  ListInit(&log_streams, sizeof(FILE**), /*free_fn=*/NULL);
  char logs_file[MAX_PATH];
  sprintf(logs_file, "%s/logs/tests", working_dir);
  logs_fd = fopen(logs_file, "a");
  ListAdd(&log_streams, &logs_fd);
  // ListAdd(&log_streams, &stdout);
  char test_dir[1000];
  sprintf(test_dir, "%s/test_out", working_dir);
  char mem_dir[1000];
  sprintf(mem_dir, "%s/mem_out", working_dir);
  char make[1000];
  sprintf(make, "OUT_DIR=%s %s", test_dir, problem->cmd_make_lib);  
  if (ExecCmdInWorkingDir("Building student solution", make,
			  &log_streams, working_dir)) {
    goto ret;
  }
  sprintf(make, "OUT_DIR=%s %s", test_dir, problem->cmd_make_test);  
  if (ExecCmdInWorkingDir("Building test", make, &log_streams,
			  working_dir)) {
    goto ret;
  }
  sprintf(make, "OUT_DIR=%s %s %s", mem_dir, MEM_CHECK_FLAGS,
	  problem->cmd_make_lib);
  if (ExecCmdInWorkingDir("Building student solution for memory check", make,
			  &log_streams, working_dir)) {
    goto ret;
  }
  sprintf(make, "OUT_DIR=%s %s %s", mem_dir, MEM_CHECK_FLAGS,
	  problem->cmd_make_test);
  if (ExecCmdInWorkingDir("Building test for memory check", make, &log_streams,
			  working_dir)) {
    goto ret;
  }
  result->test_compiled = true;
  List test_names;
  ListInit(&test_names, sizeof(char*), StrFree);
  CHECK(DetermineListOfTestsToRun(problem, test_dir, &test_names));
  for (int j = 0; j < test_names.size; ++j) {
    char* name = *(char**)ListGet(&test_names, j);
    TestResult res;
    TestResultInit(&res, strdup(name));
    char desc[MAX_CMD];
    sprintf(desc, "Running test %s", name);
    char cmd[MAX_CMD];
    sprintf(cmd, "timeout 10s %s --run_test=%s --crash_on_failure 2>&1", problem->test_binary, name);
    res.succeeded = !ExecCmdInWorkingDir(desc, cmd, &log_streams, test_dir);
	if (!problem->check_memory) {
	  LOG_INFO("Skipping memory check");
	  res.memory = res.succeeded;
	} else if (res.succeeded) {
      sprintf(desc, "Checking test %s on memory", name);
      sprintf(cmd, "ASAN_OPTIONS=log_path=%s/logs/mem_%s timeout 10s %s --run_test=%s --crash_on_failure",
	      working_dir, name, problem->test_binary, name);
      res.memory = !ExecCmdInWorkingDir(desc, cmd, &log_streams, mem_dir);
    }
    ListAdd(&result->tests, &res);
  }
  ListDispose(&test_names);
  CalculateScore(problem, result, test_dir);
 ret:
  if (logs_fd != NULL) {
    fclose(logs_fd);
  }
  ListDispose(&log_streams);
}

void OutputResultAsCsv(StudentList* students, TesterOpts* opts) {
  if (opts->working_dir == NULL) {
    return;
  }
  char path[MAX_PATH];
  sprintf(path, "%s/results.csv", opts->working_dir);
  FILE* out = fopen(path, "w");
  StudentListToCsv(students, out);
  fclose(out);
  LOG_INFO("--- RESULTS: %s", path);
}

typedef struct {
  TesterOpts* opts;
  Student* student;
  ProblemInfo* problem;
  ProblemResult* result;
} Args;

void Eval(void* pt) {
  List log_streams;
  ListInit(&log_streams, sizeof(FILE**), /*free_fn=*/NULL);
  ListAdd(&log_streams, &stdout);
  Args* args = pt;
  char student_dir[MAX_PATH];
  sprintf(student_dir, "%s/%s", args->opts->students_dir, args->student->id);
  if (SolutionExists(args->student, args->problem, student_dir)) {
    args->result->solution_found = true;
    LOG_INFO("*** Evaluating student %s on problem %s", args->student->id,
	     args->problem->id);
    char working_dir[MAX_PATH];
    sprintf(working_dir, "%s/%s/%s", args->opts->working_dir, args->student->id,
	    args->problem->id);
    PrepareWorkingDir(args->student, args->problem, student_dir, working_dir);
    EvaluateStudentOnProblem(args->student, args->problem, working_dir,
			     args->result);
    LOG_INFO("### Done evaluating student %s on problem %s", args->student->id,
	     args->problem->id);
  } else  {
    LOG_INFO("### Student %s did not provide solution on problem %s",
	     args->student->id, args->problem->id);
  }
  ListDispose(&log_streams);
}

int main(int argc, char* argv[]) {
  TesterOpts opts;
  TesterOptsInit(&opts, argc, argv);
  StudentList students;
  ListStudents(opts.students_dir, &students);
  StudentListLogInfo(&students);
  ProblemSet problems;
  ListProblemSet(opts.problems_dir, &problems);
  ThreadPool pool;
  ThreadPoolInit(&pool, /*num_workers=*/5);
  for (int i = 0; i < students.size; ++i) {
    Student* student = StudentListGet(&students, i);
    if (opts.mode == STUDENT && strcmp(student->id, opts.student_name) != 0) {
      continue;
    }
    for (int j = 0; j < problems.size; ++j) {
      ProblemInfo* problem = ProblemSetGet(&problems, j);
      ProblemResult result;
      ProblemResultInit(&result, problem->id);
	  result.individual_tests = problem->individual_tests;
      ListAdd(&student->problems, &result);
      Args* args = malloc(sizeof(Args));
      args->opts = &opts;
      args->student = student;
      args->problem = problem;
      args->result = ListGet(&student->problems,
			     student->problems.size - 1);
      ThreadPoolSchedule(&pool, Eval, args, free);
    }
  }
  ThreadPoolWait(&pool);
  ThreadPoolDispose(&pool);
  StudentListLogResults(&students, /*log_individual_tests=*/false);
  OutputResultAsCsv(&students, &opts);
  StudentListDispose(&students);
  ProblemSetDispose(&problems);
  TesterOptsDispose(&opts);
  return 0;
}
