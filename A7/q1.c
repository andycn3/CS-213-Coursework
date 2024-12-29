#include <stdio.h>

int g[10];

void add(int a, int b) {
    g[b] = a + g[b];
}

void main() {
    int l1 = 1;
    int l2 = 2;
    add(3, 4);
    add(l1, l2);
    for (int i = 0; i < 10; i++) {
        printf("%d\n", g[i]);
    }
}