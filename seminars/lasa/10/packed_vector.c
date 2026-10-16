#include "packed_vector.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// typedef struct {
//     void * base; // int elem_size | Freefn frefn | elem
//     int logLen;
//     int allocLen;
// } Vector;

#define SIZE(p) (*(int*)p)
#define PREFIX (sizeof(int) + sizeof(FreeFn))
#define NEXT(p) ((char*)p + PREFIX + SIZE(p))
#define FREEFN(p) (*(FreeFn*)((char*)p + sizeof(int)))
#define ELEM(p) ((char*)p + PREFIX)

void * NodeAt(Vector * v, int index) {
    void* node = v->base;
    for(int i = 0; i < index; i++) {
        node = NEXT(node);
    }
    return node;
}

void VectorInit(Vector* v) {
  v->base = NULL;
  v->logLen = 0;
  v->size = 0;
}

void VectorDestroy(Vector* v) {
    for(int i = 0; i < v->logLen; i++) {
        void * node = NodeAt(v, i);
        FreeFn freefn = FREEFN(node);
        if(freefn != NULL) {
            freefn(ELEM(node));
        }
    }
    free(v->base);
    v->base = NULL;
    v->logLen = 0;
    v->size = 0;
}

void VectorInsert(Vector* v, int index, int elem_size, void* elem, FreeFn free_fn) {
    if(index > v->logLen) {
        return;
    }
    int oldSize = v->size;

    v->size += PREFIX + elem_size;
    v->base = realloc(v->base, v->size);
    assert(v->base != NULL);

    void* cur_node = NodeAt(v, index);
    // e1 e2 e3 e4, 
    if(index < v->logLen) {
        memmove((char*)cur_node + PREFIX + elem_size, cur_node, 
        oldSize - ((char*)cur_node - (char*)v->base));
    }

    SIZE(cur_node) = elem_size;
    FREEFN(cur_node) = free_fn;
    memcpy(ELEM(cur_node), elem, elem_size);
    v->logLen++;
}

void VectorRemove(Vector* v, int index) {
    if(index >= v->logLen) {
        return;
    }
    void * node = NodeAt(v, index);
    int size = SIZE(node);
    FreeFn freefn =  FREEFN(node);
    if(freefn != NULL) {
        freefn(ELEM(node));
    }

    // 1 2 3 4 -> 1 3 4
    if(index + 1 < v->logLen) {
        memmove(node, NEXT(node), v->size - (NEXT(node) - (char*)v->base));
    }
    v->logLen--;
    v->size -= size + PREFIX;
}

void* VectorGet(Vector* v, int index, int* elem_size) {
    void* node = NodeAt(v, index);
    int size = SIZE(node);

    *elem_size = size;
    return ELEM(node);
}

void VectorRotate(Vector* v, int index) {
    void * node = NodeAt(v, index);
    int leftSize = (char*)node - (char*)v->base;
    int rightSize = v->size - leftSize;

    void * tmp = malloc(leftSize);
    memcpy(tmp, v->base, leftSize);
    memmove(v->base, node, rightSize);
    memmove((char*)v->base + rightSize, tmp, leftSize);
    free(tmp);
}

void VectorMap(Vector* v, MapFn map_fn, void* aux) {
    for(int i = 0; i < v->logLen; i++) {
        void * node = NodeAt(v, i);
        map_fn(i, SIZE(node), ELEM(node), aux);
    }
}
