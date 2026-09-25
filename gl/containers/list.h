#ifndef GL_CONTAINERS_LIST_H_
#define GL_CONTAINERS_LIST_H_

#include <stddef.h>

// Element destructor function.
typedef void(*FreeFn)(void*);

typedef struct {
  // Pointer the the continuous memory used to store list items. 
  void* elems;
  // Size of each element in bytes.
  size_t elem_size;
  // Element destructor.
  FreeFn free_fn;
  // Number of elements in the list.
  int size;
  // Represents how many elements can list hold at the moment.
  int alloc_len;
} List;

// Initializes new list where each element takes |elem_size| number of bytes.
// |free_fn| function will be invoked with points to each element during list
// destruction.
void ListInit(List* list, size_t elem_size, FreeFn free_fn);
// Cleans up memory used be the |list|.
void ListDispose(List* list);
// Adds new element to the |list| and copies contents of |elem| into it.
void ListAdd(List* list, const void* elem);
// Returns pointer to |index|-th element from the list.
void* ListGet(List* list, int index);

#endif // GL_CONTAINERS_LIST_H_
