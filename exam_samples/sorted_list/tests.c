#include "sorted_list.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr)                                                           \
  {                                                                            \
    if (!(expr)) {                                                             \
      printf("%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #expr);      \
      return false;                                                            \
    }                                                                          \
  }

#define TEST(name) bool Test##name()

#define RUN_TEST(name)                                                         \
  printf("++ RUNNING TEST: %s\n", #name);                                      \
  if (Test##name()) {                                                          \
    printf("-- TEST %s: SUCCEEDED\n", #name);                                  \
  } else {                                                                     \
    printf("-- TEST %s: FAILED\n", #name);                                     \
  }                                                                            \
  printf("\n");

int IntCmp(const void *a, const void *b) {
  return *(const int *)a - *(const int *)b;
}

int StrCmpDyn(const void *a, const void *b) {
  return strcmp(*(const char **)a, *(const char **)b);
}

void StrFree(void *ptr) { free(*(char **)ptr); }

typedef struct {
  int x;
  int y;
} Point;

void PointFree(void *ptr) { free(*(Point **)ptr); }

Point PointNew(int x, int y) {
  Point p;
  p.x = x;
  p.y = y;
  return p;
}

Point *PointNewDyn(int x, int y) {
  Point *p = malloc(sizeof(Point));
  assert(p != NULL);
  p->x = x;
  p->y = y;
  return p;
}

int PointCmp(const void *a, const void *b) {
  const Point *p = a;
  const Point *q = b;
  if (p->x != q->x) {
    return p->x - q->x;
  }
  return p->y - q->y;
}

int PointCmpDyn(const void *a, const void *b) {
  return PointCmp(*(const Point **)a, *(const Point **)b);
}

TEST(Integers_Empty) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(int), IntCmp, NULL, NULL);
  for (int i = 0; i < 100; ++i) {
    ASSERT(SortedListGet(&l, &i) == NULL);
    ASSERT(SortedListRemove(&l, &i) == false);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(Integers_SingleKey) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(int), IntCmp, NULL, NULL);
  int x = 5;
  int y = 7;
  ASSERT(SortedListGet(&l, &x) == NULL);
  SortedListAdd(&l, &x, &y);
  ASSERT(*(const int *)SortedListGet(&l, &x) == 7);
  y = 10;
  SortedListAdd(&l, &x, &y);
  ASSERT(*(const int *)SortedListGet(&l, &x) == 10);
  ASSERT(SortedListRemove(&l, &x));
  ASSERT(SortedListGet(&l, &x) == NULL);
  ASSERT(SortedListRemove(&l, &x) == false);
  SortedListDestroy(&l);
  return true;
}

TEST(Integers_AddGetRemove) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(int), IntCmp, NULL, NULL);
  for (int i = 99; i >= 0; --i) {
    SortedListAdd(&l, &i, &i);
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(*(const int *)SortedListGet(&l, &i) == i);
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(SortedListRemove(&l, &i));
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(SortedListGet(&l, &i) == NULL);
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(SortedListRemove(&l, &i) == false);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(Integers_Duplicates) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(int), IntCmp, NULL, NULL);
  for (int i = 99; i >= 0; --i) {
    SortedListAdd(&l, &i, &i);
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(*(const int *)SortedListGet(&l, &i) == i);
  }
  for (int i = 0; i < 100; ++i) {
    int j = 100 - i;
    SortedListAdd(&l, &i, &j);
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(*(const int *)SortedListGet(&l, &i) == 100 - i);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(IntegerPoint_SingleKey) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(Point), IntCmp, NULL, NULL);
  int x = 5;
  Point p = PointNew(1, 2);
  SortedListAdd(&l, &x, &p);
  ASSERT(PointCmp(SortedListGet(&l, &x), &p) == 0);
  ASSERT(SortedListRemove(&l, &x));
  ASSERT(SortedListGet(&l, &x) == NULL);
  SortedListAdd(&l, &x, &p);
  SortedListDestroy(&l);
  return true;
}

TEST(IntegerPoint_AddGetRemove) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(Point), IntCmp, NULL, NULL);
  for (int i = 99; i >= 0; --i) {
    Point p = PointNew(i, 100 - i);
    SortedListAdd(&l, &i, &p);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(i, 100 - i);
    ASSERT(PointCmp(SortedListGet(&l, &i), &p) == 0);
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(SortedListRemove(&l, &i));
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(SortedListGet(&l, &i) == NULL);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(IntegerPoint_Duplicates) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(Point), IntCmp, NULL, NULL);
  for (int i = 99; i >= 0; --i) {
    Point p = PointNew(i, 100 - i);
    SortedListAdd(&l, &i, &p);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(100 - i, i);
    SortedListAdd(&l, &i, &p);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(100 - i, i);
    ASSERT(PointCmp(SortedListGet(&l, &i), &p) == 0);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(IntegerPoint_ValueDynamicallyAllocated) {
  SortedList l;
  SortedListInit(&l, sizeof(int), sizeof(Point *), IntCmp, NULL, PointFree);
  for (int i = 0; i < 100; ++i) {
    Point *p = PointNewDyn(i, 100 - i);
    SortedListAdd(&l, &i, &p);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(i, 100 - i);
    ASSERT(PointCmp(*(const Point **)SortedListGet(&l, &i), &p) == 0);
  }
  for (int i = 0; i < 50; ++i) {
    ASSERT(SortedListRemove(&l, &i));
    ASSERT(SortedListGet(&l, &i) == NULL);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(PointInteger_SingleKey) {
  SortedList l;
  SortedListInit(&l, sizeof(Point), sizeof(int), IntCmp, NULL, NULL);
  int x = 5;
  Point p = PointNew(1, 2);
  SortedListAdd(&l, &p, &x);
  ASSERT(*(const int *)SortedListGet(&l, &p) == x);
  ASSERT(SortedListRemove(&l, &p));
  ASSERT(SortedListGet(&l, &p) == NULL);
  SortedListAdd(&l, &p, &x);
  SortedListDestroy(&l);
  return true;
}

TEST(PointInteger_AddGetRemove) {
  SortedList l;
  SortedListInit(&l, sizeof(Point), sizeof(int), IntCmp, NULL, NULL);
  for (int i = 99; i >= 0; --i) {
    Point p = PointNew(i, 100 - i);
    SortedListAdd(&l, &p, &i);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(i, 100 - i);
    ASSERT(*(const int *)SortedListGet(&l, &p) == i);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(i, 100 - i);
    ASSERT(SortedListRemove(&l, &p));
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(i, 100 - i);
    ASSERT(SortedListGet(&l, &p) == NULL);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(PointInteger_Duplicates) {
  SortedList l;
  SortedListInit(&l, sizeof(Point), sizeof(int), IntCmp, NULL, NULL);
  for (int i = 99; i >= 0; --i) {
    Point p = PointNew(i, 100 - i);
    SortedListAdd(&l, &p, &i);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(100 - i, i);
    SortedListAdd(&l, &p, &i);
  }
  for (int i = 0; i < 100; ++i) {
    Point p = PointNew(100 - i, i);
    ASSERT(*(const int *)SortedListGet(&l, &p) == i);
  }
  SortedListDestroy(&l);
  return true;
}

TEST(PointInteger_KeyDynamicallyAllocated) {
  SortedList l;
  SortedListInit(&l, sizeof(Point *), sizeof(int), PointCmpDyn, PointFree,
                 NULL);
  for (int i = 0; i < 100; ++i) {
    Point *p = PointNewDyn(i, 100 - i);
    SortedListAdd(&l, &p, &i);
  }
  for (int i = 0; i < 100; ++i) {
    Point *p = PointNewDyn(i, 100 - i);
    ASSERT(*(const int *)SortedListGet(&l, &p) == i);
    free(p);
  }
  for (int i = 0; i < 50; ++i) {
    Point *p = PointNewDyn(i, 100 - i);
    ASSERT(SortedListRemove(&l, &p));
    ASSERT(SortedListGet(&l, &p) == NULL);
    free(p);
  }
  SortedListDestroy(&l);
  return true;
}

char *IntToStr(int n) {
  char *ret = malloc(5);
  assert(ret != NULL);
  int len = 0;
  while (n > 0) {
    ret[len] = n % 10 + 1;
    len++;
    n /= 10;
  }
  ret[len] = '\0';
  for (int i = 0; 2 * i < n; ++i) {
    char tmp = ret[i];
    ret[i] = ret[len - i - 1];
    ret[len - i - 1] = tmp;
  }
  return ret;
}

TEST(StringPoint_BothKeyValueDynamic) {
  SortedList l;
  SortedListInit(&l, sizeof(char *), sizeof(Point *), StrCmpDyn, StrFree,
                 PointFree);
  for (int i = 99; i >= 0; --i) {
    char *i_str = IntToStr(i);
    Point *p = PointNewDyn(i, 100 - i);
    SortedListAdd(&l, &i_str, &p);
  }
  for (int i = 0; i < 100; ++i) {
    char *i_str = IntToStr(i);
    Point *p = PointNewDyn(100 - i, i);
    SortedListAdd(&l, &i_str, &p);
  }
  for (int i = 0; i < 100; ++i) {
    char *i_str = IntToStr(i);
    Point p = PointNew(100 - i, i);
    const Point *q = SortedListGet(&l, &i_str);
    ASSERT(PointCmp(*(const Point **)SortedListGet(&l, &i_str), &p) == 0);
    free(i_str);
  }
  for (int i = 0; i < 50; ++i) {
    char *i_str = IntToStr(i);
    ASSERT(SortedListRemove(&l, &i_str));
    ASSERT(SortedListGet(&l, &i_str) == NULL);
    free(i_str);
  }
  SortedListDestroy(&l);
  return true;
}

int main(int argc, char **argv) {
  RUN_TEST(Integers_Empty);
  RUN_TEST(Integers_SingleKey);
  RUN_TEST(Integers_AddGetRemove);
  RUN_TEST(Integers_Duplicates);
  RUN_TEST(IntegerPoint_SingleKey);
  RUN_TEST(IntegerPoint_AddGetRemove);
  RUN_TEST(IntegerPoint_Duplicates);
  RUN_TEST(IntegerPoint_ValueDynamicallyAllocated);
  RUN_TEST(PointInteger_SingleKey);
  RUN_TEST(PointInteger_AddGetRemove);
  RUN_TEST(PointInteger_Duplicates);
  RUN_TEST(PointInteger_KeyDynamicallyAllocated);
  RUN_TEST(StringPoint_BothKeyValueDynamic);
  return 0;
}
