// !!! YOU ARE NOT ALLOWED TO CHANGE THIS FILE !!!

#include <stdbool.h>

// Returns 0 if two passed elements are equal.
// Returns negative number if first element is less than second one.
// Returns positive number otherwise.
typedef int (*CompareFn)(const void *, const void *);

// Must free up memory at given pointer.
typedef void (*FreeFn)(void *);

typedef struct {
  void *base;
  int log_len;
  int alloc_len;
  int key_size;
  int value_size;
  CompareFn key_cmp_fn;
  FreeFn key_free_fn;
  FreeFn value_free_fn;
} SortedList;

// Initializes list.
void SortedListInit(SortedList *l, int key_size, int value_size,
                    CompareFn key_cmp_fn, FreeFn key_free_fn,
                    FreeFn value_free_fn);

// Frees up all dynamically allocated memory used by list.
void SortedListDestroy(SortedList *l);

// Adds new key/value pair to the list.
// Takes ownership of both key and value pointers.
void SortedListAdd(SortedList *l, void *key, void *value);

// Removes key/value pair associated with given key from the list.
// Returns true if key was found, false otherwise.
// Does not take ownerhip of give key pointer.
bool SortedListRemove(SortedList *l, void *key);

// Gets address of the value associated with the given key.
// Returns NULL if no such key was found.
// Does not take ownership of given key pointer.
// Does not give ownership of returned pointer to the caller.
const void* SortedListGet(SortedList *l, void *key);
