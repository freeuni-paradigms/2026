#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sorted_set.h"

int compareInt(const void *a, const void *b) {
  int x = *(int *)a;
  int y = *(int *)b;
  return x - y;
}

int compareStr(const void *a, const void *b) {
  char *s1 = *(char **)a;
  char *s2 = *(char **)b;
  return strcmp(s1, s2);
}

void testInts() {
  sortedset s;
  SetNew(&s, sizeof(int), compareInt);

  int num = 42;
  assert(SetSearch(&s, &num) == NULL);   // empty set
  assert(SetAdd(&s, &num) == true);      // first add works
  assert(SetAdd(&s, &num) == false);     // duplicate is rejected

  int *found = SetSearch(&s, &num);
  assert(found != NULL);
  assert(*found == 42);

  int missing = 7;
  assert(SetSearch(&s, &missing) == NULL);

  printf("testInts passed\n");
}

void testStrings() {
  sortedset s;
  SetNew(&s, sizeof(char *), compareStr);

  char *a = strdup("pear");
  char *b = strdup("apple");
  char *c = strdup("a-rather-long-string");
  assert(SetAdd(&s, &a) == true);        // pass the ADDRESS of the pointer
  assert(SetAdd(&s, &b) == true);
  assert(SetAdd(&s, &c) == true);

  char *key = "apple";
  char **found = SetSearch(&s, &key);
  assert(found != NULL);
  assert(strcmp(*found, "apple") == 0);

  key = "fig";
  assert(SetSearch(&s, &key) == NULL);

  printf("testStrings passed\n");
}

void testGrowth() {
  sortedset s;
  SetNew(&s, sizeof(int), compareInt);

  // start capacity is 4, so this forces the set to grow several times
  for (int i = 0; i < 100; i++) {
    assert(SetAdd(&s, &i) == true);
  }

  for (int i = 0; i < 100; i++) {
    int *found = SetSearch(&s, &i);
    assert(found != NULL);
    assert(*found == i);
  }

  printf("testGrowth passed\n");
}

void testMixedOrder() {
  sortedset s;
  SetNew(&s, sizeof(int), compareInt);

  int nums[] = {50, 20, 80, 10, 30, 70, 90, 20, 80};
  int count = 9;
  int added = 0;

  for (int i = 0; i < count; i++) {
    if (SetAdd(&s, &nums[i])) {
      added++;
    }
  }
  assert(added == 7);                    // 20 and 80 were duplicates

  for (int i = 0; i < count; i++) {
    assert(SetSearch(&s, &nums[i]) != NULL);
  }

  int missing = 60;
  assert(SetSearch(&s, &missing) == NULL);

  printf("testMixedOrder passed\n");
}

int main() {
  testInts();
  testStrings();
  testGrowth();
  testMixedOrder();
  printf("All tests passed!\n");
  return 0;
}
