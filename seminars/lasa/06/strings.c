#include <stdio.h>
#include <stdlib.h>

// s1 = s2 -> 0
// s1 > s2 -> pos
// s1 < s2 -> neg


int strcmp(const char *s1, const char *s2)
{

    while(*s1 != '\0' || *s2 != '\0') {
        int sub = *s1++ - *s2++;
        if(sub != 0){
            return sub;
        }
    }

    return 0;
}

char *strcpy(char *dest, const char *src)
{   
    while(*src) {
        *dest++ = *src++;
    }
    *dest = '\0';

}

size_t strlen(const char *s)
{   
    int len = 0;
    while(*s++){len++;};
    return len;
}

char *strcat(char *dest, const char *src)
{   
    char* dest_start = dest;
    while(*dest++);
    dest--;
    strcpy(dest, src);
    return dest_start;
}

char *strdup(const char *src)
{
    char * dup = malloc(strlen(src) + 1);
    strcpy(dup, src);
    return dup;
}

void *memset(void *dest, int ch, size_t count)
{   
   void * dest_start = dest;
   while(count--) {
     *(char*)dest++ = ch;
   }
   return dest_start;
}

void *memcpy(void* dest, const void* src, int n) {
    void* dest_start = dest;
    while(n--) {
        *(char*)dest++ = *(char*)src++;
    }
    return dest_start;
}

void* memmove(void * dest, const void* src, int n){
   void * tmp = malloc(n);
   memcpy(tmp, src, n);
   memcpy(dest, tmp, n);
   free(tmp);
   return dest;
}

char *strstr(const char *haystack, const char *needle) {
    while(*haystack) {
        char * cur_haystack = haystack;
        char * cur_needle = needle;

        while(*cur_haystack && (*cur_haystack == *cur_needle)) {
            cur_haystack++;
            cur_needle++;
        }

        if(!*cur_needle) {
            return haystack;
        }
        haystack++;
    }
    return NULL;
}


int main() {
    char * s1 = "abc";
    char * s2 = "ab";

    int res = strcmp(s1, s2);
    if(res == 0){
        printf("s1 == s2\n");
    } else if(res < 0) {
        printf("s1 < s2\n");
    } else {
        printf("s1 > s2\n");
    }

    char * src = " World";
    char dest[50] = "Hello";
    // strcpy(dest, src);
    // printf("dest: %s\n", dest);
    printf("Concatenated string: %s\n", strcat(dest, src));

    char * dup =  strdup(src);
    printf("dup: %s\n", dup);

    printf("Address of src: %p\n", src);
    printf("Address of dup: %p\n", dup);

    // char dest1[50];
    // memset(dest1, 65, 10);
    // printf("dest1: %s\n", dest1);
    char dest1[50] = "Hello World";
    memmove(dest1+6, dest1, 8);
    printf("Final dest: %s\n", dest1);

    char * haystack = "Hello";
    char * needle = "lo";

    printf("start of needle in haystack: %s\n", strstr(haystack, needle));

}