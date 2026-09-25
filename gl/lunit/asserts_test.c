#include "gl/lunit/asserts.h"

#include <limits.h>
#include <stdlib.h>

#include "gl/lunit/lunit.h"

void TestInts() {
  Test* test = malloc(sizeof(Test));
  for (int i = -100; i <= 100; ++i) {
    ASSERT_INT_EQ(i, i);
  }
  free(test);
}

void TestChars() {
  Test* test = malloc(sizeof(Test));
  for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
    ASSERT_CHAR_EQ(i, i);
  }
  free(test);
}

void TestStrings() {
  Test* test = malloc(sizeof(Test));  
  ASSERT_STR_EQ("", "");
  ASSERT_STR_EQ("a", "a");
  ASSERT_STR_EQ("abc", "abc");
  ASSERT_STR_NEQ("", "a");
  ASSERT_STR_NEQ("a", "");
  ASSERT_STR_NEQ("a", "b");
  ASSERT_STR_NEQ("a", "abc");
  ASSERT_STR_NEQ("abc", "a");
  ASSERT_STR_STARTS_WITH("abc", "");
  ASSERT_STR_STARTS_WITH("abc", "a");
  ASSERT_STR_STARTS_WITH("abc", "ab");
  ASSERT_STR_STARTS_WITH("abc", "abc");
  free(test);
}

void TestMems() {
  Test* test = malloc(sizeof(Test));  
  ASSERT_MEM_EQ("", "", 1);
  ASSERT_MEM_EQ("a", "a", 2);
  ASSERT_MEM_EQ("abc", "abc", 3);
  free(test);
}

int main(int argc, char* argv[]) {
  TestInts();
  TestChars();
  TestStrings();
  TestMems();
  return 0;
}
