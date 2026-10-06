#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Decompresses run-length encoded (RLE) data.
 *
 * The data is a sequence of blocks and ends with '\0':
 *
 *   [header][len chars][header][len chars] ... '\0'
 *
 *   header = one byte:   high 4 bits = len  (how long the repeated piece is)
 *                        low  4 bits = run  (how many times it repeats)
 *
 *   "aabcbcbcx" is compressed as   00010010|a|00100011|b|c|00010001|x
 *                                  len 1, run 2    len 2, run 3    len 1, run 1
 *                                  -> "aa"         -> "bcbcbc"     -> "x"
 *
 * Takes ownership of the memory block *data points to (it was malloc'd).
 * After returning, *data must point to a malloc'd C string with the
 * decompressed text. Its ownership goes back to the caller.
 */
void Decompress(char **data) {
  size_t total = 0;
  unsigned char * compressed = (unsigned char*) *data;
  unsigned char * p = compressed;

  while(*p != '\0'){
    int len = *p >> 4;
    int run = *p & 0x0F;
    total += len * run;
    p += 1 + len;
  }

  char * result = malloc(total + 1);
  assert(result != NULL);

  p = compressed;
  char* out = result;

  while(*p != '\0'){
    int len = *p >> 4;
    int run = *p & 0x0F;
    for(int i = 0; i < run; i++){
      memcpy(out, p + 1, len);
      out += len;
    }
    p += 1 + len;
  }

  *out = '\0';
  free(*data);
  *data = result;
}

/* ---------------------------------------------------------------- */
/*  tests - don't touch                                             */
/*    ./decompress                       all tests                  */
/*    make asan && ./decompress_asan     also finds leaks           */
/* ---------------------------------------------------------------- */

static int passed = 0, total = 0;

/* Decompress takes ownership and frees the input, so it has to be on the heap */
static char *heap_copy(const char *s) {
  char *copy = malloc(strlen(s) + 1);
  assert(copy != NULL);
  strcpy(copy, s);
  return copy;
}

/*
 * layout:     the compressed bytes, written the way we draw them on the board
 * compressed: the same bytes as a C string. "\x12" "a" is the header 0x12
 *             (len 1, run 2) followed by 'a'. The literals are split because
 *             "\x12a" would be read as ONE hex escape.
 */
/* prints at most 40 chars of s */
static void show(const char *label, const char *s) {
  if (s == NULL) {
    printf("    %8s NULL\n", label);
    return;
  }
  int len = (int)strlen(s);
  printf("    %8s \"%.40s\"%s  (%d chars)\n", label, s, len > 40 ? "..." : "", len);
}

static void test(const char *layout, const char *compressed, const char *expected) {
  char *data = heap_copy(compressed);
  Decompress(&data);

  int ok = data != NULL && strcmp(data, expected) == 0;
  total++;
  if (ok) passed++;
  printf("  [%s] %s\n", ok ? " OK " : "FAIL", layout);
  if (!ok) {
    show("got", data);
    show("expected", expected);
  }
  free(data);
}

/* expected text for `blocks` blocks of 11111111|15 chars: each piece 15 times */
static void fill_expected(char *buf, int blocks) {
  const char *pieces[] = {"ABCDEFGHIJKLMNO", "abcdefghijklmno", "0123456789!@#$%", "PQRSTUVWXYZpqrs"};
  char *out = buf;
  for (int b = 0; b < blocks; b++)
    for (int r = 0; r < 15; r++) {
      memcpy(out, pieces[b], 15);
      out += 15;
    }
  *out = '\0';
}

int main(void) {
  setbuf(stdout, NULL); // print right away, so a crash shows which test it was
  printf("\n=== Decompress ===\n");

  test("00010010|a|00100011|b|c|00010001|x",
       "\x12" "a" "\x23" "bc" "\x11" "x",
       "aabcbcbcx");

  test("00010001|y|00110010|f|o|o|00110001|b|a|r",
       "\x11" "y" "\x32" "foo" "\x31" "bar",
       "yfoofoobar");

  test("00010011|z                       (one block)",
       "\x13" "z",
       "zzz");

  test("00011111|-                       (run = 15)",
       "\x1F" "-",
       "---------------");

  test("(nothing, just the '\\0')        (empty input -> empty string)",
       "",
       "");

  test("10000001|a|b|c|d|e|f|g|h|00110010|f|o|o   (header >= 128!)",
       "\x81" "abcdefgh" "\x32" "foo",
       "abcdefghfoofoo");

  char big[4 * 15 * 15 + 1];
  fill_expected(big, 1);
  test("11111111|A|B|...|O                (len 15, run 15 -> 225 chars)",
       "\xFF" "ABCDEFGHIJKLMNO",
       big);

  fill_expected(big, 4);
  test("4 x 11111111|15 chars             (900 chars, no fixed-size buffers)",
       "\xFF" "ABCDEFGHIJKLMNO" "\xFF" "abcdefghijklmno" "\xFF" "0123456789!@#$%" "\xFF" "PQRSTUVWXYZpqrs",
       big);

  // the Georgian letter "a" is 3 bytes in UTF-8: E1 83 90. len counts BYTES.
  test("00110010|E1|83|90                 (UTF-8: len counts bytes)",
       "\x32" "\xE1\x83\x90",
       "\xE1\x83\x90" "\xE1\x83\x90");

  printf("\n  %d / %d passed\n", passed, total);
  return 0;
}
