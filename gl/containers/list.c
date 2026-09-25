#include "gl/containers/list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define LIST_INIT_LEN 4
#define LIST_GROWTH_MULTIPLIER 2

void ListInit(List* list, size_t elem_size, FreeFn free_fn) {
  list->size = 0;
  list->elem_size = elem_size;
  list->alloc_len = LIST_INIT_LEN;
  list->elems = malloc(list->alloc_len * list->elem_size);
  assert(list->elems != NULL);
  list->free_fn = free_fn;
}

void ListDispose(List* list) {
  assert(list->elems != NULL);
  if (list->free_fn) {
    for (int i = 0; i < list->size; ++i) {
      list->free_fn(ListGet(list, i));
    }
  }
  if (list->elems != NULL) {
    free(list->elems);
  }
}

void ListAdd(List* list, const void* elem) {
  if (list->size == list->alloc_len) {
    list->alloc_len *= LIST_GROWTH_MULTIPLIER;
    list->elems = realloc(list->elems, list->alloc_len * list->elem_size);
    assert(list->elems != NULL);
  }
  memcpy(list->elems + list->size * list->elem_size, elem, list->elem_size);
  list->size++;
}

void* ListGet(List* list, int index) {
  assert(index < list->size);
  return list->elems + index * list->elem_size;
}
