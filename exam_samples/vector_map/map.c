#include "vector.h"
#include "map.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void MapInit(Map* m, int key_size, int value_size, CmpFn key_cmp_fn, FreeFn key_free_fn, FreeFn value_free_fn) {
  // IMPLEMENT
}

void MapDestroy(Map* m) {
  // IMPLEMENT
}

void MapSet(Map* m, void* key, void* value) {
  // IMPLEMENT
}


void* MapGet(Map* m, void* key) {
  // IMPLEMENT
  return NULL;
}

void MapRemove(Map* m, void* key) {
  // IMPLEMENT
}

void MapMap(Map* m, MapMapFn map_fn, void* aux) {
  // IMPLEMENT
}
