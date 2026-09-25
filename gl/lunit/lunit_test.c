#include "gl/lunit/lunit.h"

TEST(CStringEq) {
  ASSERT_STR_EQ("a", "a");
  ASSERT_STR_EQ("abc", "abc");
}

TEST(CStringNEq) {
  ASSERT_STR_NEQ("gio", "elo");
}

int main(int argc, char* argv[]) {
  LUnitOpts opts;
  LUnitOptsInit(&opts, argc, argv);
  TEST_SUITE(all);
  ADD_TEST(all, CStringEq);
  ADD_TEST(all, CStringNEq);
  TestSuite* suites[] = {&all};
  ProcessTestSuites(1, suites,  &opts);
  TestSuiteDispose(&all);
  return 0;
}
