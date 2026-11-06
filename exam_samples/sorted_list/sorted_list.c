#include "sorted_list.h"

#include <stdbool.h>
#include <stdlib.h>

void SortedListInit(SortedList *l, int key_size, int value_size,
                    CompareFn key_cmp_fn, FreeFn key_free_fn,
                    FreeFn value_free_fn) {
  // IMPLEMENT
}

void SortedListDestroy(SortedList *l) {
  // IMPLEMENT
}

void SortedListAdd(SortedList *l, void *key, void *value) {
  // IMPLEMENT
}

bool SortedListRemove(SortedList *l, void *key) {
  // IMPLEMENT
  return false;
}

const void* SortedListGet(SortedList *l, void *key) {
  // IMPLEMENT
  return NULL;
}
