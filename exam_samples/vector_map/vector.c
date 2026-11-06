#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ELEM(v, i) (((char*)((v)->base_)) + ((v)->elem_size_) * (i))

void VectorInit(Vector* v, int elem_size) {
  v->elem_size_ = elem_size;
  v->log_len_ = 0;
  v->alloc_len_ = 4;
  v->base_ = malloc(v->alloc_len_ * elem_size);
  assert(v->base_ != NULL);
}

void VectorDestroy(Vector* v) {
  free(v->base_);
}

void VectorAppend(Vector* v, void* elem) {
  if (v->log_len_ == v->alloc_len_) {
    v->alloc_len_ *= 2;
    v->base_ = realloc(v->base_, v->alloc_len_ * v->elem_size_);
    assert(v->base_ != NULL);
  }
  memcpy(ELEM(v, v->log_len_), elem, v->elem_size_);
  ++v->log_len_;
}

void VectorRemove(Vector* v, int index) {
  assert(0 <= index && index <= v->log_len_);
  memmove(ELEM(v, index), ELEM(v, index + 1), v->elem_size_ * (v->log_len_ - index - 1));
  --v->log_len_;
}

void VectorMap(Vector* v, MapFn map_fn, void* aux) {
  for (int i = 0; i < v->log_len_; ++i) {
    map_fn(ELEM(v, i), aux);
  }
}
