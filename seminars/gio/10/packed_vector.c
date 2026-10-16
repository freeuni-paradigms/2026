#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A packed vector: elements of DIFFERENT sizes, all in ONE malloc'd block,
 * one after another. Every element carries its own size and its own free
 * function in front of its bytes:
 *
 *   [ size | free_fn | bytes... ][ size | free_fn | bytes... ][ size | free_fn | bytes... ]
 *     element 0                    element 1                    element 2
 *
 * size is an int (4 bytes) and free_fn is a function pointer (8 bytes), so an
 * element takes 12 + size bytes. No gaps between them: that's "packed".
 */
typedef void (*MapFn)(int index, int size, void *elem, void *aux);
typedef void (*FreeFn)(void *addr);

typedef struct {
  // to be completed: what does the vector have to remember?
  char* base;
  int count; // how many elements
  int bytes; // how many bytes of the block they take
} Vector;

/* an empty vector */
void VectorInit(Vector *v) {
  v->base = NULL;
  v->count = 0;
  v->bytes = 0;
}


static FreeFn * FreeFnField(char* e){
  return (FreeFn*)(e + sizeof(int));
}

static int* SizeField(char* e){
  return (int*)e;
}


static const int kHeaderSize = sizeof(int) + sizeof(FreeFn);

static char * BytesOf(char* e){
  return e + kHeaderSize;
}

static void FreeElem(char* e){
  FreeFn free_fn = *FreeFnField(e);
  if(free_fn != NULL){
    free_fn(BytesOf(e));
  }
}

static char* NextElem(char* e){
  return BytesOf(e) + *SizeField(e);
}


/* calls every element's free_fn, then frees the vector's memory */
void VectorDestroy(Vector *v) {
  char* e = v->base;
  for(int i = 0; i < v->count; i++){
    FreeElem(e);
    e = NextElem(e);
  }
  free(v->base);
}

static char* ElemAt(Vector* v, int index){
  char* e = v->base;
  for(int i = 0; i < index; i++){
    e = NextElem(e);
  }
  return e;
}

/* copies elem_size bytes from elem into the vector at index; the elements from index on move right */
void VectorInsert(Vector *v, int index, int elem_size, void *elem, FreeFn free_fn) {
  int newBytes = kHeaderSize + elem_size;
  v->base = realloc(v->base, v->bytes + newBytes);
  assert(v->base != NULL);
  
  char* e = ElemAt(v, index);
  int bytesAfter = v->base + v->bytes - e;
  memmove(e + newBytes, e, bytesAfter);

  *SizeField(e) = elem_size;
  *FreeFnField(e) = free_fn;
  memcpy(BytesOf(e), elem, elem_size);

  v->count++;
  v->bytes += newBytes;


}

/* frees the element at index; the elements after it move left */
void VectorRemove(Vector *v, int index) {
  char* e = ElemAt(v, index);
  char* next = NextElem(e);
  int removedBytes = next-e;
  int bytesAfter = v->base + v->bytes - e;

  FreeElem(e);
  memmove(e, next, bytesAfter);
  v->count --;
  v->bytes -= removedBytes;
}
/* the address of the element's bytes (inside the vector), and its size */
void *VectorGet(Vector *v, int index, int *elem_size) {

  char * e = ElemAt(v, index);
  *elem_size = *SizeField(e);
  return BytesOf(e);
}

/* element index becomes element 0, index + 1 becomes 1, ..., element 0 goes after the old last one */
void VectorRotate(Vector *v, int index) {
  if(index == 0) return;

  char* middle = ElemAt(v, index);
  int frontBytes = middle - v->base;
  int backBytes = v->bytes - frontBytes;

  char* front = malloc(frontBytes);
  assert(front != NULL);
  memcpy(front, v->base, frontBytes);

  memmove(v->base, middle, backBytes);
  memcpy(v->base + backBytes, front,  frontBytes);

  free(front);
}

/* calls map_fn on every element, in order */
void VectorMap(Vector *v, MapFn map_fn, void *aux) {
  char* e = v->base;
  for(int i = 0; i < v->count; i++){
    map_fn(i, *SizeField(e), BytesOf(e), aux);
    e = NextElem(e);
  }
}

/* ---------------------------------------------------------------- */
/*  tests - don't touch                                             */
/*    ./packed_vector                    all tests                  */
/*    make asan && ./packed_vector_asan  also finds leaks, double   */
/*                                       free, use after free       */
/* ---------------------------------------------------------------- */

