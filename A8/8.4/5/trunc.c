#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "list.h"

// Copy list.h and your completed list.c implementation into the q5 directory 
// and use the provided Makefile to compile and test your implementation of trunc

void print_str (element_t ev) {
  if (ev == NULL) {
    printf("(NULL)\n");
    return;
  } else {
    printf("%s\n", (char*)ev);
  }
}

void is_int(element_t* rv, element_t arg) {
  char* endptr;
  int result = strtol(arg, &endptr, 10);
  int **r = (int**) rv;
  if (*r == NULL) {
    *r = malloc(sizeof(int));
  }
  if (*endptr != '\0') {
    **r = -1;
  } else {
    **r = result;
  }
}

/* arg0 is ints list
   arg1 is initial list*/
void comp(element_t* rv, element_t arg0, element_t arg1) {
  int* check = (int*)arg0;
  char** r = (char**) rv;

  if (*check == -1) {
    *r = (char*)arg1;
  } else {
    *r = NULL;
  }
}

int is_pos(element_t x) {
  int* result = (int*)x;
  if (*result > 0) {
    return 1;
  } else {
    return 0;
  }
}

int is_real(element_t x) {
  if (x == NULL) {
    return 0;
  } else {
    return 1;
  }
}

void truncate(element_t* rv, element_t arg0, element_t arg1) {
  int* limit = (int*)arg0;
  char* word = (char*)arg1;
  char** dest = (char**)rv;

  *dest = malloc((*limit + 1) * sizeof(char));

  strncpy(*dest, word, *limit);
  (*dest)[*limit] = '\0';
}

void put_tgth(element_t* rv, element_t out_e, element_t in_val) {
  char* first = (char*)out_e;
  char* next = (char*)in_val;
  char** final = (char**)rv;

  char* x = " ";

  if (strlen(first) > 0) {
    strcat(first, x);
  }
  strcat(first, next);

  *rv = first;
}

void add (element_t* rv, element_t av, element_t bv) {
  int *a = av, *b = bv, **r = (int**) rv;
  if (*r == NULL)
    *r = malloc(sizeof(int));
  **r = *a + *b;
}

void find_max (element_t* rv, element_t av, element_t bv) {
  int *a = av;
  int *b = bv;
  int **r = (int**) rv;

  if (*r == NULL)
    *r = malloc(sizeof(int));

  if (*a > * b) {
    **r = *a;
  } else {
    **r = *b;
  }
}

int main (int argc, char** argv) {
  struct list* init = list_create();
  
  for (int i = 1; i < argc; i++) {
    list_append(init, argv[i]);
  }

  struct list* ints = list_create();
  list_map1(is_int, ints, init);

  struct list* with_null = list_create();
  list_map2(comp, with_null, ints, init);

  struct list* no_neg = list_create();
  list_filter(is_pos, no_neg, ints);

  struct list* no_null = list_create();
  list_filter(is_real, no_null, with_null);

  struct list* truncated = list_create();
  list_map2(truncate, truncated, no_neg, no_null);

  int s = 0, *sp = &s;
  list_foldl (add, (element_t*) &sp, no_neg);
  int size = (sizeof(char) * s) + (2 * list_len(truncated)) -1;
  char* str = (char*)malloc(size);
  memset(str, 0, size);
  char** startp = &str;
  list_foldl(put_tgth, (element_t*)startp, truncated);

  list_foreach(print_str, truncated);
  print_str(str);

  int max = 0, *maxp = &max;
  list_foldl (find_max, (element_t*) &maxp, no_neg);
  printf("%d", max);

  list_foreach(free, ints);
  list_foreach(free, truncated);

  list_destroy(init);
  list_destroy(ints);
  list_destroy(with_null);
  list_destroy(no_neg);
  list_destroy(no_null);
  list_destroy(truncated);

  free(str);
}
