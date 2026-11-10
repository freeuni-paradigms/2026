#include "max_2bit.h"

int FindMax(void* encoding) {

    int max = 0;
    int max_pos=0;
    int pos = 0;

    while(*(char*)encoding) {
        char ch = *(char*)encoding;
        int a = (ch >> 6) & 0b11;
        int b = (ch >> 4) & 0b11;
        int c = (ch >> 2) & 0b11;
        int d = (ch) & 0b11;

        if(a > max) {
            max = a;
            max_pos = pos;
        }

        if(b > max) {
            max = b;
            max_pos = pos + 1;
        }

        if(c > max) {
            max = c;
            max_pos = pos + 2;
        }
        
        if(d > max) {
            max = d;
            max_pos = pos + 3;
        }

        pos += 4;
        (char*)encoding++;
    }
    return max_pos;
}
