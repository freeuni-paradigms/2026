#include "compress.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ASSERT_INT_EQ(a, b) {						\
    int expected = (a);							\
    int actual = (b);							\
    if (expected != actual) {						\
      printf("%s:%d Assertion failed: expected %d got %d\n", __FILE__, __LINE__, expected, actual); \
      return false;							\
    }									\
  }

#define ASSERT_BYTE_EQ(a, b) {						\
    char expected = (a);							\
    char actual = (b);							\
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


bool Empty() {
  char k[] = { 0b00000000 };
  void* c = malloc(1000);
  ASSERT_INT_EQ(0, Compress(k, c));
  free(c);
  return true;
}

bool One() {
  char k[] = { 0b00100000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(1, Compress(k, c));
  ASSERT_BYTE_EQ(0b00010010, c[0]);
  free(c);
  return true;
}

bool TwoDistinct() {
  char k[] = { 0b00101011, 0b00000000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(2, Compress(k, c));
  ASSERT_BYTE_EQ(0b00010010, c[0]);
  ASSERT_BYTE_EQ(0b00011011, c[1]);
  free(c);
  return true;
}

bool AllSame() {
  // 1111
  char k[] = { 0b00010001, 0b00010001, 0b00000000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(1, Compress(k, c));
  ASSERT_BYTE_EQ(0b01000001, c[0]);
  free(c);
  return true;
}

bool TwoNumbers() {
  // 11555
  char k[] = { 0b00010001, 0b01010101, 0b01010000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(2, Compress(k, c));
  ASSERT_BYTE_EQ(0b00100001, c[0]);
  ASSERT_BYTE_EQ(0b00110101, c[1]);
  free(c);
  return true;
}

bool Fifteen() {
  // 111111111111111
  char k[] = { 0b00010001, 0b00010001, 0b00010001, 0b00010001, 0b00010001, 0b00010001, 0b00010001, 0b00010000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(1, Compress(k, c));
  ASSERT_BYTE_EQ(0b11110001, c[0]);
  free(c);
  return true;
}

bool OnesInFrontAndBack(){
  // 115551
  char k[] = { 0b00010001, 0b01010101, 0b01010001, 0b00000000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(3, Compress(k, c));
  ASSERT_BYTE_EQ(0b00100001, c[0]);
  ASSERT_BYTE_EQ(0b00110101, c[1]);
  ASSERT_BYTE_EQ(0b00010001, c[2]);
  free(c);
  return true;
}

bool MixedBig() {
  // 55555555555888888888888888555556
  char k[] = {0b1010101,0b1010101,0b1010101,0b1010101,0b1010101,0b1011000,0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b1010101,0b1010101,0b1010110,0b00000000};
  char* c = malloc(1000);
  ASSERT_INT_EQ(4, Compress(k, c));
  ASSERT_BYTE_EQ(0b10110101, c[0]);
  ASSERT_BYTE_EQ(0b11111000, c[1]);
  ASSERT_BYTE_EQ(0b1010101, c[2]);
  ASSERT_BYTE_EQ(0b10110, c[3]);
  free(c);
  return true;
}

bool FourFifteens() {
  // 15|15|15|15
  char k[] = { 0b11111111, 0b11111111, 0b00000000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(1, Compress(k, c));
  ASSERT_BYTE_EQ(0b01001111, c[0]);
  free(c);
  return true;
}

bool AllDistinct() {
  // 12345
  char k[] = { 0b10010,0b110100,0b1010000 };
  char* c = malloc(1000);
  ASSERT_INT_EQ(5, Compress(k, c));
  ASSERT_BYTE_EQ(0b10001, c[0]);
  ASSERT_BYTE_EQ(0b10010, c[1]);
  ASSERT_BYTE_EQ(0b10011, c[2]);
  ASSERT_BYTE_EQ(0b10100, c[3]);
  ASSERT_BYTE_EQ(0b10101, c[4]);
  free(c);
  return true;
}

int main(int argc, char** argv) {
  TEST(Empty);
  TEST(One);
  TEST(TwoDistinct);
  TEST(AllSame);
  TEST(TwoNumbers);
  TEST(Fifteen);
  TEST(OnesInFrontAndBack);
  TEST(MixedBig);
  TEST(FourFifteens);
  TEST(AllDistinct);
  return 0;
}
