#include "str_element.h"
#include "refcount.h"
#include <stdio.h>
#include <string.h>


/* TODO: Implement all public str_element functions, including element interface functions.

You may add your own private functions here too. */

void print_str(struct element* e);
int compare_str(struct element* elem1, struct element* elem2);

struct str_element_class {
    void (*print)(struct element *);
    int (*compare)(struct element *, struct element *);
    char* (*str_element_get_value)(struct str_element*);
    int (*is_str_element)(struct element*);
} str_element_class_table = {print_str, compare_str, str_element_get_value, is_str_element};

struct str_element {
    struct str_element_class* class;
    char* value;
    int type;
};

void str_element_set(void *this_v, char* i) {
  struct str_element *this = this_v;
  this->value = rc_malloc((strlen(i) + 1), NULL);
  strcpy(this->value, i);
  this->type = 0;
}

void str_element_finalize(void *p) {
    struct str_element *str_elem = (struct str_element *)p;
    if (str_elem->value) {
        rc_free_ref(str_elem->value);
    }
}

/* Static constructor that creates new integer elements. */
struct str_element *str_element_new(char* value) {
    struct str_element *this = rc_malloc(sizeof(*this), str_element_finalize);
    this->class = &str_element_class_table;
    str_element_set(this, value);
    return this;
}
/* Static function that obtains the value held in an int_element. */
char* str_element_get_value(struct str_element *e) {
    struct str_element *str_elem = (struct str_element *)e;
    return str_elem->value;
}
/* Static function that determines whether this is an int_element. */
int is_str_element(struct element *e) {
    if (e->class == &str_element_class_table) {
        return 1;
    } else {
        return 0;
    }
}

void print_str(struct element* e) {
    struct str_element *str_elem = (struct str_element *)e;
    printf("%s", str_elem->value);
}

int compare_str(struct element* elem1, struct element* elem2) {
    struct str_element * e1 = (struct str_element *)elem1;
    if (is_str_element(elem2) == 1) {
        struct str_element * e2 = (struct str_element *)elem2;
        return strcmp(e1->value, e2->value);
    } else {
        return 1;
    }
}

