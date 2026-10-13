#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A sorted set stored as a PACKED binary search tree: one malloc'd block
 * with the index of the root, and then the nodes one after another:
 *
 *   [ root ][ elem | left | right ][ elem | left | right ][ elem | left | right ] ...
 *            node 0                 node 1                 node 2
 *
 * root, left and right are node indexes. -1 means "no node" (the NULL of indexes).
 */
typedef struct {
  void* base; // the block [root][node0][node1]...
  int elemSize; // bytes in one client element
  int count; // how many nodes are used
  int capacity; // how many nodes fit in the block
  int (*cmpfn)(const void *, const void *);
} sortedset;


static int NodeSize(const sortedset * set){
  return set->elemSize + 2*sizeof(int);
}


static void* NodeAt(const sortedset* set, int index){
  return (char*)set->base + sizeof(int) + index * NodeSize(set);
}

static int* ChildrenOf(const sortedset* set, const void* elemPtr){
  return (int*)((char*)elemPtr + set->elemSize);
}


static int* FindNode(sortedset* set, const void* elemPtr){
  int* link = (int*)set->base;
  while(*link != -1){
    void* node = NodeAt(set, *link);
    int cmp = set->cmpfn(elemPtr, node);
    if(cmp == 0) return link;
    int* children = ChildrenOf(set, node);
    if(cmp < 0) link = &children[0];
    else link = &children[1];
  }
  return link;
}


static const int kInitialCapacity = 4;

/* allocates the block for kInitialCapacity nodes and makes the set empty */
void SetNew(sortedset *set, int elemSize, int (*cmpfn)(const void *, const void *)) {
  set->elemSize = elemSize;
  set->cmpfn = cmpfn;
  set->count = 0;
  set->capacity = kInitialCapacity;
  set->base = malloc(sizeof(int) + set->capacity * NodeSize(set));
  assert(set->base != NULL);
  *(int*)set->base = -1;

}

/* returns the address of the matching element INSIDE the set, or NULL */
void *SetSearch(sortedset *set, const void *elemPtr) {
  int * link = FindNode(set, elemPtr);
  if(*link == -1) return NULL;
  return NodeAt(set, *link);
}

/* copies the element into the set if it isn't there yet. true iff it was copied */
bool SetAdd(sortedset *set, const void *elemPtr) {
  if(set->count == set->capacity){
    set->capacity *= 2;
    set->base = realloc(set->base, sizeof(int) + set->capacity * NodeSize(set));
    assert(set->base != NULL);
  }

  int * link = FindNode(set, elemPtr);
  if(*link != -1) return false;

  int index = set->count;
  void* node = NodeAt(set, index);
  memcpy(node, elemPtr, set->elemSize);
  int* children = ChildrenOf(set, node);
  children[0] = -1;
  children[1] = -1;
  *link = index;
  set->count++;
  return true;
  
}

/* frees the block. (Not in the handout: the tests need it, so ASan sees no leaks) */
void SetDispose(sortedset *set) {
  free(set->base);
}

/* ---------------------------------------------------------------- */
/*  tests - don't touch                                             */
/*    ./sortedset                        all tests                  */
/*    make asan && ./sortedset_asan      also finds leaks and       */
/*                                       use after free             */
/* ---------------------------------------------------------------- */

static int IntCompare(const void *a, const void *b) {
  return *(int *)a - *(int *)b;      // fine for small ints (no overflow)
}

/* the elements are char *, so the set hands us the ADDRESSES of two char * */
static int StringPtrCompare(const void *a, const void *b) {
  return strcmp(*(char **)a, *(char **)b);
}

typedef struct {
  double x, y;
} pointT;

/* two points are "equal" when they're the same distance from (0, 0) */
static int DistanceCompare(const void *a, const void *b) {
  const pointT *p = a;
  const pointT *q = b;
  double dp = p->x * p->x + p->y * p->y;   // distance squared: no sqrt needed
  double dq = q->x * q->x + q->y * q->y;
  if (dp < dq) return -1;
  if (dp > dq) return 1;
  return 0;
}

static int passed = 0, total = 0;
static char why[200];   // what went wrong, printed under a FAIL

/* prints the name first, so if a test hangs or crashes you can see which one */
static void start(const char *name) {
  printf("  [ .. ] %s", name);
  why[0] = '\0';
  total++;
}

static void finish(bool ok) {
  printf("\r  [%s]\n", ok ? " OK " : "FAIL");
  if (ok) passed++;
  else printf("         %s\n", why);
}

/* the handout's tree: node 0 = 40, node 1 = 20, ..., node 6 = 50 */
static const int handout[] = {40, 20, 70, 55, 60, 10, 50};
static const char *handoutBlock = "0 | 40 1 2 | 20 5 -1 | 70 3 -1 | 55 6 4 | 60 -1 -1 | 10 -1 -1 | 50 -1 -1";

