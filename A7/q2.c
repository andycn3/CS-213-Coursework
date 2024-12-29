#include <stdio.h>

int x[8] = {1, 2, 3, -1, -2, 0, 184, 340057058};
int y[8];

int f(int a) {
    int l1 = 0;
    while (a != 0) {
        if ((a & 0x80000000) != 0) {
            l1++;
        }
        a = a << 1;
    }
    return l1;
}

void main() {
    int l1 = 8;
    while (l1 != 0) {
        l1--;
        y[l1] = f(x[l1]);
    }

    for (int i = 0; i < 8; i++) {
        printf("%d\n", x[i]);
    }
    for (int i = 0; i < 8; i++) {
        printf("%d\n", y[i]);
    }
}