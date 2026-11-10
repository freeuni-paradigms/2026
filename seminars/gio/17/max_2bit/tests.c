#include "max_2bit.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_INT_EQ(a, b) {						\
    int expected = (a);							\
    int actual = (b);							\
    if (expected != actual) {						\
      printf("%s:%d Assertion failed: expected %d got %d\n", __FILE__, __LINE__, expected, actual); \
      return false;							\
    }									\
  }

#define TEST(t) { \
    printf("-- TESTING: %s\n", #t); \
    if (t()) {	  \
      printf("TEST %s: SUCCEEDED\n", #t);	\
    } \
    else { \
      printf("TEST %s: FAILED\n", #t);		\
    } \
    printf("####\n\n"); \
  }

bool None() {
  char k[] = {(char)0};
  ASSERT_INT_EQ(0, FindMax(k))
  return true;
}

bool One() {
  char k[] = {(char)36, (char)0};
  ASSERT_INT_EQ(1, FindMax(k));
  return true;
}

bool Two() {
  char k[] = {(char)36, (char)39, (char)0};
  ASSERT_INT_EQ(7, FindMax(k));
  return true;
}

bool EarlyStop() {
  char k[] = {(char)36, (char)100, (char)0, (char)116};
  ASSERT_INT_EQ(1, FindMax(k));
  return true;
}

bool AllOnes() {
  char k[] = {(char)255, (char)0};
  ASSERT_INT_EQ(0, FindMax(k));
  return true;
}

bool HighBits() {
  char k[] = {(char)128, (char)0};
  ASSERT_INT_EQ(0, FindMax(k));
  return true;
}

bool LowBits() {
  char k[] = {(char)1, (char)0};
  ASSERT_INT_EQ(3, FindMax(k));
  return true;
}

bool Many() {
  char k[1000];
  for (int i = 0; i < 999; ++i) {
    k[i] = (char)(i + 1);
  }
  k[999] = (char)0;
  ASSERT_INT_EQ(11, FindMax(k));
  return true;
}

int main(int argc, char** argv) {
  TEST(None);
  TEST(One);
  TEST(Two);
  TEST(EarlyStop);
  TEST(AllOnes);
  TEST(HighBits);
  TEST(LowBits);
  TEST(Many);
  return 0;
}
