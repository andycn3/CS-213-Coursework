#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

uthread_t t0, t1, t2;

char flag = 0;
uthread_cond_t con;
uthread_mutex_t mut;

void randomStall() {
  int i, r = random() >> 16;
  while (i++<r);
}

void* p0(void* v) {
  randomStall();
  uthread_mutex_lock(mut);
  while (flag != 0) {
    uthread_cond_wait(con);
  }
  flag++;
  uthread_cond_broadcast(con);
  uthread_mutex_unlock(mut);
  printf("zero\n");
  return NULL;
}

void* p1(void* v) {
  randomStall();
  uthread_mutex_lock(mut);
  while (flag != 1) {
    uthread_cond_wait(con);
  }
  flag++;
  uthread_cond_broadcast(con);
  uthread_mutex_unlock(mut);
  printf("one\n");
  return NULL;
}

void* p2(void* v) {
  randomStall();
  uthread_mutex_lock(mut);
  while (flag != 2) {
    uthread_cond_wait(con);
  }
  flag++;
  uthread_cond_broadcast(con);
  uthread_mutex_unlock(mut);
  printf("two\n");
  return NULL;
}

int main(int arg, char** arv) {
  uthread_init(4);

  mut = uthread_mutex_create();
  con = uthread_cond_create(mut);

  t0 = uthread_create(p0, NULL);
  t1 = uthread_create(p1, NULL);
  t2 = uthread_create(p2, NULL);
  randomStall();
  uthread_join (t0, NULL);
  uthread_join (t1, NULL);
  uthread_join (t2, NULL);
  printf("three\n");
  printf("------\n");

  uthread_cond_destroy(con);
  uthread_mutex_destroy(mut);
}
