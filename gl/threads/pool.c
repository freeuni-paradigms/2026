#include "gl/threads/pool.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "gl/logging/logging.h"

#define TASK_AVAILABLE "/gl/threads/pool/task_available"
#define WORKER_DONE "/gl/threads/pool/worker_done"

void TaskInit(Task* task, TaskFn task_fn, void* args, FreeFn free_fn) {
  task->task_fn = task_fn;
  task->args = args;
  task->free_fn = free_fn;
}

void TaskDispose(Task* task) {
  if (task->free_fn != NULL) {
    task->free_fn(task->args);
  }
}

typedef struct {
  sem_t* task_available;
  sem_t* done;
  pthread_mutex_t* lock;
  List* tasks;
  int* processed_tasks;
  bool* shutdown;
} WorkerArgs;

void* Worker(void* pt) {
  WorkerArgs* args = pt;
  while (true) {
    if (sem_wait(args->task_available) == -1) {
      continue;
    }
    pthread_mutex_lock(args->lock);
    if (*args->shutdown && args->tasks->size == *args->processed_tasks) {
      pthread_mutex_unlock(args->lock);      
      break;
    }
    Task* task = ListGet(args->tasks, *args->processed_tasks);
    assert(task != NULL);
    ++(*args->processed_tasks);
    pthread_mutex_unlock(args->lock);
    task->task_fn(task->args);
  }
  sem_t* done = args->done;
  free(args);
  CHECK(sem_post(done) == 0);
  return NULL;
}

void TaskFree(void* pt) {
  TaskDispose(pt);
}

void ThreadPoolInit(ThreadPool* pool, int num_workers) {
  pool->num_workers = num_workers;
  ListInit(&pool->tasks, sizeof(Task), TaskFree);
  pool->processed_tasks = 0;
  pthread_mutex_init(&pool->lock, NULL);
#ifdef __APPLE__
  pool->task_available = sem_open(TASK_AVAILABLE, O_CREAT,
				  0644, 0);
  pool->worker_done = sem_open(WORKER_DONE, O_CREAT, 0644,
			       0);
  sem_unlink(TASK_AVAILABLE);
  sem_unlink(WORKER_DONE);  
#else
  pool->task_available = malloc(sizeof(sem_t));
  pool->worker_done = malloc(sizeof(sem_t));
  sem_init(pool->task_available, /*pshared=*/0, 0);
  sem_init(pool->worker_done, /*pshared=*/0, 0);
#endif
  pool->shutdown = false;
  for (int i = 0; i < num_workers; ++i) {
    WorkerArgs* args = malloc(sizeof(WorkerArgs));
    args->task_available = pool->task_available;
    args->done = pool->worker_done;
    args->lock = &pool->lock;
    args->tasks = &pool->tasks;
    args->processed_tasks = &pool->processed_tasks;
    args->shutdown = &pool->shutdown;
    pthread_t tid;
    CHECK(pthread_create(&tid, NULL, Worker, args) == 0);
    CHECK(pthread_detach(tid) == 0);
  }
}

void ThreadPoolDispose(ThreadPool* pool) {
  assert(pool->shutdown);
  ListDispose(&pool->tasks);
  pthread_mutex_destroy(&pool->lock);
#ifdef __APPLE__
  sem_close(pool->task_available);
  sem_close(pool->worker_done);
#else
  sem_destroy(pool->task_available);
  sem_destroy(pool->worker_done);
  free(pool->task_available);
  free(pool->worker_done);
#endif
}

bool ThreadPoolSchedule(ThreadPool* pool, TaskFn task_fn, void* args,
			FreeFn free_fn) {
  bool added = false;
  Task task;
  TaskInit(&task, task_fn, args, free_fn);
  pthread_mutex_lock(&pool->lock);
  if (!pool->shutdown) {
    ListAdd(&pool->tasks, &task);
    pthread_mutex_unlock(&pool->lock);
    sem_post(pool->task_available);
    added = true;
  }
  return added;
}

void ThreadPoolWait(ThreadPool* pool) {
  pthread_mutex_lock(&pool->lock);
  pool->shutdown = true;
  pthread_mutex_unlock(&pool->lock);
  for (int i = 0; i < pool->num_workers; ++i) {
    CHECK(sem_post(pool->task_available) == 0);
  }
  for (int i = 0; i < pool->num_workers; ++i) {
    if (sem_wait(pool->worker_done) == -1) {
      --i;
      continue;
    }
  }
}
