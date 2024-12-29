#include <stdio.h>

int q2(int arg0,int arg1, int arg2) {
    int result;

    if (arg0 < 10) {
        result = 0;
    } else if (arg0 > 18) {
        result = 0;
    } else {
        arg0 -= 10;
        if (arg0 % 2 == 1) {
            result = 0;
        } else {
            switch (arg0) {
                case 0: result = arg1 + arg2; break;
                case 2: result = arg1 - arg2; break;
                case 4: 
                    if (arg1 > arg2) {
                        result = 1;
                    } else {
                        result = 0;
                    }
                    break;
                case 6:
                    if (arg2 > arg1) {
                        result = 1;
                    } else {
                        result = 0;
                    }
                    break;
                case 8:
                    if (arg1 == arg2) {
                        result = 1;
                    } else {
                        result = 0;
                    }
                    break;
            }
        }
    }
    
    return result;
}

int main() {
    printf("%d", q2(12, 10, 6));
}