#include "gl/containers/list.h"

#include <stdlib.h>

#include "gl/lunit/lunit.h"

TEST(Empty) {
  List list;
  ListInit(&list, /*elem_size=*/1, /*free_fn*/NULL);
  ASSERT_INT_EQ(list.size, 0);
  ListDispose(&list);
}

TEST(Integers) {
  List list;
  ListInit(&list, sizeof(int), /*free_fn=*/NULL);
  int num_elems = 10;
  for (int i = 0; i < num_elems; ++i) {
    ListAdd(&list, &i);
  }
  ASSERT_INT_EQ(list.size, num_elems);
  for (int i = 0; i < num_elems; ++i) {
    ASSERT_INT_EQ(*(int*)ListGet(&list, i), i);
  }
  ListDispose(&list);
}

void StrFree(void* elem) {
  free(*(char**)elem);
}

TEST(Strings) {
  List list;
  ListInit(&list, sizeof(char*), StrFree);
  int num_elems = 2;
  char* elems[] = {"foo", "bar"};
  for (int i = 0; i < num_elems; ++i) {
    char* elem = strdup(elems[i]);
    ListAdd(&list, &elem);
  }
  ASSERT_INT_EQ(list.size, num_elems);
  for (int i = 0; i < num_elems; ++i) {
    ASSERT_STR_EQ(*(char**)ListGet(&list, i), elems[i]);
  }
  ListDispose(&list);
}

int* elems_freed = NULL;

void ElemFree(void* elem) {
  ++elems_freed[*(int*)elem];
}

TEST(Free) {
  List list;
  ListInit(&list, sizeof(int), ElemFree);
  int num_elems = 10;
  for (int i = 0; i < num_elems; ++i) {
    ListAdd(&list, &i);
  }
  ASSERT_INT_EQ(list.size, num_elems);
  elems_freed = malloc(num_elems * sizeof(int));
  memset(elems_freed, 0, num_elems * sizeof(int));
  ListDispose(&list);
  for (int i = 0; i < num_elems; ++i) {
    ASSERT_INT_EQ(elems_freed[i], 1);
  }
}

int main(int argc, char* argv[]) {
  LUnitOpts opts;
  LUnitOptsInit(&opts, argc, argv);
  RUN_TEST(Empty, &opts);
  RUN_TEST(Integers, &opts);
  RUN_TEST(Strings, &opts);
  RUN_TEST(Free, &opts);
  return 0;
}
