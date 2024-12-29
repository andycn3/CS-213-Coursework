#include "int_element.h"
#include "refcount.h"
#include <stdio.h>



/* TODO: Implement all public int_element functions, including element interface functions.

You may add your own private functions here too. */

void print_int(struct element* e);
int compare_int(struct element* elem1, struct element* elem2);

struct int_element_class {
    void (*print)(struct element *);
    int (*compare)(struct element *, struct element *);
    int (*int_element_get_value)(struct int_element*);
    int (*is_int_element)(struct element*);
} int_element_class_table = {print_int, compare_int, int_element_get_value, is_int_element};

struct int_element {
    struct int_element_class* class;
    int value;
    int type;
};

void int_element_set(void *this_v, int i) {
  struct int_element *this = this_v;
  this->value = i;
  this->type = 0;
}

/* Static constructor that creates new integer elements. */
struct int_element *int_element_new(int value) {
    struct int_element *this = rc_malloc(sizeof(*this), NULL);
    this->class = &int_element_class_table;
    int_element_set(this, value);
    return this;
}
/* Static function that obtains the value held in an int_element. */
int int_element_get_value(struct int_element *e) {
    struct int_element *int_elem = (struct int_element *)e;
    return int_elem->value;
}
/* Static function that determines whether this is an int_element. */
int is_int_element(struct element *e) {
    if (e->class == &int_element_class_table) {
        return 1;
    } else {
        return 0;
    }
}

void print_int(struct element* e) {
    struct int_element *int_elem = (struct int_element *)e;
    printf("%d", int_elem->value);
}

int compare_int(struct element* elem1, struct element* elem2) {
    struct int_element * e1 = (struct int_element *)elem1;
    if (is_int_element(elem2) == 1) {
        struct int_element * e2 = (struct int_element *)elem2;
        return e1->value - e2->value;
    } else {
        return -1;
    }
}
