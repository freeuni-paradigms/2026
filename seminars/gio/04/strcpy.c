#include <stdio.h>
#include <string.h>


/*
 * Copies the string src (INCLUDING the terminating '\0') into dest.
 * Returns dest.
 *
 * Note: strcpy has no idea how big dest is.
 * Making sure dest is large enough is the caller's job.
 */
char *my_strcpy1(char *dest, const char *src) {
  int i = 0;
  while(src[i] != '\0'){
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0';
  return dest;
}




/*
 * Copies the string src (INCLUDING the terminating '\0') into dest.
 * Returns dest.
 *
 * Note: strcpy has no idea how big dest is.
 * Making sure dest is large enough is the caller's job.
 */
char *my_strcpy(char *dest, const char *src) {
  char * start = dest;

  while(*src != '\0'){
    *dest = *src;
    dest++;
    src++;
  }
  *dest = '\0';

  return start;
}


/* ---------------------------------------------------------------- */
/*  tests - don't touch, run with ./strcpy                           */
/* ---------------------------------------------------------------- */

static int passed = 0, total = 0;

static void check(const char *name, int ok) {
  total++;
  if (ok) passed++;
  printf("  [%s] %s\n", ok ? " OK " : "FAIL", name);
}

/* fill the buffer with 'A's so a missing '\0' becomes visible */
static void dirty(char *buf, size_t size) {
  memset(buf, 'A', size - 1);
  buf[size - 1] = '\0';
}

static void run_tests(void) {
  char buf[16];

  printf("\n=== tests ===\n");

  // 1. basic copy
  dirty(buf, sizeof buf);
  my_strcpy(buf, "paradigms");
  printf("  buf = \"%s\"\n", buf);
  check("basic copy", strcmp(buf, "paradigms") == 0);

  // 2. forgot the '\0'? then you'll see "QWERTYAAAAAAAAA"
  dirty(buf, sizeof buf);
  my_strcpy(buf, "QWERTY");
  printf("  buf = \"%s\"\n", buf);
  check("'\\0' is copied too", strcmp(buf, "QWERTY") == 0);

  // 3. empty string - only the '\0' gets copied
  dirty(buf, sizeof buf);
  my_strcpy(buf, "");
  check("empty string", buf[0] == '\0');

  // 4. strcpy writes exactly strlen(src) + 1 bytes, nothing more
  dirty(buf, sizeof buf);
  my_strcpy(buf, "abc");
  check("bytes after '\\0' untouched",
        strcmp(buf, "abc") == 0 && buf[4] == 'A' && buf[5] == 'A');

  // 5. exact fit: "hello" needs 6 bytes, not 5
  char exact[6];
  my_strcpy(exact, "hello");
  check("exact fit (5 chars + '\\0')", strcmp(exact, "hello") == 0);

  // 6. return value is dest -> calls can be chained
  char *ret = my_strcpy(buf, "chained call works");
  check("returns dest", ret == buf);
  if (ret != NULL)
    printf("  chained: %s\n", ret);

  // 7. dest doesn't have to be the start of an array
  my_strcpy(buf, "hello");
  my_strcpy(buf + 5, "world");
  printf("  buf = \"%s\"\n", buf);
  check("copy into buf + 5", strcmp(buf, "helloworld") == 0);

  // 8. same result as the real strcpy
  char mine[16], real[16];
  dirty(mine, sizeof mine);
  dirty(real, sizeof real);
  my_strcpy(mine, "Free Uni");
  strcpy(real, "Free Uni");
  check("same bytes as strcpy", memcmp(mine, real, sizeof mine) == 0);

  printf("\n  %d / %d passed\n", passed, total);
}

/* ---------------------------------------------------------------- */
/*  demos - run with ./strcpy demo                                   */
/* ---------------------------------------------------------------- */

static void demo_overflow(void) {
  // struct fields sit next to each other in memory (no padding between char arrays)
  struct {
    char small[4];
    char neighbor[12];
  } s;

  strcpy(s.neighbor, "I'm safe");
  printf("\n=== overflow: dest too small ===\n");
  printf("  before: neighbor = \"%s\"\n", s.neighbor);

  // "paradigms" needs 10 bytes, small has 4 -> the rest lands in neighbor
  my_strcpy(s.small, "paradigms");
  printf("  after my_strcpy(s.small, \"paradigms\"):\n");
  printf("          neighbor = \"%s\"\n", s.neighbor);
}

static void demo_int_array(void) {
  printf("\n=== strcpy into an int array ===\n");

  // strcpy only sees bytes - it doesn't care what's really there
  int arr[2] = {0, 0};
  my_strcpy((char *)arr, "abc");

  // 'a' = 0x61, 'b' = 0x62, 'c' = 0x63, '\0' = 0x00
  // bytes in memory:  61 62 63 00
  // little-endian -> the first byte is the lowest -> 0x00636261
  printf("  arr[0] = %d = 0x%08x\n", arr[0], arr[0]);
  printf("  arr[1] = %d\n", arr[1]);
}

int main(int argc, char *argv[]) {
  if (argc > 1 && strcmp(argv[1], "demo") == 0) {
    demo_overflow();
    demo_int_array();
    return 0;
  }

  run_tests();
  return 0;
}
