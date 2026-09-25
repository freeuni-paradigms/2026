#include <stdio.h>
#include <string.h>


void _strcpy(char * src, char * dest) {
    int i = 0;
    while(src[i] != '\0'){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}


int _strlen(char * c) {
    int i = 0;
    while(c[i] != '\0') {
        i++;
    }
    return i;
}


bool _strcmp(char * src, char * dest) {
    if(_strlen(src) != _strlen(dest)) {
        return false;
    }
    
    int i = 0;
    while(src[i] != '\0') {
        if (dest[i] != src[i]) {
            return false;
        }
        i++;
    }
    return true;
}

void _strcat(char * src, char * dest) {
    int destlen = _strlen(dest);
    _strcpy(src, dest + destlen);
}


int main() {

    unsigned char c = 151;
    short sh = c;

    // printf("Sh: %d\n", sh);

    int i = 46 + (1 << 15);
    unsigned short s = i;

    // printf("i: %d\n", i);
    // printf("s: %d\n", s);

    float f = 2.95;
    int x = f;

    int x1 = *(int*)&f;

    // printf("f : %f\n", f);
    // printf("x : %d\n", x);
    // printf("x1 : %d\n", x1);

    short s1 = 122;
    int x2 = *(int*)&s1;

    // printf("s1: %b\n", s1);
    // printf("x2: %b\n", x2);

    char * src = "World";
    char dest[20] = "Hello ";

    // _strcpy(dest, src);

    // printf("%d\n", _strlen(dest));
    // printf("%d\n", _strcmp(src, dest));
    _strcat(src, dest);
    printf("%s\n", dest);
}