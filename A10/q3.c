#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#define NUM_THREADS 3
uthread_t threads[NUM_THREADS];
int complete_threads = 0;
uthread_cond_t cond_threads;
uthread_mutex_t mut_threads;

void randomStall() {
  int i, r = random() >> 16;
  while (i++<r);
}

void waitForAllOtherThreads() {
  uthread_mutex_lock(mut_threads);
	complete_threads++;
	if (complete_threads != NUM_THREADS) {
    uthread_cond_wait(cond_threads);
  } else {
    uthread_cond_broadcast(cond_threads);
  }
	uthread_mutex_unlock(mut_threads);
}

void* p(void* v) {
  randomStall();
  printf("a\n");
  waitForAllOtherThreads();
  printf("b\n");
  return NULL;
}

int main(int arg, char** arv) {
  uthread_init(4);
  mut_threads = uthread_mutex_create();
  cond_threads = uthread_cond_create(mut_threads);
  for (int i=0; i<NUM_THREADS; i++)
    threads[i] = uthread_create(p, NULL);
  for (int i=0; i<NUM_THREADS; i++)
    uthread_join (threads[i], NULL);
  printf("------\n");
}