/* writes the block as text: "root | elem left right | elem left right | ..." */
static void ReadBlock(const char *block, int nodes, char *out) {
  int root;
  memcpy(&root, block, sizeof(int));
  out += sprintf(out, "%d", root);
  for (int i = 0; i < nodes; i++) {
    int node[3];   // elem, left, right
    memcpy(node, block + sizeof(int) + i * sizeof(node), sizeof(node));
    out += sprintf(out, " | %d %d %d", node[0], node[1], node[2]);
  }
}

static void TestEmpty(void) {
  start("empty set: SetSearch -> NULL");
  sortedset set;
  SetNew(&set, sizeof(int), IntCompare);
  int key = 40;
  bool ok = SetSearch(&set, &key) == NULL;
  if (!ok) sprintf(why, "SetSearch(40) on an empty set didn't return NULL");
  SetDispose(&set);
  finish(ok);
}

static void TestHandoutTree(void) {
  sortedset set;
  SetNew(&set, sizeof(int), IntCompare);

  start("add 40 20 70 55 60 10 50 (the handout's tree) -> all true");
  bool ok = true;
  for (int i = 0; i < 7; i++) {
    if (!SetAdd(&set, &handout[i]) && ok) {
      sprintf(why, "SetAdd(%d) returned false", handout[i]);
      ok = false;
    }
  }
  finish(ok);

  start("the packed block is exactly the handout's picture (page 2)");
  int root = 40;
  char *first = SetSearch(&set, &root);   // node 0 is 4 bytes after the start of the block
  char *block = first == NULL ? NULL : first - sizeof(int);
  char got[200];
  if (block == NULL) {
    sprintf(why, "can't look at the block: SetSearch(40) returned NULL");
    finish(false);
  } else {
    ReadBlock(block, 7, got);
    ok = strcmp(got, handoutBlock) == 0;
    if (!ok) sprintf(why, "expected:  %s", handoutBlock);
    finish(ok);
    printf("         block:     %s\n", got);
  }

  start("SetSearch finds all 7, and returns the address INSIDE the set");
  ok = block != NULL;
  if (!ok) sprintf(why, "SetSearch(40) returned NULL");
  for (int i = 0; i < 7 && ok; i++) {
    int key = handout[i];
    char *found = SetSearch(&set, &key);
    char *node = block + sizeof(int) + i * (sizeof(int) + 2 * sizeof(int));
    if (found == NULL) {
      sprintf(why, "SetSearch(%d) returned NULL", key);
      ok = false;
    } else if (found == (char *)&key) {
      sprintf(why, "SetSearch(%d) returned elemPtr itself, not the copy inside the set", key);
      ok = false;
    } else if (found != node) {
      sprintf(why, "SetSearch(%d) should return node %d = block + %d, got block + %d",
              key, i, (int)(node - block), (int)(found - block));
      ok = false;
    }
  }
  finish(ok);

  start("SetSearch 5 30 45 65 80 (not in the set) -> NULL");
  int missing[] = {5, 30, 45, 65, 80};
  ok = block != NULL;
  if (!ok) sprintf(why, "the set looks empty: SetSearch(40) returned NULL");
  for (int i = 0; i < 5 && ok; i++) {
    if (SetSearch(&set, &missing[i]) != NULL) {
      sprintf(why, "SetSearch(%d) didn't return NULL", missing[i]);
      ok = false;
    }
  }
  finish(ok);

  start("SetAdd 40 and 55 again -> false, the block doesn't change");
  int again[] = {40, 55};
  ok = block != NULL;
  if (!ok) sprintf(why, "the set looks empty: SetSearch(40) returned NULL");
  for (int i = 0; i < 2 && ok; i++) {
    if (SetAdd(&set, &again[i])) {
      sprintf(why, "SetAdd(%d) returned true the 2nd time", again[i]);
      ok = false;
    }
  }
  if (ok) {
    first = SetSearch(&set, &root);
    if (first == NULL) {
      sprintf(why, "SetSearch(40) returned NULL");
      ok = false;
    } else {
      ReadBlock(first - sizeof(int), 7, got);
      if (strcmp(got, handoutBlock) != 0) {
        sprintf(why, "the block changed: %.150s", got);
        ok = false;
      }
    }
  }
  finish(ok);

  SetDispose(&set);
}

