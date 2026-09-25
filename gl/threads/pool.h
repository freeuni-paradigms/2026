#ifndef GL_THREADS_POOL_H_
#define GL_THREADS_POOL_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#include "gl/containers/list.h"

typedef void (*TaskFn)(void*);

typedef struct {
  TaskFn task_fn;
  void* args;
  FreeFn free_fn;
} Task;

void TaskInit(Task* task, TaskFn task_fn, void* args, FreeFn free_fn);
void TaskDispose(Task* task);

typedef struct {
  int num_workers;
  pthread_mutex_t lock;
  List tasks;
  int processed_tasks;
  sem_t* task_available;
  sem_t* worker_done;
  bool shutdown;
} ThreadPool;

void ThreadPoolInit(ThreadPool* pool, int num_workers);
// Thread pool must already be shutdown.
void ThreadPoolDispose(ThreadPool* pool);
bool ThreadPoolSchedule(ThreadPool* pool, TaskFn task_fn, void* args,
			FreeFn free_fn);
void ThreadPoolWait(ThreadPool* pool);


#endif // GL_THREADS_POOL_H_
