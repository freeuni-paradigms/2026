#include "sorted_set.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* 
 * Function: SetNew 
 * Usage: SetNew(&stringSet, sizeof(char *), StringPtrCompare); 
 *        SetNew(&constellations, sizeof(pointT), DistanceCompare); 
 * ---------------- 
 * SetNew allocates the requisite space needed to manage what 
 * will initially be an empty sorted set.  More specifically, the 
 * routine allocates space to hold up to 'kInitialCapacity' (currently 4) 
 * client elements. 
 */

// typedef struct { 
//   void *base;
//   int elemSize;
//   int logLen;
//   int allocLen;
//   int (*cmpfn)(const void *, const void *);
// } sortedset;
#define MEMORY_SIZE(capacity, elemSize) (sizeof(int) + capacity * (elemSize + 2 * sizeof(int)))
#define ELEM_PTR(base, index, elemSize) ((char *)base + sizeof(int) + index * (elemSize + 2 * sizeof(int)))

static const int kInitialCapacity = 4; 
void SetNew(sortedset *set, int elemSize,  
            int (*cmpfn)(const void *, const void *)) {
    set->base = malloc(MEMORY_SIZE(kInitialCapacity, elemSize));
    if(set->base == NULL) {
        return;
    }
    set->elemSize = elemSize;
    set->logLen = 0;
    set->allocLen = kInitialCapacity;
    set->cmpfn = cmpfn;
}

void* findElem(sortedset * set, const void * elemPtr) {
    int cur_index = 0;
    void * cur_elem;

    while(cur_index != -1) {
       cur_elem =  ELEM_PTR(set->base, cur_index, set->elemSize);
       int cmp_res = set->cmpfn(elemPtr, cur_elem);
       
       if(!cmp_res) {
            return cur_elem;
       }

       // elemPtr < cur_elem
       if(cmp_res < 0) {
            cur_index = *(int*)((char*)cur_elem + set->elemSize);
       } else {
            cur_index = *(int*)((char*)cur_elem + set->elemSize + sizeof(int));
       }
    }

    return cur_elem;
}

/* 
 * Function: SetSearch 
 * Usage: if (SetSearch(&staffSet, &lecturer) == NULL)  
 *            printf("musta been fired"); 
 * ------------------- 
 * SetSearch searches for the specified client element according 
 * the whatever comparison function was provided at the time the 
 * set was created. A pointer to the matching element is returned 
 * for successful searches, and NULL is returned to denote failure. 
 */ 
void *SetSearch(sortedset *set, const void *elemPtr) {
    if(set->logLen == 0) {
        return NULL;
    }

    void * foundElem =  findElem(set, elemPtr);
    if(!set->cmpfn(foundElem, elemPtr)) {
        return foundElem;
    }

    return NULL;
} 

bool cpyElem(sortedset *set, const void *elemPtr, bool isRoot) {
    if(isRoot) {
        *(int*)set->base = 0;
    }

    void * next_elem_ptr = ELEM_PTR(set->base, set->logLen, set->elemSize);
    memcpy(next_elem_ptr, elemPtr, set->elemSize);
    *(int*)((char*)next_elem_ptr + set->elemSize) = -1;
    *(int*)((char*)next_elem_ptr + set->elemSize + sizeof(int)) = -1;

    set->logLen++;
    return true;
}

/* 
 * Function: SetAdd 
 * Usage: if (!SetAdd(&friendsSet, &name)) free(name); 
 * ---------------- 
* Adds the specified element to the set if not already present.  If 
 * present, the client element is not copied
 into the set.  true 
 * is returned if and only if the element at address elemPtr 
 * was copied into the set. 
 */ 
bool SetAdd(sortedset *set, const void *elemPtr) {

    if(set->logLen == 0) {
        return cpyElem(set, elemPtr, true);
    }

    void * cur_elem = findElem(set, elemPtr);
    int cmp_res = set->cmpfn(elemPtr, cur_elem);
    if(!cmp_res) {
        return false;
    }
    // cur_elem must be parent of elemPtr
    int * link = (int*)((char*)cur_elem + set->elemSize);  
    if(cmp_res > 0) {
        link++;
    }
    *link = set->logLen;

    if(set->logLen == set->allocLen) {
        void * tmp = realloc(set->base, MEMORY_SIZE(set->allocLen*2, set->elemSize)); 
        if(tmp == NULL) {
            *link = -1;
            return false;
        }
        set->base = tmp;
        set->allocLen *= 2;
    }

    return cpyElem(set, elemPtr, false);
}

void SetFree(sortedset * set, void (*freefn)(const void *)) {
    if(freefn != NULL) {
        for(int i = 0; i < set->logLen; i++) {
            freefn(ELEM_PTR(set->base, i, set->elemSize));
        }
    }
    free(set->base);
    set->base = NULL;
    set->logLen = 0;
    set->allocLen = 0;
}