static void TestStrings(void) {
  start("char * elements: \"fig\" twice -> false, equal by strcmp, not by address");
  sortedset set;
  SetNew(&set, sizeof(char *), StringPtrCompare);

  const char *words[] = {"pear", "apple", "fig", "kiwi", "fig", "plum", "apple", "lime"};
  bool expected[] = {true, true, true, true, false, true, false, true};
  char *kept[8];   // the strings the set took: we free them at the end
  int numKept = 0;
  bool ok = true;
  for (int i = 0; i < 8; i++) {
    char *name = strdup(words[i]);   // a new copy: the 2nd "fig" is at another address
    bool added = SetAdd(&set, &name);
    if (added) kept[numKept++] = name;
    else free(name);                 // the handout's usage: if (!SetAdd(...)) free(name);
    if (added != expected[i] && ok) {
      sprintf(why, "SetAdd(\"%s\") returned %s", words[i], added ? "true" : "false");
      ok = false;
    }
  }

  char text[] = "kiwi";              // the same chars, at another address
  char *key = text;
  char **found = SetSearch(&set, &key);
  if (ok) {
    if (found == NULL) {
      sprintf(why, "SetSearch(\"kiwi\") returned NULL");
      ok = false;
    } else if (found == &key) {
      sprintf(why, "SetSearch(\"kiwi\") returned elemPtr itself, not the copy inside the set");
      ok = false;
    } else if (*found != kept[3]) {
      sprintf(why, "SetSearch(\"kiwi\") should find the char * we added (the set copies the pointer)");
      ok = false;
    }
  }
  char text2[] = "grape";
  key = text2;
  if (ok && SetSearch(&set, &key) != NULL) {
    sprintf(why, "SetSearch(\"grape\") didn't return NULL");
    ok = false;
  }
  finish(ok);

  SetDispose(&set);
  for (int i = 0; i < numKept; i++) free(kept[i]);
}

static void TestPoints(void) {
  start("pointT (16 bytes): DistanceCompare decides what \"equal\" means");
  sortedset set;
  SetNew(&set, sizeof(pointT), DistanceCompare);

  pointT points[] = {{3, 4}, {1, 0}, {6, 8}, {0, 5}, {2, 2}, {-3, -4}, {5, 12}};
  bool expected[] = {true, true, true, false, true, false, true};   // (0,5), (-3,-4): distance 5, like (3,4)
  bool ok = true;
  for (int i = 0; i < 7 && ok; i++) {
    if (SetAdd(&set, &points[i]) != expected[i]) {
      sprintf(why, "SetAdd((%g, %g)) returned %s", points[i].x, points[i].y,
              expected[i] ? "false" : "true");
      ok = false;
    }
  }
  pointT key = {5, 0};   // distance 5: finds the (3, 4) inside the set
  pointT *found = SetSearch(&set, &key);
  if (ok && (found == NULL || found == &key || found->x != 3 || found->y != 4)) {
    sprintf(why, "SetSearch((5, 0)) should return the stored (3, 4)");
    ok = false;
  }
  pointT origin = {0, 0};
  if (ok && SetSearch(&set, &origin) != NULL) {
    sprintf(why, "SetSearch((0, 0)) didn't return NULL");
    ok = false;
  }
  finish(ok);

  SetDispose(&set);
}

static void TestMany(const char *name, int step) {
  start(name);
  sortedset set;
  SetNew(&set, sizeof(int), IntCompare);

  // A real program mallocs other things too. These stay right after the set's
  // block, so realloc can't just make the block longer: it has to MOVE it.
  void *others[1000];
  bool ok = true;
  for (int i = 0; i < 1000; i++) {
    int value = i * step % 1000;
    if (!SetAdd(&set, &value) && ok) {
      sprintf(why, "SetAdd(%d) returned false", value);
      ok = false;
    }
    others[i] = malloc(16);
  }
  for (int i = 0; i < 1000 && ok; i++) {
    int *found = SetSearch(&set, &i);
    if (found == NULL || found == &i || *found != i) {
      sprintf(why, "SetSearch(%d) didn't find it", i);
      ok = false;
    }
  }
  for (int i = 1000; i < 1100 && ok; i++) {
    if (SetSearch(&set, &i) != NULL) {
      sprintf(why, "SetSearch(%d) didn't return NULL", i);
      ok = false;
    }
  }
  for (int i = 0; i < 1000 && ok; i++) {
    if (SetAdd(&set, &i)) {
      sprintf(why, "SetAdd(%d) returned true the 2nd time", i);
      ok = false;
    }
  }
  finish(ok);

  SetDispose(&set);
  for (int i = 0; i < 1000; i++) free(others[i]);
}

int main(void) {
  setbuf(stdout, NULL); // print right away, so a crash shows which test it was
  printf("\n=== sortedset ===\n");
  printf("  sizeof(int) = %d, sizeof(char *) = %d, sizeof(pointT) = %d\n\n",
         (int)sizeof(int), (int)sizeof(char *), (int)sizeof(pointT));

  TestEmpty();
  TestHandoutTree();
  TestStrings();
  TestPoints();
  TestMany("1000 ints, mixed order (0 37 74 ...): realloc 8 times", 37);
  TestMany("1000 ints, sorted order (0 1 2 ...): the tree is one long line", 1);

  printf("\n  %d / %d passed\n", passed, total);
  return 0;
}
