#include "sol.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_TRUE(a) {						\
    bool actual = (a);							\
    if (!actual) {							\
      printf("%s:%d Assertion failed: expected true got false\n", __FILE__, __LINE__); \
      return false;							\
    }									\
  }

#define ASSERT_FALSE(a) {						\
    bool actual = (a);							\
    if (actual) {							\
      printf("%s:%d Assertion failed: expected false got true\n", __FILE__, __LINE__); \
      return false;							\
    }									\
  }

#define ASSERT_DOUBLE_EQ(a, b) {						\
    double expected = (a);							\
    double actual = (b);							\
    if (abs(expected - actual) > 0.000001) {										\
      printf("%s:%d Assertion failed: expected %f got %f\n", __FILE__, __LINE__, expected, actual); \
      return false;							\
    }									\
  }

#define ASSERT_INT_EQ(a, b) {						\
    int expected = (a);							\
    int actual = (b);							\
    if (expected != actual) {						\
      printf("%s:%d Assertion failed: expected %d got %d\n", __FILE__, __LINE__, expected, actual); \
      return false;							\
    }									\
  }

#define ASSERT_STR_EQ(a, b) {						\
    char* expected = (a);						\
    char* actual = (b);							\
    if (strcmp(expected, actual) != 0) {				\
      printf("%s:%d Assertion failed: expected \"%s\" got \"%s\"\n", __FILE__, __LINE__, expected, actual); \
      return false;							\
    }									\
  }

#define ASSERT_MEM_EQ(a, b, size) {					\
    void* expected = (a);						\
    void* actual = (b);							\
    if (memcmp(expected, actual, size) != 0) {								\
      printf("%s:%d Assertion failed: memory does not match\n", __FILE__, __LINE__); \
      return false;							\
    }									\
  }

