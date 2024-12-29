#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"
#include "queue.h"
#include "disk.h"

queue_t pending_read_queue;
int steps;
int value;

void interrupt_service_routine() {
  void* val;
  void (*callback)(void*,void*);
  queue_dequeue (pending_read_queue, &val, NULL, &callback);
  callback (val, NULL);
}

void handleOtherReads(void *resultv, void *countv) {
  value = *(int*) resultv;
  steps--;
}

void handleFirstRead(void *resultv, void *countv) {
  steps = *(int*) resultv;
  value = *(int*) resultv;
}

int main(int argc, char **argv) {
  // Command Line Arguments
  static char* usage = "usage: treasureHunt starting_block_number";
  int starting_block_number;
  char *endptr;
  if (argc == 2)
    starting_block_number = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();


  // Start the Hunt
  // TODO
  int result;
  queue_enqueue(pending_read_queue, &result, NULL, handleFirstRead);
  disk_schedule_read(&result, starting_block_number);
  while (steps == 0); // infinite loop so that main doesn't return before hunt completes
  int jump[result];
  int max = result;
  for (int i = 0; i < max; i++) {
    queue_enqueue(pending_read_queue, &jump[i], NULL, handleOtherReads);
    disk_schedule_read(&jump[i], value);
    while(value == result);
    result = value;
  }

  while (steps > 0);
  printf("%d \n", jump[max - 1]);  

  exit(EXIT_SUCCESS);
}
