#include "gl/lunit/cmp.h"

#include <limits.h>

#include "gl/logging/logging.h"

void TestInts() {
  for (int i = -100; i <= 100; ++i) {
    CHECK(IntEq(i, i));
    CHECK(!IntEq(i, i + 1));    
  }
}

void TestChars() {
  for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
    CHECK(CharEq(i, i));
    CHECK(!CharEq(i, i + 1));
  }
}

void TestStrings() {
  CHECK(StrEq("", ""));
  CHECK(StrEq("a", "a"));
  CHECK(StrEq("abc", "abc"));
  CHECK(StrNEq("", "a"));
  CHECK(StrNEq("a", ""));
  CHECK(StrNEq("a", "b"));
  CHECK(StrNEq("a", "abc"));
  CHECK(StrNEq("abc", "a"));
  CHECK(StrStartsWith("abc", ""));
  CHECK(StrStartsWith("abc", "a"));
  CHECK(StrStartsWith("abc", "ab"));
  CHECK(StrStartsWith("abc", "abc"));
  CHECK(!StrStartsWith("abc", "abcd"));
}

void TestMems() {
  CHECK(MemEq("", "", 1));
  CHECK(MemEq("a", "a", 2));
  CHECK(MemEq("abc", "abc", 3));
  CHECK(!MemEq("abc", "axe", 3));  
}

int main(int argc, char* argv[]) {
  TestInts();
  TestChars();
  TestStrings();
  TestMems();
  return 0;
}