#define TEST(t) {				\
    printf("-- TESTING: %s\n", #t);		\
    if (t()) {					\
      printf("TEST %s: SUCCEEDED\n", #t);	\
    }						\
    else {					\
      printf("TEST %s: FAILED\n", #t);		\
    }						\
    printf("####\n\n");				\
  }

int IntCmp(void* a, void* b) {
  return *(int*)a - *(int*)b;
}

int StrCmp(void* a, void* b) {
  return strcmp(*(char**)a, *(char**)b);
}

void StrFree(void* a) {
  free(*(char**)a);
}

// Integers

bool Empty() {
  Vector v;
  VectorInit(&v);
  VectorDestroy(&v);
  return true;
}

bool Ints() {
  Vector v;
  VectorInit(&v);
  int x = 1;
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  x = 2;
  VectorInsert(&v, 1, sizeof(int), &x, NULL);
  x = 3;
  VectorInsert(&v, 2, sizeof(int), &x, NULL);
  int size;
  int* y;
  y = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(1, *y);
  y = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(2, *y);
  y = VectorGet(&v, 2, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(3, *y);
  VectorDestroy(&v);
  return true;
}

bool Ints_Overwrite() {
  Vector v;
  VectorInit(&v);
  int x = 1;
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  x = 2;
  VectorInsert(&v, 1, sizeof(int), &x, NULL);
  x = 3;
  VectorInsert(&v, 2, sizeof(int), &x, NULL);
  int size;
  int* y;
  y = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(1, *y);
  y = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(2, *y);
  y = VectorGet(&v, 2, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(3, *y);
  // Overwrite
  x = 4;
  VectorRemove(&v, 0);
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  x = 5;
  VectorRemove(&v, 1);
  VectorInsert(&v, 1, sizeof(int), &x, NULL);
  x = 6;
  VectorRemove(&v, 2);
  VectorInsert(&v, 2, sizeof(int), &x, NULL);
  // Checks
  y = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(4, *y);
  y = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(5, *y);
  y = VectorGet(&v, 2, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(6, *y);
  VectorDestroy(&v);
  return true;
}

bool Ints_Rotate() {
  Vector v;
  VectorInit(&v);
  for (int i = 0; i < 50; ++i) {
	VectorInsert(&v, i, sizeof(int), &i, NULL);
  }
  for (int i = 0; i < 50; ++i) {
	int size;
	int* k = VectorGet(&v, i, &size);
	ASSERT_INT_EQ(sizeof(int), size);
	ASSERT_INT_EQ(i, *k);
  }
  VectorRotate(&v, 25);
  for (int i = 0; i < 25; ++i) {
	int size;
	int* k = VectorGet(&v, i, &size);
	ASSERT_INT_EQ(sizeof(int), size);
	ASSERT_INT_EQ(25 + i, *k);
  }
  for (int i = 25; i < 50; ++i) {
	int size;
	int* k = VectorGet(&v, i, &size);
	ASSERT_INT_EQ(sizeof(int), size);
	ASSERT_INT_EQ(i - 25, *k);
  }
  VectorDestroy(&v);
  return true;
}

typedef struct {
  int count;
  int sum;
} SumData;

void IntSum(int index, int size, void* elem, void* aux) {
  SumData* d = aux;
  d->count++;
  d->sum += *(int*)elem;
}

bool Ints_Sum_With_Map() {
  Vector v;
  VectorInit(&v);
  for (int i = 0; i < 100; ++i) {
	int k = 10 * i;
	VectorInsert(&v, i, sizeof(int), &k, NULL);
  }
  SumData d;
  d.count = 0;
  d.sum = 0;
  VectorMap(&v, IntSum, &d);
  ASSERT_INT_EQ(100, d.count);
  ASSERT_INT_EQ(10 * 99 * 100 / 2, d.sum);
  VectorDestroy(&v);
  return true;
}

bool Ints_InsertGet() {
  Vector v;
  VectorInit(&v);
  for (int i = 100; i > 0; --i) {
	int k = 20 * i;
	VectorInsert(&v, 100 - i, sizeof(int), &k, NULL);
  }
  for (int i = 0; i < 100; ++i) {
	int size;
	int* k = VectorGet(&v, i, &size);
	ASSERT_INT_EQ(sizeof(int), size);
	ASSERT_INT_EQ(20 * (100 - i), *k);
  }
  VectorDestroy(&v);
  return true;
}

bool Ints_InsertFront() {
  Vector v;
  VectorInit(&v);
  for (int i = 0; i < 100; ++i) {
	VectorInsert(&v, 0, sizeof(int), &i, NULL);
  }
  for (int i = 0; i < 100; ++i) {
	int size;
	int* k = VectorGet(&v, i, &size);
	ASSERT_INT_EQ(sizeof(int), size);
	ASSERT_INT_EQ(99 - i, *k);
  }
  VectorDestroy(&v);
  return true;
}

bool Ints_InsertGet_Remove() {
  Vector v;
  VectorInit(&v);
  for (int i = 0; i < 100; ++i) {
	VectorInsert(&v, 0, sizeof(int), &i, NULL);
  }
  for (int i = 49; i >= 0; --i) {
	VectorRemove(&v, i);
  }
  for (int i = 0; i < 50; ++i) {
	int size;
	int* k = VectorGet(&v, i, &size);
	ASSERT_INT_EQ(sizeof(int), size);
	ASSERT_INT_EQ(49 - i, *k);
  }
  VectorDestroy(&v);
  return true;
}

bool Mixed() {
  Vector v;
  VectorInit(&v);
  int x = 5;
  char* y = "freeuni";
  double z = 3.4;
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  VectorInsert(&v, 1, strlen(y), y, NULL);
  VectorInsert(&v, 2, sizeof(double), &z, NULL);
  int size;
  int* a = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(sizeof(int), size);
  ASSERT_INT_EQ(5, *a);
  char* b = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(strlen(y), size);
  ASSERT_TRUE(0 == strncmp(y, b, strlen(y)));
  double* c = VectorGet(&v, 2, &size);
  ASSERT_DOUBLE_EQ(3.4, *c);
  ASSERT_MEM_EQ(&z, c, sizeof(double));
  VectorDestroy(&v);
  return true;
}

void MixedSum(int index, int size, void* elem, void* aux) {
  SumData* d = aux;
  d->count++;
  if (size == 4) {
	d->sum += *(int*)elem;
  } else {
	d->sum += size;
  }
}

bool Mixed_Map() {
  Vector v;
  VectorInit(&v);
  int x = 5;
  char* y = "hi";
  int z = 10;
  char* w = "uni";
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  VectorInsert(&v, 1, strlen(y), y, NULL);
  VectorInsert(&v, 2, sizeof(int), &z, NULL);
  VectorInsert(&v, 3, strlen(w), w, NULL);
  SumData d;
  d.count = 0;
  d.sum = 0;
  VectorMap(&v, MixedSum, &d);
  ASSERT_INT_EQ(4, d.count);
  ASSERT_INT_EQ(5 + 2 + 10 + 3, d.sum);
  VectorDestroy(&v);
  return true;
}

bool Mixed_Rotate() {
  Vector v;
  VectorInit(&v);
  int x = 5;
  char* y = "a";
  int z = 10;
  char* w = "bcdefg";
  VectorInsert(&v, 0, sizeof(int), &x, NULL);
  VectorInsert(&v, 1, strlen(y), y, NULL);
  VectorInsert(&v, 2, sizeof(int), &z, NULL);
  VectorInsert(&v, 3, strlen(w), w, NULL);
  VectorRotate(&v, 1);
  int size;
  char* a = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(1, size);
  ASSERT_MEM_EQ("a", a, 1);
  int* b = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(4, size);
  ASSERT_INT_EQ(10, *b);
  char* c = VectorGet(&v, 2, &size);
  ASSERT_INT_EQ(6, size);
  ASSERT_MEM_EQ("bcdefg", c, 6);
  int* d = VectorGet(&v, 3, &size);
  ASSERT_INT_EQ(4, size);
  ASSERT_INT_EQ(5, *d);
  VectorDestroy(&v);
  return true;
}

void FreeStr(void* addr) {
  free(*(char**)addr);
}

bool Strings_WithFree_InsertGet() {
  Vector v;
  VectorInit(&v);
  char* x = strdup("free");
  char* y = strdup("uni");
  char* z = strdup("abcdefgh");;
  char* w = strdup("0123456789");
  VectorInsert(&v, 0, sizeof(char*), &x, FreeStr);
  VectorInsert(&v, 1, sizeof(char*), &y, FreeStr);
  VectorInsert(&v, 2, sizeof(char*), &z, FreeStr);
  VectorInsert(&v, 3, sizeof(char*), &w, FreeStr);
  int size;
  char** a = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("free", *a);
  char** b = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("uni", *b);
  char** c = VectorGet(&v, 2, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("abcdefgh", *c);
  char** d = VectorGet(&v, 3, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("0123456789", *d);
  VectorDestroy(&v);
  return true;
}

bool Strings_WithFree_Rotate() {
  Vector v;
  VectorInit(&v);
  char* x = strdup("free");
  char* y = strdup("uni");
  char* z = strdup("abcdefgh");;
  char* w = strdup("0123456789");
  VectorInsert(&v, 0, sizeof(char*), &x, FreeStr);
  VectorInsert(&v, 1, sizeof(char*), &y, FreeStr);
  VectorInsert(&v, 2, sizeof(char*), &z, FreeStr);
  VectorInsert(&v, 3, sizeof(char*), &w, FreeStr);
  VectorRotate(&v, 2);
  int size;
  char** a = VectorGet(&v, 2, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("free", *a);
  char** b = VectorGet(&v, 3, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("uni", *b);
  char** c = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("abcdefgh", *c);
  char** d = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("0123456789", *d);
  VectorDestroy(&v);
  return true;
}

bool FreeFn_Mixed() {
  Vector v;
  VectorInit(&v);
  char* x = strdup("free");
  char* y = "uni";
  char* z = strdup("abcdefgh");;
  char* w = "0123456789";
  VectorInsert(&v, 0, sizeof(char*), &x, FreeStr);
  VectorInsert(&v, 1, strlen(y), y, NULL);
  VectorInsert(&v, 2, sizeof(char*), &z, FreeStr);
  VectorInsert(&v, 3, strlen(w), w, NULL);
  int size;
  char** a = VectorGet(&v, 0, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("free", *a);
  char* b = VectorGet(&v, 1, &size);
  ASSERT_INT_EQ(strlen("uni"), size);
  ASSERT_MEM_EQ("uni", b, 3);
  char** c = VectorGet(&v, 2, &size);
  ASSERT_INT_EQ(sizeof(char*), size);
  ASSERT_STR_EQ("abcdefgh", *c);
  char* d = VectorGet(&v, 3, &size);
  ASSERT_INT_EQ(strlen("0123456789"), size);
  ASSERT_MEM_EQ("0123456789", d, 10);
  VectorDestroy(&v);
  return true;
}

int main(int argc, char** argv) {
  TEST(Empty);
  TEST(Ints);
  TEST(Ints_InsertGet);
  TEST(Ints_InsertFront);
  TEST(Ints_InsertGet_Remove);
  TEST(Ints_Overwrite);
  TEST(Ints_Rotate);
  TEST(Ints_Sum_With_Map);
  TEST(Mixed);
  TEST(Mixed_Map);
  TEST(Mixed_Rotate);
  TEST(Strings_WithFree_InsertGet);
  TEST(Strings_WithFree_Rotate);
  TEST(FreeFn_Mixed);
  return 0;
}
