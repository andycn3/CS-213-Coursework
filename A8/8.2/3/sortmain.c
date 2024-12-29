#include <stdio.h>
#include <stdlib.h>

#include "int_element.h"
#include "str_element.h"
#include "element.h"
#include "refcount.h"

/* If the string is numeric, return an int_element. Otherwise return a str_element. */
struct element *parse_string(char *str) {
  char *endp;
  /* strtol returns a pointer to the first non-numeric character in endp.
     If it gets to the end of the string, that character will be the null terminator. */
  int value = strtol(str, &endp, 10);
  if(str[0] != '\0' && endp[0] == '\0') {
    /* String was non-empty and strtol conversion succeeded - integer */
    return (struct element *)int_element_new(value);
  } else {
    return (struct element *)str_element_new(str);
  }
}

int main(int argc, char **argv) {
  /* TODO: Read elements into a new array using parse_string */
  struct element* e[argc];
  for (int i = 1; i < argc; i++) {
    e[i-1] = parse_string(argv[i]);
  }
  /* TODO: Sort elements with qsort */

int compare_elements(const void *a, const void *b) {
    struct element *elem1 = *(struct element **)a;
    struct element *elem2 = *(struct element **)b;
    return elem1->class->compare(elem1, elem2);
}

  qsort(e, argc - 1, sizeof(struct element *), compare_elements);
  printf("Sorted: ");
  for (int i = 0; i < argc - 1; i++) {
    e[i]->class->print(e[i]);
    rc_free_ref(e[i]);
    printf(" ");
  }
  printf("\n");
}
