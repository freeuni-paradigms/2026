#include "Scheme.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *ConcatAll(nodeType *list)
{   
    nodeType type = *list;

    if(type == Nil || type == Integer) {
        return strdup("");
    }

    if(type == String) {
        return strdup((char*)(list + 1));
    }

    nodeType ** ptr = (nodeType**)(list + 1);
    char* left = ConcatAll(ptr[0]);
    char* right = ConcatAll(ptr[1]);

    char * result =  malloc(strlen(left) + strlen(right) + 1);
    strcpy(result, left);
    strcat(result, right);

    free(left);
    free(right);
    
    return result;
}


int SumAll(nodeType * list) {

    nodeType type = *list;

    if(type == Nil || type == String) {
        return 0;
    }

    if(type == Integer) {
        return *(int*)(list + 1);
    }

    nodeType ** ptr = (nodeType**)(list + 1);
    int left = SumAll(ptr[0]);
    int right = SumAll(ptr[1]);

    return left + right;
}

void Test0() {
        nodeType *stringNodes[32] = {};
    nodeType *intNodes   [32] = {};
    nodeType *listNodes  [32] = {};

    nodeType  nilNode = Nil;


    for (int i = 0; i < 32; i++) {
        char some_string[32] = {};

        sprintf(some_string, "test%d", i);

        
        nodeType *ptr = malloc(sizeof(nodeType) + strlen(some_string) + 1);

        *ptr = String;

        strcpy((char *)(ptr + 1), some_string);
        stringNodes[i] = ptr;
    }

    for (int i = 0; i < 32; i++) {
        
        nodeType *ptr = malloc(sizeof(nodeType) + sizeof(int));
        *ptr = Integer;

        memcpy(ptr + 1, &i, sizeof(int));

        intNodes[i] = ptr;
    }


    for (int i = 0; i < 32; i++) {
        
        nodeType *ptr = malloc(sizeof(nodeType) + 2 * sizeof(void *));
        *ptr = List;
        
        // set both pointers to nil

        ((nodeType **) (ptr + 1))[0] = &nilNode;
        ((nodeType **) (ptr + 1))[1] = &nilNode; 
        
        listNodes[i] = ptr;
    }

    for (int i = 0; i < 31; i++) {
        // connect all lists to eachother
        ((nodeType **) (listNodes[i] + 1))[1] = listNodes[i + 1]; 

        // even lists point to strings, odds point to ints
        if ( i % 2 == 0) {
            ((nodeType **) (listNodes[i] + 1))[0] = stringNodes[i]; 
        } else {
            ((nodeType **) (listNodes[i] + 1))[0] = intNodes[i]; 
        }
    }

    printf("%s\n", ConcatAll(listNodes[0]));
    printf("%d\n", SumAll(listNodes[0]));
}

// (Yankees 2 Diamondbacks 1)
void Test1() {
    nodeType * list =  malloc(sizeof(nodeType) + 2 * sizeof(void*));
    nodeType * list1 =  malloc(sizeof(nodeType) + 2 * sizeof(void*));
    nodeType * list2 =  malloc(sizeof(nodeType) + 2 * sizeof(void*));
    nodeType * list3 =  malloc(sizeof(nodeType) + 2 * sizeof(void*));

    *list = List;
    *list1 = List;
    *list2 = List;
    *list3 = List;

    nodeType  nilNode = Nil;
    nodeType* Yankees =  malloc(sizeof(nodeType) + strlen("Yankees") + 1);
    *Yankees = String;
    strcpy((char*)(Yankees + 1), "Yankees");

    nodeType* Diamondbacks =  malloc(sizeof(nodeType) + strlen("Diamondbacks") + 1);
    *Diamondbacks = String;
    strcpy((char*)(Diamondbacks + 1), "Diamondbacks");

    nodeType* int2 =  malloc(sizeof(nodeType) + sizeof(int));
    *int2 =  Integer;
    *(int*)(int2 + 1) = 2;

    nodeType* int1 =  malloc(sizeof(nodeType) + sizeof(int));
    *int1 =  Integer;
    *(int*)(int1 + 1) = 1;


    ((nodeType**)(list + 1))[0] = Yankees;
    ((nodeType**)(list + 1))[1] = list1;

    ((nodeType**)(list1 + 1))[0] = int2;
    ((nodeType**)(list1 + 1))[1] = list2;

    ((nodeType**)(list2 + 1))[0] = Diamondbacks;
    ((nodeType**)(list2 + 1))[1] = list3;

    ((nodeType**)(list3 + 1))[0] = int1;
    ((nodeType**)(list3 + 1))[1] = &nilNode;

    char * result = ConcatAll(list); 
    printf("%s\n", result);
    printf("%d\n", SumAll(list));

    free(int1);
    free(int2);
    free(Yankees);
    free(Diamondbacks);

    free(list);
    free(list1);
    free(list2);
    free(list3);

    free(result);
}

int main() {
    Test0();
    Test1();
}