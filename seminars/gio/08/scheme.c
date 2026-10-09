#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  Integer, String, List, Nil
} nodeType;


static char *ConcatStrings(const char* first, const char* second){
  size_t len1 = strlen(first);
  size_t total = len1 + strlen(second) + 1;
  char* result = malloc(total);
  assert(result != NULL);
  strcpy(result, first);
  strcpy(result + len1, second);
  return result;
}



/*
 * Every node starts with its nodeType (the tag). What comes after depends on it:
 *
 *   Integer   [ Integer | int ]
 *   String    [ String  | Y a n k e e s \0 ]     the chars are INSIDE the node
 *   List      [ List    | first | rest ]         two addresses: the element, the rest of the list
 *   Nil       [ Nil ]                            the end of a list
 *
 * Traverses a properly structured list, and returns the ordered
 * concatenation of all strings, including those in nested sublists.
 * Integers are skipped. The returned string is malloc'd, the caller frees it.
 *
 *   (Yankees 2 Diamondbacks 1)   ->  "YankeesDiamondbacks"
 *   (one (2 (three 4)) 5 six)    ->  "onethreesix"
 */

char *ConcatAll(nodeType *list) {
  if(*list == Integer || *list == Nil){
    return strdup("");
  }

  if(*list == String){
    return strdup((char *)(list + 1));
  }

  nodeType ** pair = (nodeType **)(list + 1);
  char* first = ConcatAll(pair[0]);
  char* rest = ConcatAll(pair[1]);
  char* result = ConcatStrings(first, rest);
  free(first);
  free(rest);
  return result;

}

/* ---------------------------------------------------------------- */
/*  tests - don't touch                                             */
/*    ./scheme                       all tests                      */
/*    make asan && ./scheme_asan     also finds leaks               */
/* ---------------------------------------------------------------- */

/*
 * Builds the nodes for a list written the Scheme way: "(one (2 (three 4)) 5 six)".
 * A word made of digits becomes an Integer node, any other word a String node,
 * and every list ends with its own Nil node, like in the handout's pictures.
 */
static nodeType *NewNode(nodeType type, size_t payload) {
  nodeType *node = malloc(sizeof(nodeType) + payload);
  assert(node != NULL);
  *node = type;
  return node;
}

static nodeType *ParseElement(const char **s);

/* *s is inside a list: parses the elements up to and including the ')' */
static nodeType *ParseRest(const char **s) {
  while (**s == ' ') (*s)++;
  if (**s == ')') {
    (*s)++;
    return NewNode(Nil, 0);
  }
  nodeType *first = ParseElement(s);
  nodeType *rest = ParseRest(s);
  nodeType *node = NewNode(List, 2 * sizeof(nodeType *));
  ((nodeType **)(node + 1))[0] = first;
  ((nodeType **)(node + 1))[1] = rest;
  return node;
}

static nodeType *ParseElement(const char **s) {
  if (**s == '(') {
    (*s)++;
    return ParseRest(s);
  }
  const char *start = *s;
  int digits = 1;
  while (**s != ' ' && **s != '(' && **s != ')' && **s != '\0') {
    if (**s < '0' || **s > '9') digits = 0;
    (*s)++;
  }
  int len = *s - start;
  if (digits) {
    nodeType *node = NewNode(Integer, sizeof(int));
    *(int *)(node + 1) = atoi(start);
    return node;
  }
  nodeType *node = NewNode(String, len + 1);
  memcpy(node + 1, start, len);
  ((char *)(node + 1))[len] = '\0';
  return node;
}

static void FreeNodes(nodeType *node) {
  if (*node == List) {
    FreeNodes(((nodeType **)(node + 1))[0]);
    FreeNodes(((nodeType **)(node + 1))[1]);
  }
  free(node);
}

static int passed = 0, total = 0;

/* prints at most 60 chars of s */
static void show(const char *label, const char *s) {
  if (s == NULL) {
    printf("    %8s NULL\n", label);
    return;
  }
  int len = (int)strlen(s);
  printf("    %8s \"%.60s\"%s  (%d chars)\n", label, s, len > 60 ? "..." : "", len);
}

static void test(const char *scheme, const char *expected) {
  const char *s = scheme;
  nodeType *list = ParseElement(&s);
  char *result = ConcatAll(list);

  int ok = result != NULL && strcmp(result, expected) == 0;
  total++;
  if (ok) passed++;
  printf("  [%s] %.60s%s\n", ok ? " OK " : "FAIL", scheme, strlen(scheme) > 60 ? "..." : "");
  if (!ok) {
    show("got", result);
    show("expected", expected);
  }
  free(result);     // the result is ours: ConcatAll has to return malloc'd memory
  FreeNodes(list);  // the list is still ours: ConcatAll must not free or change it
}

int main(void) {
  setbuf(stdout, NULL); // print right away, so a crash shows which test it was
  printf("\n=== ConcatAll ===\n");
  printf("  sizeof(nodeType) = %d, sizeof(nodeType *) = %d\n\n",
         (int)sizeof(nodeType), (int)sizeof(nodeType *));

  // the two lists drawn in the handout
  test("(Yankees 2 Diamondbacks 1)", "YankeesDiamondbacks");
  test("(one (2 (three 4)) 5 six)", "onethreesix");

  // the other examples from the handout
  test("(2 3 5 7)", "");
  test("(House at Pooh Corner)", "HouseatPoohCorner");
  test("(4 calling birds 3 French hens 2 turtle doves 1 partridge)",
       "callingbirdsFrenchhensturtledovespartridge");
  test("((1 2) (buckle my shoe))", "bucklemyshoe");
  test("(how (nested (can (u (go)))) how (nested (can (u (go)))))",
       "hownestedcanugohownestedcanugo");

  test("()", "");                          // the empty list is a single Nil node
  test("(a () b (()) c)", "abc");          // empty sublists as elements
  test("((((((((((deep))))))))))", "deep");

  // 500 x "ab 7": no fixed-size buffers
  char scheme[1 + 500 * 5 + 2], expected[500 * 2 + 1];
  scheme[0] = '(';
  for (int i = 0; i < 500; i++) {
    memcpy(scheme + 1 + i * 5, "ab 7 ", 5);
    memcpy(expected + i * 2, "ab", 2);
  }
  strcpy(scheme + 1 + 500 * 5, ")");
  expected[500 * 2] = '\0';
  test(scheme, expected);

  printf("\n  %d / %d passed\n", passed, total);
  return 0;
}
