#include "gl/threads/pool.h"

#include <stdlib.h>

#include "gl/lunit/lunit.h"

TEST(Empty) {
  ThreadPool pool;
  ThreadPoolInit(&pool, /*num_workers=*/0);
  ThreadPoolWait(&pool);
  ThreadPoolDispose(&pool);
}

int* counts = NULL;

void Count(void* arg) {
  ++counts[*(int*)arg];
}

TEST(Sequential) {
  int num_tasks = 10;
  counts = malloc(num_tasks * sizeof(int));
  memset(counts, 0, num_tasks * sizeof(int));
  ThreadPool pool;
  ThreadPoolInit(&pool, /*num_workers=*/1);
  for (int i = 0; i < num_tasks; ++i) {
    int* arg = malloc(sizeof(int));
    *arg = i;
    ThreadPoolSchedule(&pool, Count, arg, free);
  }
  ThreadPoolWait(&pool);
  ThreadPoolDispose(&pool);
  for (int i = 0; i < num_tasks; ++i) {
    ASSERT_INT_EQ(counts[i], 1);
  }  
}

TEST(TwoWorkers) {
  int num_tasks = 10;
  counts = malloc(num_tasks * sizeof(int));
  memset(counts, 0, num_tasks * sizeof(int));
  ThreadPool pool;
  ThreadPoolInit(&pool, /*num_workers=*/2);
  for (int i = 0; i < num_tasks; ++i) {
    int* arg = malloc(sizeof(int));
    *arg = i;
    ThreadPoolSchedule(&pool, Count, arg, free);
  }
  ThreadPoolWait(&pool);
  ThreadPoolDispose(&pool);
  for (int i = 0; i < num_tasks; ++i) {
    ASSERT_INT_EQ(counts[i], 1);
  }  
}

int main(int argc, char* argv[]) {
  LUnitOpts opts;
  LUnitOptsInit(&opts, argc, argv);
  RUN_TEST(Empty, &opts);
  RUN_TEST(Sequential, &opts);
  RUN_TEST(TwoWorkers, &opts);
  return 0;
}