static int passed = 0, total = 0;
static char why[300];   // what went wrong, printed under a FAIL

/* prints the name first, so if a test crashes you can see which one */
static void start(const char *name) {
  printf("  [ .. ] %s", name);
  why[0] = '\0';
  total++;
}

static void finish(bool ok) {
  printf("\r  [%s]\n", ok ? " OK " : "FAIL");
  if (ok) passed++;
  else printf("         %s\n", why);
}

/* is element index exactly these size bytes? (sets why if not) */
static bool Holds(Vector *v, int index, int size, const void *expected) {
  int got = -1;
  char *bytes = VectorGet(v, index, &got);
  if (bytes == NULL) {
    sprintf(why, "VectorGet(%d) returned NULL", index);
    return false;
  }
  if (got != size) {
    sprintf(why, "VectorGet(%d): the size should be %d, got %d", index, size, got);
    return false;
  }
  if (memcmp(bytes, expected, size) != 0) {
    sprintf(why, "VectorGet(%d): the size is right (%d), but the bytes are wrong", index, size);
    return false;
  }
  return true;
}

/* for a vector of ints: are elements 0 .. n-1 exactly the ints in expected, e.g. "10 20 30"? */
static bool IntsAre(Vector *v, int n, const char *expected) {
  char got[300] = "";
  for (int i = 0; i < n && strlen(got) < 250; i++) {
    int size = -1;
    int *x = VectorGet(v, i, &size);
    char *end = got + strlen(got);
    if (i > 0) *end++ = ' ';
    if (x == NULL) sprintf(end, "NULL");
    else if (size != sizeof(int)) sprintf(end, "(size %d)", size);
    else sprintf(end, "%d", *x);
  }
  if (strcmp(got, expected) == 0) return true;
  snprintf(why, sizeof(why), "should be  %.120s,  got  %.120s", expected, got);
  return false;
}

static void Count(int index, int size, void *elem, void *aux) {
  if (aux != NULL) (*(int *)aux)++;
}

/* a free_fn for elements that are a char *: frees the chars, and counts the calls */
static int freeCalls;
static void *lastFreed;

static void FreeString(void *addr) {
  freeCalls++;
  lastFreed = addr;
  free(*(char **)addr);
}

static void TestEmpty(void) {
  start("empty vector: VectorInit, VectorMap (no calls), VectorDestroy");
  Vector v;
  VectorInit(&v);
  int calls = 0;
  VectorMap(&v, Count, &calls);
  bool ok = calls == 0;
  if (!ok) sprintf(why, "VectorMap called map_fn %d times on an empty vector", calls);
  finish(ok);
  VectorDestroy(&v);
}

static void TestIntsAtEnd(void) {
  start("ints: insert 10 20 30 at the end");
  Vector v;
  VectorInit(&v);
  int x = 10;   // one variable for all three: VectorInsert has to copy the bytes
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  x = 20;
  VectorInsert(&v, 1, sizeof(int), &x, NULL);
  x = 30;
  VectorInsert(&v, 2, sizeof(int), &x, NULL);
  finish(IntsAre(&v, 3, "10 20 30"));
  VectorDestroy(&v);
}

static void TestIntsMiddle(void) {
  start("ints: insert in the middle and at the front -> 0 10 20 30 40");
  Vector v;
  VectorInit(&v);
  int values[] = {10, 30, 20, 0, 40};
  int indexes[] = {0, 1, 1, 0, 4};   // 10 / 10 30 / 10 20 30 / 0 10 20 30 / 0 10 20 30 40
  for (int i = 0; i < 5; i++) VectorInsert(&v, indexes[i], sizeof(int), &values[i], NULL);
  finish(IntsAre(&v, 5, "0 10 20 30 40"));
  VectorDestroy(&v);
}

static void TestIntsFront(void) {
  start("ints: 100 inserts at index 0 -> 99 98 ... 0");
  Vector v;
  VectorInit(&v);
  for (int i = 0; i < 100; i++) VectorInsert(&v, 0, sizeof(int), &i, NULL);
  bool ok = true;
  for (int i = 0; i < 100 && ok; i++) {
    int expected = 99 - i;
    ok = Holds(&v, i, sizeof(int), &expected);
  }
  finish(ok);
  VectorDestroy(&v);
}

