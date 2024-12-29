#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct str {
    int length;
    char* data;
};

struct str str1 = {30, "Welcome! Please enter a name:\n"};
struct str str2 = {11, "Good luck, "};
struct str str3 = {43, "The secret phrase is \"squeamish ossifrage\"\n"};

void print(struct str* s) {
    write(1, s->data, s->length);
}

int main() {
    char buf[128];
    print(&str1);
    int space = read(0, buf, 256);
    print(&str2);
    write(1, buf, space);
}

void proof() {
    print(&str3);
    exit(0);
}


