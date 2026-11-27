#include "competition.h"

#include <stdlib.h>

#include "robot.h"

/*
 * RobotFightWith(x, y)              true if x beats y (robot.h). It takes a while
 *
 * pthread_create(&t, NULL, fn, arg)   pthread_join(t, &result)   pthread_detach(t)
 * pthread_mutex_t m;     pthread_mutex_init(&m, NULL)   pthread_mutex_lock(&m)   ..._unlock(&m)
 * pthread_cond_t c;      pthread_cond_init(&c, NULL)
 *                        pthread_cond_wait(&c, &m)   pthread_cond_signal(&c)   ..._broadcast(&c)
 * sem_t s;               sem_init(&s, 0, value)   sem_wait(&s)   sem_post(&s)   (#include <semaphore.h>)
 *
 * Change only competition.h and competition.c.
 */

void CompetitionInit(Competition* comp) {
  comp->num_free = 0;
  comp->num_fighting = 0;
  comp->stopped = false;
  pthread_mutex_init(&comp->lock, NULL);
  pthread_cond_init(&comp->changed, NULL);
}



typedef struct{
  const Robot* x;
  const Robot* y;
  Competition* comp;
}FightArgs;


static void* Fight(void* data){
  FightArgs* args = data;
  Competition* comp = args->comp;
  const Robot* winner = RobotFightWith(args->x, args->y) ? args->x : args->y;
  free(args);
  pthread_mutex_lock(&comp->lock);
  comp->num_fighting -= 2;
  comp->free[comp->num_free++] = winner;
  pthread_cond_signal(&comp->changed);
  pthread_mutex_unlock(&comp->lock);
  return NULL;
}


static bool Over(Competition* comp){
  return comp->stopped && comp->num_fighting == 0;
}


static void* Manage(void * data){
  Competition* comp = data;
  pthread_mutex_lock(&comp->lock);
  while(true){
    while(comp->num_free < 2 && !Over(comp)){
      pthread_cond_wait(&comp->changed, &comp->lock);
    }
    if(comp->num_free < 2) break;
    FightArgs* args = malloc(sizeof(FightArgs));
    args->comp = comp;
    args->x = comp->free[--comp->num_free];
    args->y = comp->free[--comp->num_free];
    comp->num_fighting += 2;
    pthread_t fight;
    pthread_create(&fight, NULL, Fight, args);
    pthread_detach(fight); //nobody joins it
  }
  const Robot* winner = comp->num_free == 1 ? comp->free[0] : NULL;
  pthread_mutex_unlock(&comp->lock);
  return (void*) winner;
}



void CompetitionStart(Competition* comp) {
  pthread_create(&comp->manager, NULL, Manage, comp);
}

const Robot* CompetitionStop(Competition* comp) {
  pthread_mutex_lock(&comp->lock);
  comp->stopped = true;
  pthread_cond_signal(&comp->changed);
  pthread_mutex_unlock(&comp->lock);
  void* winner;
  pthread_join(comp->manager, &winner);
  pthread_mutex_destroy(&comp->lock);
  pthread_cond_destroy(&comp->changed);
  return winner;
}

bool CompetitionAddParticipant(Competition* comp, const Robot* robot) {
  pthread_mutex_lock(&comp->lock);
  if(comp->stopped){
    pthread_mutex_unlock(&comp->lock);
    return false;
  }
  comp->free[comp->num_free++] = robot;
  pthread_cond_signal(&comp->changed);
  pthread_mutex_unlock(&comp->lock);
  return true;
}