static void TestIntsRemove(void) {
  start("ints: remove the last, the first, a middle one, insert it back");
  Vector v;
  VectorInit(&v);
  for (int i = 0; i < 10; i++) VectorInsert(&v, i, sizeof(int), &i, NULL);
  VectorRemove(&v, 9);   // 0 1 2 3 4 5 6 7 8
  VectorRemove(&v, 0);   // 1 2 3 4 5 6 7 8
  VectorRemove(&v, 3);   // 1 2 3 5 6 7 8
  bool ok = IntsAre(&v, 7, "1 2 3 5 6 7 8");
  if (ok) {
    int four = 4;
    VectorInsert(&v, 3, sizeof(int), &four, NULL);
    ok = IntsAre(&v, 8, "1 2 3 4 5 6 7 8");
  }
  finish(ok);
  VectorDestroy(&v);
}

static void TestMixed(void) {
  start("different sizes: int, 7 chars, double, 1 char");
  Vector v;
  VectorInit(&v);
  int x = 5;
  char *s = "freeuni";   // only its 7 chars, without the '\0'
  double d = 3.4;
  char c = 'x';
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  VectorInsert(&v, 1, strlen(s), s, NULL);
  VectorInsert(&v, 2, sizeof(double), &d, NULL);
  VectorInsert(&v, 3, sizeof(char), &c, NULL);
  bool ok = Holds(&v, 0, sizeof(int), &x) && Holds(&v, 1, 7, "freeuni") &&
            Holds(&v, 2, sizeof(double), &d) && Holds(&v, 3, 1, "x");
  finish(ok);
  VectorDestroy(&v);
}

static void TestMixedMiddle(void) {
  start("different sizes: remove and insert in the middle");
  Vector v;
  VectorInit(&v);
  int x = 5;
  double d = 3.4;
  VectorInsert(&v, 0, sizeof(int), &x, NULL);           // 5
  VectorInsert(&v, 1, 2, "hi", NULL);                   // 5 "hi"
  VectorInsert(&v, 2, sizeof(double), &d, NULL);        // 5 "hi" 3.4
  VectorInsert(&v, 3, 3, "uni", NULL);                  // 5 "hi" 3.4 "uni"
  VectorRemove(&v, 1);                                  // 5 3.4 "uni"
  VectorInsert(&v, 1, 10, "abcdefghij", NULL);          // 5 "abcdefghij" 3.4 "uni"
  VectorRemove(&v, 3);                                  // 5 "abcdefghij" 3.4
  VectorInsert(&v, 0, 1, "z", NULL);                    // "z" 5 "abcdefghij" 3.4
  bool ok = Holds(&v, 0, 1, "z") && Holds(&v, 1, sizeof(int), &x) &&
            Holds(&v, 2, 10, "abcdefghij") && Holds(&v, 3, sizeof(double), &d);
  finish(ok);
  VectorDestroy(&v);
}

static void TestPacked(void) {
  start("packed: one block, the same header before every element");
  Vector v;
  VectorInit(&v);
  int x = 5;
  double d = 3.4;
  char *s = "freeuni";
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  VectorInsert(&v, 1, strlen(s), s, NULL);
  VectorInsert(&v, 2, sizeof(double), &d, NULL);
  VectorInsert(&v, 3, 1, "x", NULL);
  VectorInsert(&v, 4, sizeof(int), &x, NULL);

  char *addr[5];
  int size[5];
  bool ok = true;
  for (int i = 0; i < 5 && ok; i++) {
    addr[i] = VectorGet(&v, i, &size[i]);
    if (addr[i] == NULL) {
      sprintf(why, "VectorGet(%d) returned NULL", i);
      ok = false;
    }
  }
  long header = 0;
  for (int i = 0; i + 1 < 5 && ok; i++) {
    long gap = addr[i + 1] - (addr[i] + size[i]);   // from the end of element i to the start of i + 1
    if (i == 0) header = gap;
    if (gap != header || gap <= 0 || gap > 64) {
      sprintf(why, "element %d starts %ld bytes after element %d ends, element 1 starts %ld bytes after element 0",
              i + 1, gap, i, header);
      ok = false;
    }
  }
  finish(ok);
  if (ok) printf("         %ld bytes of header in front of every element\n", header);
  VectorDestroy(&v);
}

