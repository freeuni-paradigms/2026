#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ./casting      - runs every section
 * ./casting 5    - runs only section 5
 */

/* prints the bits of any variable, most significant byte first */
static void print_bits(const void *p, size_t size) {
  const unsigned char *bytes = p;
  // little-endian: the LAST byte in memory is the most significant one
  for (size_t i = size; i > 0; i--) {
    for (int b = 7; b >= 0; b--)
      putchar((bytes[i - 1] >> b) & 1 ? '1' : '0');
    putchar(' ');
  }
  putchar('\n');
}

/* SHOW("%d", x) prints:   x  =  42   00000000 00000000 00000000 00101010 */
#define SHOW(fmt, x)                                  \
  do {                                                \
    char value_[64];                                  \
    snprintf(value_, sizeof value_, fmt, x);          \
    printf("  %-12s = %-16s ", #x, value_);           \
    print_bits(&(x), sizeof(x));                      \
  } while (0)

/* ================================================================ */

static void widening(void) {
  // smaller -> bigger type: the value never changes
  // a char is just a small number - %c and %d only change how printf shows it
  char c = 'A';
  int i = c;
  printf("  c as %%c = '%c', c as %%d = %d\n", c, c);
  SHOW("%d", c);
  SHOW("%d", i);

  // signed source: the sign bit is copied into all new bits (sign extension)
  char neg = -5;
  short neg_s = neg;
  int neg_i = neg;
  SHOW("%d", neg);
  SHOW("%d", neg_s);
  SHOW("%d", neg_i);

  // unsigned source: new bits are 0 (zero extension)
  // same 8 bits as -5 above, completely different int
  unsigned char u = 251;
  int u_i = u;
  SHOW("%d", u);
  SHOW("%d", u_i);
}

static void char_signedness(void) {
  // is plain `char` signed or unsigned? IMPLEMENTATION-DEFINED!
  //   x86 gcc: signed   -> -56
  //   ARM Linux: unsigned -> 200   (but macOS on Apple Silicon: signed)
  char c = (char)200;
  int i = c;
  SHOW("%d", c);
  SHOW("%d", i);

  // 0xFF is the int 255, ff gets promoted to the int -1
  char ff = (char)0xFF;
  printf("  ff == 0xFF  -> %s\n", ff == 0xFF ? "true" : "false");

  // classic bug:  char ch; while ((ch = getchar()) != EOF) ...
  //   signed char:   byte 0xFF (ÿ) looks like EOF (-1), loop stops early
  //   unsigned char: ch is never -1, loop never stops
  //   fix: int ch;
}

static void narrowing(void) {
  // bigger -> smaller type: the high bits are simply cut off
  //                  15    12
  // 00000000 00000000 10010000 00010001
  int i1 = 17 + (1 << 15) + (1 << 12);
  short s1 = (short)i1;
  unsigned short us1 = (unsigned short)i1;
  SHOW("%d", i1);
  SHOW("%d", s1);  // bit 15 is now the sign bit -> negative
  SHOW("%d", us1); // same bits, read as unsigned -> positive

  int i2 = 300;
  char c2 = (char)i2; // 300 - 256
  SHOW("%d", i2);
  SHOW("%d", c2);

  int i3 = 128;
  char c3 = (char)i3; // one more than CHAR_MAX
  SHOW("%d", i3);
  SHOW("%d", c3);

  int i4 = 65535;
  short s4 = (short)i4; // all ones
  SHOW("%d", i4);
  SHOW("%d", s4);

  int i5 = 65536;
  short s5 = (short)i5; // the only 1 bit gets cut off
  SHOW("%d", i5);
  SHOW("%d", s5);
}

static void signed_unsigned(void) {
  // same size: the bits DON'T change, only how we read them
  int minus_one = -1;
  unsigned int u = (unsigned int)minus_one;
  SHOW("%d", minus_one);
  SHOW("%u", u);

  unsigned int big = 3000000000u;
  int big_i = (int)big;
  SHOW("%u", big);
  SHOW("%d", big_i);

  int imax = INT_MAX;
  int imin = INT_MIN;
  unsigned int umax = UINT_MAX;
  SHOW("%d", imax);
  SHOW("%d", imin);
  SHOW("%u", umax);

  // printf doesn't know the real type - the format decides how bits are read
  printf("  printf(\"%%d\", umax) -> %d\n", umax);
  printf("  printf(\"%%u\", imin) -> %u\n", imin);
  printf("  printf(\"%%x\", -1)   -> %x\n", minus_one);
}

static void conversion_order(void) {
  // widening + sign change: WHICH HAPPENS FIRST matters
  // rule: when widening, the SOURCE type decides sign vs zero extension
  short s = -1;
  unsigned int a = (unsigned int)s;                 // sign-extend, then read as unsigned
  unsigned int b = (unsigned int)(unsigned short)s; // read as unsigned (65535), then zero-extend
  SHOW("%d", s);
  SHOW("%u", a);
  SHOW("%u", b);

  unsigned short us = 65535;
  int c = us;        // unsigned source -> zero extension
  int d = (short)us; // make it signed first -> sign extension
  SHOW("%u", us);
  SHOW("%d", c);
  SHOW("%d", d);
}

static void promotion(void) {
  // anything smaller than int becomes int before arithmetic
  unsigned char a = 200, b = 100;
  int sum = a + b;           // computed as int -> 300
  unsigned char small = a + b; // 300 doesn't fit into 8 bits -> 44
  SHOW("%d", sum);
  SHOW("%d", small);

  char c = 127;
  printf("  c + 1        = %d   (int, no overflow)\n", c + 1);
  c++; // stored back into a char
  SHOW("%d", c);

  printf("  sizeof(c)     = %zu\n", sizeof(c));
  printf("  sizeof(c + 1) = %zu\n", sizeof(c + 1));
  printf("  sizeof('a')   = %zu   (in C 'a' is an int! in C++ it's 1)\n", sizeof('a'));

  // ~ is also done on int: x is promoted and the 24 new zeros become ones
  unsigned char x = 0x0F;
  int not_x = ~x;
  SHOW("%d", x);
  SHOW("%d", not_x);
  printf("  ~x == 0xF0  -> %s\n", ~x == 0xF0 ? "true" : "false");
}

static void comparisons(void) {
  // int vs unsigned int: the int is converted to unsigned!
  int i = -1;
  unsigned int u = 1;
  printf("  i < u              -> %s   (int -1 vs unsigned 1)\n", i < u ? "true" : "false");
  printf("  i < sizeof(int)    -> %s   (sizeof gives size_t, unsigned)\n",
         i < sizeof(int) ? "true" : "false");

  // short vs unsigned short: BOTH are promoted to int first -> normal math
  short s = -1;
  unsigned short us = 1;
  printf("  s < us             -> %s    (short -1 vs unsigned short 1)\n",
         s < us ? "true" : "false");

  // size_t is unsigned - it can never be negative
  size_t diff = strlen("ab") - strlen("abc");
  printf("  strlen(\"ab\") - strlen(\"abc\") = %zu\n", diff);
  printf("  strlen(\"ab\") - strlen(\"abc\") < 0 -> %s\n",
         strlen("ab") - strlen("abc") < 0 ? "true" : "false");

  // unsigned >= 0 is always true -> this loop never ends
  printf("  for (unsigned k = 3; k >= 0; k--):");
  int guard = 0;
  for (unsigned int k = 3; k >= 0 && guard < 6; k--, guard++)
    printf(" %u", k);
  printf(" ...\n");
}

static int will_overflow(int x) {
  int next = x + 1;
  return next < x; // looks like a reasonable check...
}

static void overflow(void) {
  // unsigned overflow is DEFINED: arithmetic modulo 2^32
  unsigned int umax = UINT_MAX;
  unsigned int wrapped = umax + 1;
  unsigned int zero = 0;
  unsigned int under = zero - 1;
  SHOW("%u", wrapped);
  SHOW("%u", under);

  // signed overflow is UNDEFINED BEHAVIOUR
  // with -O0 it usually wraps around, but the compiler is allowed to assume it never happens
  int imax = INT_MAX;
  int next = imax + 1;
  SHOW("%d", next);

  // compile with -O0 -> 1,  compile with -O2 -> 0  (gcc deletes the check)
  printf("  will_overflow(INT_MAX) = %d\n", will_overflow(INT_MAX));

  // INT_MIN has no positive twin: -INT_MIN and abs(INT_MIN) are also UB
}

static void float_int(void) {
  // (int) of a float converts the VALUE: the bits are recomputed
  float f = 2.9f;
  int i = (int)f; // truncation, not rounding
  SHOW("%f", f);
  SHOW("%d", i);

  float nf = -2.9f;
  int ni = (int)nf; // toward zero, not floor (floor would be -3)
  SHOW("%f", nf);
  SHOW("%d", ni);

  // float has 24 bits of precision - not every int fits
  int big = 16777217; // 2^24 + 1
  float fbig = (float)big;
  int back = (int)fbig;
  SHOW("%d", big);
  SHOW("%f", fbig);
  SHOW("%d", back);

  // 0.1 can't be written in binary exactly
  printf("  0.1f             = %.20f\n", 0.1f);
  printf("  0.1 + 0.2 == 0.3 -> %s\n", 0.1 + 0.2 == 0.3 ? "true" : "false");

  // printf does NOT convert: printf("%f", i) is undefined behaviour
  // you have to cast yourself:
  printf("  printf(\"%%f\", (float)i) -> %f\n", (float)i);
}

static void reinterpret(void) {
  // (int)f               - converts the VALUE
  // *(int *)&f           - reads the same 4 BYTES as if they were an int
  float f = 1.25f;
  int by_value = (int)f;
  int by_bits = *(int *)&f;
  SHOW("%f", f);
  SHOW("%d", by_value);
  SHOW("%d", by_bits);

  // S EEEEEEEE MMMMMMMMMMMMMMMMMMMMMMM
  // 0 01111111 01000000000000000000000
  // (-1)^S * 1.M * 2^(E - 127)
  //    = (-1)^0 * 1.01(binary) * 2^(127 - 127) = 1.25 * 1 = 1.25

  // 2.4 = 1.2 * 2^1, and 0.2 in binary is 0.0011 0011 0011 ... forever -> rounded
  float f2 = 2.4f;
  int f2_value = (int)f2;
  int f2_bits = *(int *)&f2;
  SHOW("%f", f2);
  SHOW("%d", f2_value);
  SHOW("%d", f2_bits);

  int one = 1;
  float tiny = *(float *)&one; // the smallest positive float
  SHOW("%d", one);
  SHOW("%g", tiny);

  int magic = 1065353216; // 0 01111111 000... -> 1.0
  float from_magic = *(float *)&magic;
  SHOW("%d", magic);
  SHOW("%f", from_magic);

  // equal values, different bits
  float zero = 0.0f;
  float neg_zero = -0.0f;
  SHOW("%f", zero);
  SHOW("%f", neg_zero);
  printf("  zero == neg_zero -> %s\n", zero == neg_zero ? "true" : "false");

  // note: *(int *)&f breaks the "strict aliasing" rule - fine with -O0 for class,
  // the portable way is memcpy(&by_bits, &f, sizeof f);
}

static void memory(void) {
  // little-endian: the LOWEST byte is stored FIRST
  int x = 0x01020304;
  unsigned char *p = (unsigned char *)&x;
  printf("  x = 0x%08x, bytes in memory: %02x %02x %02x %02x\n", x, p[0], p[1], p[2], p[3]);
  printf("  *(char *)&x      = %d\n", *(char *)&x);

  short *sp = (short *)&x;
  printf("  ((short *)&x)[0] = 0x%04x\n", sp[0]);
  printf("  ((short *)&x)[1] = 0x%04x\n", sp[1]);

  // two shorts read as one int
  short arr[2] = {5, 1};
  int both = *(int *)arr; // 5 + (1 << 16)
  SHOW("%d", both);

  // one short read as an int: the 2 extra bytes don't belong to lonely -> garbage
  // (undefined behaviour - the result can change between runs / compilers)
  short lonely = 5;
  int garbage = *(int *)&lonely;
  SHOW("%d", lonely);
  SHOW("%d", garbage);

  // an int array read as a string: 'A' = 0x41, 'B' = 0x42 ...
  int word[2] = {0x44434241, 0};
  printf("  (char *)word     = \"%s\"\n", (char *)word);

  // the pointer type decides how big one step is
  int nums[3] = {10, 20, 30};
  printf("  nums             = %p\n", (void *)nums);
  printf("  nums + 1         = %p   (+4 bytes)\n", (void *)(nums + 1));
  printf("  (char *)nums + 1 = %p   (+1 byte)\n", (void *)((char *)nums + 1));
}

/* ================================================================ */

typedef struct {
  const char *title;
  void (*run)(void);
} section;

static const section sections[] = {
    {"widening: sign extension vs zero extension", widening},
    {"char: signed or unsigned?", char_signedness},
    {"narrowing: the high bits are cut off", narrowing},
    {"signed <-> unsigned: same bits, different value", signed_unsigned},
    {"order of conversions", conversion_order},
    {"integer promotion", promotion},
    {"signed vs unsigned comparisons", comparisons},
    {"overflow", overflow},
    {"float <-> int: value conversion", float_int},
    {"reinterpreting bits: *(int *)&f", reinterpret},
    {"memory, endianness, pointer casts", memory},
};

int main(int argc, char *argv[]) {
  int count = sizeof sections / sizeof sections[0];
  int only = argc > 1 ? atoi(argv[1]) : 0;

  if (only < 0 || only > count) {
    printf("usage: %s [1-%d]\n", argv[0], count);
    return 1;
  }

  for (int k = 1; k <= count; k++) {
    if (only != 0 && only != k)
      continue;
    printf("\n=== %d. %s ===\n", k, sections[k - 1].title);
    sections[k - 1].run();
  }
  return 0;
}
