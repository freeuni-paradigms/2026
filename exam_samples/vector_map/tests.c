#include "map.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

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
      printf("%s:%d Assertion failed: expected %s got %s\n", __FILE__, __LINE__, expected, actual); \
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

int CmpInt(void* a, void* b) {
  return *(int*)a - *(int*)b;
}

bool Empty() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(int), CmpInt, NULL, NULL);
  MapDestroy(&m);
  return true;
}

// Ints

bool IntToInt() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(int), CmpInt, NULL, NULL);
  int x = 5;
  int y = 7;
  MapSet(&m, &x, &y);
  MapSet(&m, &y, &x);
  int k = 5;
  ASSERT_INT_EQ(7, *(int*)MapGet(&m, &k));
  k = 7;
  ASSERT_INT_EQ(5, *(int*)MapGet(&m, &k));
  MapDestroy(&m);
  return true;
}

bool IntToInt_Many() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(int), CmpInt, NULL, NULL);
  for (int i = 0; i < 100; ++i) {
	int j = 100 - i;
	MapSet(&m, &i, &j);
  }
  for (int i = 99; i >= 0; --i) {
	ASSERT_INT_EQ(100 - i, *(int*)MapGet(&m, &i));
  }
  MapDestroy(&m);
  return true;
}

bool IntToInt_Many_Overwrite() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(int), CmpInt, NULL, NULL);
  for (int i = 0; i < 100; ++i) {
	int j = 100 - i;
	MapSet(&m, &i, &j);
  }
  for (int i = 99; i >= 0; --i) {
	ASSERT_INT_EQ(100 - i, *(int*)MapGet(&m, &i));
  }
  // Overwrite
  for (int i = 0; i < 100; ++i) {
	MapSet(&m, &i, &i);
  }
  for (int i = 0; i < 100; i++) {
	ASSERT_INT_EQ(i, *(int*)MapGet(&m, &i));
  }
  MapDestroy(&m);
  return true;
}

bool IntToInt_Many_GetUnknown() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(int), CmpInt, NULL, NULL);
  for (int i = 0; i < 100; ++i) {
	int j = 100 - i;
	MapSet(&m, &i, &j);
  }
  for (int i = 100; i < 200; i++) {
	ASSERT_TRUE(NULL == MapGet(&m, &i));
  }
  MapDestroy(&m);
  return true;
}

bool IntToInt_Many_Remove() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(int), CmpInt, NULL, NULL);
  for (int i = 0; i < 100; ++i) {
	MapSet(&m, &i, &i);
  }
  for (int i = 0; i < 50; ++i) {
	MapRemove(&m, &i);
  }
  for (int i = 0; i < 50; i++) {
	ASSERT_TRUE(NULL == MapGet(&m, &i));
  }
  for (int i = 50; i < 100; i++) {
	ASSERT_INT_EQ(i, *(int*)MapGet(&m, &i));
  }
  MapDestroy(&m);
  return true;
}

void Mapping(void* key, void* value, void* aux) {
  int* values = aux;
  values[*(int*)key] = *(int*)value;
}

bool IntToInt_Many_Map() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(int), CmpInt, NULL, NULL);
  for (int i = 0; i < 100; ++i) {
	int j = 100 - i;
	MapSet(&m, &i, &j);
  }
  int values[100];
  MapMap(&m, Mapping, values);
  for (int i = 0; i < 100; ++i) {
	ASSERT_INT_EQ(100 - i, values[i]);
  }
  MapDestroy(&m);
  return true;
}

// Strings

void StrFree(void* ptr) {
  free(*(char**)ptr);
}

bool IntToString() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(char*), CmpInt, NULL, StrFree);
  int a = 1;
  int b = 2;
  int c = 3;
  char* x = strdup("one");
  char* y = strdup("two");
  char* z = strdup("three");
  MapSet(&m, &a, &x);
  MapSet(&m, &b, &y);
  MapSet(&m, &c, &z);
  int k = 1;
  ASSERT_STR_EQ("one", *(char**)MapGet(&m, &k));
  k = 2;
  ASSERT_STR_EQ("two", *(char**)MapGet(&m, &k));
  k = 3;
  ASSERT_STR_EQ("three", *(char**)MapGet(&m, &k));
  MapDestroy(&m);
  return true;
}

bool IntToString_Many() {
  Map m;
  MapInit(&m, sizeof(int), sizeof(char*), CmpInt, NULL, StrFree);
  for (int i = 0; i < 100; ++i) {
	char* tmp = malloc(10);
	assert(tmp != NULL);
	sprintf(tmp, "%d", i * 10);
	MapSet(&m, &i, &tmp);
  }
  for (int i = 99; i > 0; --i) {
	char tmp[10];
	sprintf(tmp, "%d", i * 10);
	ASSERT_STR_EQ(tmp, *(char**)MapGet(&m, &i));
  }
  for (int i = 50; i < 100; ++i) {
	MapRemove(&m, &i);
  }
  for (int i = 0; i < 50; ++i) {
	char tmp[10];
	sprintf(tmp, "%d", i * 10);
	ASSERT_STR_EQ(tmp, *(char**)MapGet(&m, &i));
  }
  MapDestroy(&m);
  return true;
}

int CmpStr(void* a, void* b) {
  return strcmp(*(char**)a, *(char**)b);
}

bool StringToString_Many() {
  Map m;
  MapInit(&m, sizeof(char*), sizeof(char*), CmpStr, StrFree, StrFree);
  for (int i = 0; i < 100; ++i) {
	char* key = malloc(10);
	char* value = malloc(10);
	assert(key != NULL);
	assert(value != NULL);
	sprintf(key, "%d", i);
	sprintf(value, "%d", i * 10);
	MapSet(&m, &key, &value);
  }
  for (int i = 99; i > 0; --i) {
	char* key = malloc(10);
	sprintf(key, "%d", i);
	char value[10];
	sprintf(value, "%d", i * 10);
	ASSERT_STR_EQ(value, *(char**)MapGet(&m, &key));
	free(key);
  }
  MapDestroy(&m);
  return true;
}

int main(int argc, char** argv) {
  TEST(Empty);
  TEST(IntToInt);
  TEST(IntToInt_Many);
  TEST(IntToInt_Many_Overwrite);
  TEST(IntToInt_Many_GetUnknown);
  TEST(IntToInt_Many_Remove);
  TEST(IntToInt_Many_Map);
  TEST(IntToString);
  TEST(IntToString_Many);
  TEST(StringToString_Many);
  return 0;
}