static void TestFreeFn(void) {
  start("free_fn: Remove and Destroy call it on the element's address");
  Vector v;
  VectorInit(&v);
  char *a = strdup("free");
  char *b = strdup("abcdefgh");
  char *c = strdup("0123456789");
  VectorInsert(&v, 0, sizeof(char *), &a, FreeString);   // "free"   (a char *)
  VectorInsert(&v, 1, 3, "uni", NULL);                   // "uni"    (3 chars, nothing to free)
  VectorInsert(&v, 2, sizeof(char *), &b, FreeString);   // "abcdefgh"
  VectorInsert(&v, 3, sizeof(char *), &c, FreeString);   // "0123456789"
  freeCalls = 0;
  lastFreed = NULL;

  int size;
  void *addr = VectorGet(&v, 2, &size);
  bool ok = addr != NULL;
  if (!ok) sprintf(why, "VectorGet(2) returned NULL");
  if (ok) {
    VectorRemove(&v, 2);   // has a free_fn
    if (freeCalls != 1) {
      sprintf(why, "VectorRemove(2) called free_fn %d times, should be 1", freeCalls);
      ok = false;
    } else if (lastFreed != addr) {
      sprintf(why, "free_fn got %p, should get the element's address %p (what VectorGet returned)",
              lastFreed, addr);
      ok = false;
    }
  }
  if (ok) {
    VectorRemove(&v, 1);   // free_fn is NULL: nothing to call
    if (freeCalls != 1) {
      sprintf(why, "VectorRemove(1) called a free_fn, but that element has none");
      ok = false;
    }
  }
  if (ok) {
    char **first = VectorGet(&v, 0, &size);
    char **second = VectorGet(&v, 1, &size);
    if (first == NULL || second == NULL || strcmp(*first, "free") != 0 ||
        strcmp(*second, "0123456789") != 0) {
      sprintf(why, "after the removes, the elements should be \"free\" \"0123456789\"");
      ok = false;
    }
  }
  int before = freeCalls;
  VectorDestroy(&v);
  if (ok && freeCalls - before != 2) {
    sprintf(why, "VectorDestroy called free_fn %d times, should be 2 (the 2 strings left)",
            freeCalls - before);
    ok = false;
  }
  finish(ok);
}

static void TestRotateInts(void) {
  start("VectorRotate(10) on 0..49 -> 10..49 0..9, then VectorRotate(0)");
  Vector v;
  VectorInit(&v);
  for (int i = 0; i < 50; i++) VectorInsert(&v, i, sizeof(int), &i, NULL);
  VectorRotate(&v, 10);
  bool ok = true;
  for (int i = 0; i < 50 && ok; i++) {
    int expected = (10 + i) % 50;
    ok = Holds(&v, i, sizeof(int), &expected);
  }
  if (ok) {
    VectorRotate(&v, 0);   // changes nothing
    for (int i = 0; i < 50 && ok; i++) {
      int expected = (10 + i) % 50;
      ok = Holds(&v, i, sizeof(int), &expected);
    }
    if (!ok) strcat(why, " (after VectorRotate(0))");
  }
  finish(ok);
  VectorDestroy(&v);
}

static void TestRotateMixed(void) {
  start("VectorRotate(2) with different sizes and a free_fn");
  Vector v;
  VectorInit(&v);
  int x = 5;
  char *s = strdup("free");
  VectorInsert(&v, 0, sizeof(int), &x, NULL);            // 5
  VectorInsert(&v, 1, sizeof(char *), &s, FreeString);   // 5 "free"
  VectorInsert(&v, 2, 1, "a", NULL);                     // 5 "free" "a"
  VectorInsert(&v, 3, 6, "bcdefg", NULL);                // 5 "free" "a" "bcdefg"
  freeCalls = 0;
  VectorRotate(&v, 2);                                   // "a" "bcdefg" 5 "free"
  bool ok = Holds(&v, 0, 1, "a") && Holds(&v, 1, 6, "bcdefg") &&
            Holds(&v, 2, sizeof(int), &x) && Holds(&v, 3, sizeof(char *), &s);
  if (ok && freeCalls != 0) {
    sprintf(why, "VectorRotate called a free_fn: rotating only moves bytes, nothing is removed");
    ok = false;
  }
  finish(ok);
  VectorDestroy(&v);   // frees "free", wherever it is now
}

typedef struct {
  int calls;
  bool inOrder;      // index 0, 1, 2, ... ?
  bool sizesRight;
  bool addrsRight;   // the same address VectorGet gives?
  Vector *v;
} mapCheck;

static void CheckMap(int index, int size, void *elem, void *aux) {
  mapCheck *m = aux;
  static const int sizes[] = {4, 7, 8, 1};
  if (index != m->calls) m->inOrder = false;
  if (index >= 0 && index < 4) {
    if (size != sizes[index]) m->sizesRight = false;
    int s;
    if (elem != VectorGet(m->v, index, &s)) m->addrsRight = false;
  }
  m->calls++;
}

