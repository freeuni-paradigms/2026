#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * We call ours my_... because <string.h> is included too:
 * the tests compare our versions with the real ones.
 */

/*
 * Returns the number of chars before the first '\0'.
 */
size_t my_strlen(const char *s) {
  size_t len = 0;
  while(s[len] != '\0'){
    len++;
  }
  return len;
}

/*
 * Compares s1 and s2 char by char. Chars are compared as unsigned char.
 * Returns < 0 if s1 comes first, 0 if they are equal, > 0 if s2 comes first.
 * Only the sign is promised, not the exact number.
 */
int my_strcmp(const char *s1, const char *s2) {
  while(*s1 != '\0' && *s1 == *s2){
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}

/*
 * Copies src (INCLUDING the '\0') into dest. Returns dest.
 * (seminar 04)
 */
char *my_strcpy(char *dest, const char *src) {
  int i = 0;
  while(src[i] != '\0'){
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0';
  return dest;
}

/*
 * Appends src to the end of the string in dest. Returns dest.
 * dest's '\0' gets overwritten, and the new '\0' comes from src.
 *
 * dest must have room for strlen(dest) + strlen(src) + 1 bytes.
 */
char *my_strcat(char *dest, const char *src) {
  my_strcpy(dest + my_strlen(dest) , src);
  return dest;
}

/*
 * Returns a NEW copy of s, allocated with malloc.
 * The caller must free() it.
 */
char *my_strdup(const char *s) {
  char* copy = malloc(my_strlen(s) + 1);
  if(copy == NULL){
    return NULL;
  }
  return my_strcpy(copy, s);
}

/*
 * Returns a NEW string: first followed by second, allocated with malloc.
 * first and second must not change. The caller must free() the result.
 *
 * concat_strings("abc", "de") -> "abcde"
 */
char *concat_strings(const char *first, const char *second) {
  size_t len = my_strlen(first) + my_strlen(second) + 1;
  char* result = malloc(len);
  if(result == NULL){
    return NULL;
  }
  my_strcpy(result, first);
  my_strcat(result, second);
  return result;
}

/*
 * Sets the first n BYTES of s to c (only the low byte of c is used).
 * Returns s.
 */
void *my_memset(void *s, int c, size_t n) {
  unsigned char * p = s;
  for(size_t i = 0; i < n; i++){
    p[i] = (unsigned char) c;
  }
  return s;
}

/* ---------------------------------------------------------------- */
/*  tests - don't touch                                             */
/*    ./strings           all tests                                 */
/*    ./strings strcat    only one function                         */
/*    ./strings demo      demos                                     */
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

/* strcmp that doesn't crash on NULL */
static int same(const char *a, const char *b) {
  return a != NULL && strcmp(a, b) == 0;
}

static void test_strlen(void) {
  printf("\n=== my_strlen ===\n");
  check("\"Programming\" -> 11", my_strlen("Programming") == 11);
  check("\"a\" -> 1", my_strlen("a") == 1);
  check("\"\" -> 0", my_strlen("") == 0);
  check("\"Para\\0digms\" -> 4 (stops at the first '\\0')", my_strlen("Para\0digms") == 4);

  char big[1000];
  dirty(big, sizeof big);
  check("999 'A's -> 999", my_strlen(big) == 999);
}

static void test_strcmp(void) {
  printf("\n=== my_strcmp ===\n");
  check("\"abc\" == \"abc\"", my_strcmp("abc", "abc") == 0);
  check("\"abcde\" > \"abcdd\"", my_strcmp("abcde", "abcdd") > 0);
  check("\"abcde\" < \"bcde\"   (the first difference decides)", my_strcmp("abcde", "bcde") < 0);
  check("\"abc\" < \"b\"        (shorter is NOT automatically smaller)", my_strcmp("abc", "b") < 0);
  check("\"abc\" < \"abcd\"     (a prefix comes first)", my_strcmp("abc", "abcd") < 0);
  check("\"abcd\" > \"abc\"", my_strcmp("abcd", "abc") > 0);
  check("\"\" == \"\"", my_strcmp("", "") == 0);
  check("\"\" < \"a\"", my_strcmp("", "a") < 0);
  check("\"Zoo\" < \"apple\"   (ASCII: 'Z' = 90, 'a' = 97)", my_strcmp("Zoo", "apple") < 0);

  // 0xE9 is 'e' with an accent in Latin-1. As an unsigned char it's 233 > 'a'.
  check("\"\\xE9\" > \"a\"       (compare as unsigned char)", my_strcmp("\xE9", "a") > 0);
}

static void test_strcpy(void) {
  printf("\n=== my_strcpy ===\n");
  char buf[16];

  dirty(buf, sizeof buf);
  char *ret = my_strcpy(buf, "QWERTY");
  printf("  buf = \"%s\"\n", buf);
  check("copies the text and the '\\0'", strcmp(buf, "QWERTY") == 0);
  check("returns dest", ret == buf);

  dirty(buf, sizeof buf);
  my_strcpy(buf, "");
  check("empty string", buf[0] == '\0' && buf[1] == 'A');
}

static void test_strcat(void) {
  printf("\n=== my_strcat ===\n");
  char buf[50];

  dirty(buf, sizeof buf);
  strcpy(buf, "Start ");
  char *ret = my_strcat(buf, "First");
  printf("  buf = \"%s\"\n", buf);
  check("\"Start \" + \"First\" -> \"Start First\"", strcmp(buf, "Start First") == 0);
  check("returns dest", ret == buf);

  my_strcat(buf, " Second");
  my_strcat(buf, " Third");
  printf("  buf = \"%s\"\n", buf);
  check("three calls in a row", strcmp(buf, "Start First Second Third") == 0);

  dirty(buf, sizeof buf);
  buf[0] = '\0';
  my_strcat(buf, "abc");
  check("\"\" + \"abc\" -> \"abc\"", strcmp(buf, "abc") == 0);

  dirty(buf, sizeof buf);
  strcpy(buf, "abc");
  my_strcat(buf, "");
  check("\"abc\" + \"\" -> \"abc\"", strcmp(buf, "abc") == 0);

  dirty(buf, sizeof buf);
  strcpy(buf, "ab");
  my_strcat(buf, "cd");
  check("nothing written after the new '\\0'", strcmp(buf, "abcd") == 0 && buf[5] == 'A');
}

static void test_strdup(void) {
  printf("\n=== my_strdup ===\n");
  const char *orig = "Strdup First Example";
  char *copy = my_strdup(orig);

  printf("  orig = %p  (a string literal)\n", (void *)orig);
  printf("  copy = %p  (should be on the heap)\n", (void *)copy);
  check("same text", same(copy, orig));
  check("different address (a real copy)", copy != NULL && copy != orig);

  int independent = 0;
  if (copy != NULL && copy != orig) {
    copy[0] = 's';
    independent = copy[0] == 's' && orig[0] == 'S';
    free(copy);
  }
  check("changing the copy doesn't change the original", independent);

  char *empty = my_strdup("");
  check("\"\" -> \"\" (still needs 1 byte for the '\\0')", same(empty, ""));
  free(empty);
}

static void test_concat(void) {
  printf("\n=== concat_strings ===\n");
  // arrays with extra room, so strcat(first, ...) would not crash, just fail
  char first[16] = "abc";
  char second[16] = "de";

  char *r = concat_strings(first, second);
  printf("  result = \"%s\"\n", r != NULL ? r : "(NULL)");
  printf("  addresses (can't be the same):\n");
  printf("  first: %p, second: %p, result: %p\n", (void *)first, (void *)second, (void *)r);
  check("\"abc\" + \"de\" -> \"abcde\"", same(r, "abcde"));
  check("result is new memory", r != NULL && r != first && r != second);
  check("first and second didn't change", strcmp(first, "abc") == 0 && strcmp(second, "de") == 0);
  if (r != first && r != second)
    free(r);

  r = concat_strings("", "de");
  check("\"\" + \"de\" -> \"de\"", same(r, "de"));
  free(r);

  r = concat_strings("abc", "");
  check("\"abc\" + \"\" -> \"abc\"", same(r, "abc"));
  free(r);

  r = concat_strings("", "");
  check("\"\" + \"\" -> \"\"", same(r, ""));
  free(r);
}

static void test_memset(void) {
  printf("\n=== my_memset ===\n");
  char buf[16];
  memset(buf, 'x', sizeof buf);

  void *ret = my_memset(buf, 'A', 10);
  check("first 10 bytes are 'A'", memcmp(buf, "AAAAAAAAAA", 10) == 0);
  check("byte 10 is untouched", buf[10] == 'x');
  check("returns s", ret == buf);

  my_memset(buf, 'B', 0);
  check("n = 0 changes nothing", buf[0] == 'A');

  buf[0] = 'x';
  my_memset(buf, 0x141, 1);
  check("only the low byte of c is used: 0x141 -> 0x41 = 'A'", buf[0] == 'A');

  int arr[4] = {1, 2, 3, 4};
  my_memset(arr, 0, sizeof arr);
  check("int array -> all 0 (n = sizeof arr = 16 bytes)",
        arr[0] == 0 && arr[1] == 0 && arr[2] == 0 && arr[3] == 0);

  int ones[4] = {0, 0, 0, 0};
  my_memset(ones, 1, sizeof ones);
  check("n counts BYTES: memset(arr, 1, ...) makes every int 0x01010101",
        ones[0] == 0x01010101 && ones[3] == 0x01010101);
}

/* ---------------------------------------------------------------- */
/*  demos - run with ./strings demo                                 */
/* ---------------------------------------------------------------- */

static void demo(void) {
  int arr[3];

  printf("\n=== memset sets BYTES, not ints ===\n");
  memset(arr, 0, sizeof arr);
  printf("  memset(arr,  0, sizeof arr) -> %d %d %d\n", arr[0], arr[1], arr[2]);
  memset(arr, -1, sizeof arr);
  printf("  memset(arr, -1, sizeof arr) -> %d %d %d\n", arr[0], arr[1], arr[2]);
  memset(arr, 1, sizeof arr);
  printf("  memset(arr,  1, sizeof arr) -> %d %d %d   (0x%08x)\n", arr[0], arr[1], arr[2], arr[0]);
  int count = 3; // how many ints... but memset wants BYTES
  memset(arr, 0, count);
  printf("  memset(arr,  0, count = 3)  -> %d %d %d   (only 3 BYTES were cleared)\n",
         arr[0], arr[1], arr[2]);

  printf("\n=== why strcmp needs unsigned char ===\n");
  char c = '\xE9';
  printf("  char c = '\\xE9';  as char: %d,  as unsigned char: %d\n", c, (unsigned char)c);
  printf("  c - 'a'                 = %d\n", c - 'a');
  printf("  (unsigned char)c - 'a'  = %d\n", (unsigned char)c - 'a');
}

int main(int argc, char *argv[]) {
  const char *only = argc > 1 ? argv[1] : NULL;
  setbuf(stdout, NULL); // print right away, so a crash shows which test it was

  if (only != NULL && strcmp(only, "demo") == 0) {
    demo();
    return 0;
  }

  int all = only == NULL;
  if (all || strcmp(only, "strlen") == 0) test_strlen();
  if (all || strcmp(only, "strcmp") == 0) test_strcmp();
  if (all || strcmp(only, "strcpy") == 0) test_strcpy();
  if (all || strcmp(only, "strcat") == 0) test_strcat();
  if (all || strcmp(only, "strdup") == 0) test_strdup();
  if (all || strcmp(only, "concat") == 0) test_concat();
  if (all || strcmp(only, "memset") == 0) test_memset();

  if (total == 0) {
    printf("usage: %s [strlen|strcmp|strcpy|strcat|strdup|concat|memset|demo]\n", argv[0]);
    return 1;
  }
  printf("\n  %d / %d passed\n", passed, total);
  return 0;
}