static mapCheck *expectedAux;

static void CheckAux(int index, int size, void *elem, void *aux) {
  if (aux != expectedAux) expectedAux = NULL;
}

static void TestMap(void) {
  start("VectorMap: every element in order: index, size, address, aux");
  Vector v;
  VectorInit(&v);
  int x = 5;
  double d = 3.4;
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  VectorInsert(&v, 1, 7, "freeuni", NULL);
  VectorInsert(&v, 2, sizeof(double), &d, NULL);
  VectorInsert(&v, 3, 1, "x", NULL);

  mapCheck m = {0, true, true, true, &v};
  expectedAux = &m;
  VectorMap(&v, CheckAux, &m);   // first only compare aux: if it's wrong, CheckMap can't use it
  bool ok = false;
  if (expectedAux != &m) {
    sprintf(why, "map_fn didn't get the aux that VectorMap got");
  } else {
    VectorMap(&v, CheckMap, &m);
    if (m.calls != 4) sprintf(why, "map_fn was called %d times, should be 4", m.calls);
    else if (!m.inOrder) sprintf(why, "the indexes should come in order: 0, 1, 2, 3");
    else if (!m.sizesRight) sprintf(why, "the sizes should be 4, 7, 8, 1");
    else if (!m.addrsRight) sprintf(why, "elem should be the element's address (what VectorGet returns)");
    else ok = true;
  }
  finish(ok);
  VectorDestroy(&v);
}

/* a simple random number generator, the same numbers on every computer */
static unsigned seed = 12345;

static int Random(int n) {
  seed = seed * 1103515245 + 12345;
  return (seed >> 16) % n;
}

static void TestMany(void) {
  start("1000 random inserts/removes, sizes 1..20, other mallocs between");
  Vector v;
  VectorInit(&v);

  // what the vector should hold: element i is size[i] bytes: tag[i], tag[i] + 1, ...
  static int size[1000];
  static unsigned char tag[1000];
  int n = 0;
  void *others[1000];   // other mallocs: they make realloc MOVE the block
  bool ok = true;

  for (int step = 0; step < 1000; step++) {
    if (n > 0 && Random(3) == 0) {
      int index = Random(n);
      VectorRemove(&v, index);
      memmove(&size[index], &size[index + 1], (n - index - 1) * sizeof(int));
      memmove(&tag[index], &tag[index + 1], n - index - 1);
      n--;
    } else {
      int index = Random(n + 1);
      unsigned char bytes[20];
      int s = 1 + Random(20);
      unsigned char t = Random(256);
      for (int k = 0; k < s; k++) bytes[k] = t + k;
      VectorInsert(&v, index, s, bytes, NULL);
      memmove(&size[index + 1], &size[index], (n - index) * sizeof(int));
      memmove(&tag[index + 1], &tag[index], n - index);
      size[index] = s;
      tag[index] = t;
      n++;
    }
    others[step] = malloc(16);
  }
  for (int i = 0; i < n && ok; i++) {
    unsigned char bytes[20];
    for (int k = 0; k < size[i]; k++) bytes[k] = tag[i] + k;
    ok = Holds(&v, i, size[i], bytes);
  }
  if (ok) {
    int calls = 0;
    VectorMap(&v, Count, &calls);
    if (calls != n) {
      sprintf(why, "VectorMap called map_fn %d times, the vector has %d elements", calls, n);
      ok = false;
    }
  }
  finish(ok);
  VectorDestroy(&v);
  for (int i = 0; i < 1000; i++) free(others[i]);
}

int main(void) {
  setbuf(stdout, NULL);   // print right away, so a crash shows which test it was
  printf("\n=== packed_vector ===\n");
  printf("  sizeof(int) = %d, sizeof(FreeFn) = %d, sizeof(double) = %d, sizeof(char *) = %d\n\n",
         (int)sizeof(int), (int)sizeof(FreeFn), (int)sizeof(double), (int)sizeof(char *));

  TestEmpty();
  TestIntsAtEnd();
  TestIntsMiddle();
  TestIntsFront();
  TestIntsRemove();
  TestMixed();
  TestMixedMiddle();
  TestPacked();
  TestFreeFn();
  TestRotateInts();
  TestRotateMixed();
  TestMap();
  TestMany();

  printf("\n  %d / %d passed\n", passed, total);
  return 0;
}
