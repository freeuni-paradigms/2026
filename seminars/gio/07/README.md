## Seminar 07 - Decompress (run-length encoding)

### Commands

- Compile everything  -  `make`
- Tests  -  `./decompress`
- Finished version, same tests  -  `./decompress_solution`
- AddressSanitizer build: also finds leaks (Linux / WSL / macOS)  -  `make asan && ./decompress_asan`
- Clean  -  `make clean`

### Files

- `decompress.c` - empty `Decompress` + tests. We fill it in together.
- `decompress_solution.c` - finished version (the tests are the same).

---

## The problem (an old exam problem, 50 points)

RLE (run-length encoding) compresses data by replacing a piece that repeats with **how long the piece is**, **how many times it repeats**, and the piece itself:

```
aabcbcbcx    ->  <1,2>a <2,3>bc <1,1>x
yfoofoobar   ->  <1,1>y <3,2>foo <3,1>bar
```

Both numbers are at most 15, so both fit into **one byte**: the high 4 bits are the length, and the low 4 bits are the repeat count. So `aabcbcbcx` is stored as:

```
00010010 | a | 00100011 | b | c | 00010001 | x
```

Write the function that decompresses `'\0'`-terminated compressed data:

```c
// Decompresses given RLE compressed data.
// Takes ownership of memory block pointed by passed data pointer.
// Char array pointed by data ends with '\0'.
// After returning data must be pointing to a memory block which contains decompressed
// information ownership of which is passed back to the caller.
void Decompress(char **data);
```

---

## 0. Decode it by hand first

**Board:**

```
 0001 0010 | 'a' | 0010 0011 | 'b' | 'c' | 0001 0001 | 'x' | \0
 len 1           | len 2                 | len 1
 run 2           | run 3                 | run 1
 -> "aa"           -> "bcbcbc"             -> "x"

 "aa" + "bcbcbc" + "x" = "aabcbcbcx"
```

**Say:** "In hex, every hex digit is exactly 4 bits, so you can read len and run right off it: `0x23` is len 2, run 3." The tests write the data like that: `"\x12" "a" "\x23" "bc" "\x11" "x"`.

**Ask:** Let the students encode `"yfoofoobar"` themselves. → `0x11|y|0x32|f|o|o|0x31|b|a|r`

**Ask:** "Can a header byte ever be `0`?" → No. len ≥ 1 and run ≥ 1, so a header is never `0x00`. That's why `'\0'` can safely mark the end of the data.

**Run `./decompress` before writing anything.** The empty function doesn't touch the data, so the tests print the compressed bytes as text:

```
[FAIL] 00010001|y|00110010|f|o|o|00110001|b|a|r
       got "y2foo1bar"  (10 chars)
```

**Ask:** "Where do the `2` and the `1` come from? And why 10 chars when we only see 9?" → They're the headers. `0x32` is the ASCII code of `'2'` and `0x31` is `'1'`. The first header, `0x11`, is a control character, so it's there but it doesn't print. A header is just a byte, and printf has no idea that it's special.

---

## 1. The header byte: `len` and `run`

**Board** (seminar 02):

```
header         = 0010 0011    (0x23)

header >> 4    = 0000 0010  = 2    -> len
header & 0x0F  = 0000 0011  = 3    -> run
         0x0F  = 0000 1111
```

```c
int len = header >> 4;
int run = header & 0x0F;
```

**The trap: a header ≥ 128.** Test 6 has the header `1000 0001` (len 8, run 1). If you read it through a plain `char`:

```
as unsigned char:   129  >> 4  =  0000 1000  =  8
as char:           -127  >> 4  =  1111 1000  = -8     (the sign bit is copied in, seminar 04)
```

What actually happens with `char *p`: tests 1-5 pass, then test 6 dies:

```
Assertion `result != NULL' failed.
Aborted
```

**Ask:** "Why does malloc fail?" → len = -8, so `total += len * run` makes the size_t `total` wrap around to a huge number. Also, `p += 1 + len` moves `p` 7 bytes *backwards*, out of the block.

Two fixes:
- read the data through `unsigned char *` (what we do), or
- mask after shifting: `(header >> 4) & 0x0F` (last year's solution did this).

---

## 2. Walking over the blocks

First only walk and print, without building anything yet:

```c
void Decompress(char **data) {
  unsigned char *p = (unsigned char *)*data;
  while (*p != '\0') {
    int len = *p >> 4;
    int run = *p & 0x0F;
    printf("len %d, run %d\n", len, run);   // just for now
    p += 1 + len;                           // skip the header and the piece
  }
}
```

**Board:**

```
 0x12 | a | 0x23 | b | c | 0x11 | x | \0
  ^          ^              ^         ^
  1          2              3         4

 1: p at 0x12, len 1  ->  p += 1 + 1
 2: p at 0x23, len 2  ->  p += 1 + 2
 3: p at 0x11, len 1  ->  p += 1 + 1
 4: *p == '\0'        ->  stop
```

**Ask:** "Where is the piece?" → It starts at `p + 1` and is `len` bytes long.

**Common mistake:** forgetting to delete the debug `printf`. Last year's solution kept one, and its `len = ... run = ...` lines got mixed into the test output.

---

## 3. How big is the result?

**Ask:** "How many bytes do we need to malloc?" Let them suggest options:

1. `char result[1000]` → No. It's a local array and it dies when the function returns (seminar 06). The 900-char test also breaks any fixed size.
2. `realloc` after every block: it works, see section 6.
3. **Two passes:** first walk the blocks and add up `len * run`, then malloc exactly once.

```c
size_t total = 0;
unsigned char *p = compressed;
while (*p != '\0') {
  int len = *p >> 4;
  int run = *p & 0x0F;
  total += len * run;
  p += 1 + len;
}
char *result = malloc(total + 1);   // +1 for the '\0'
assert(result != NULL);
```

For `aabcbcbcx`: 1·2 + 2·3 + 1·1 = 9, so malloc(10).

---

## 4. Copying a piece `run` times

Second pass: same loop, but now copy. Keep a pointer `out` to where the next byte goes:

```c
char *out = result;
p = compressed;
while (*p != '\0') {
  int len = *p >> 4;
  int run = *p & 0x0F;
  for (int i = 0; i < run; i++) {
    memcpy(out, p + 1, len);
    out += len;
  }
  p += 1 + len;
}
*out = '\0';
```

**Board:**

```
result: [ a | a | b | c | b | c | b | c | x | \0 ]
          ^   ^   ^       ^       ^       ^   ^
          out: where the next piece goes (moves by len)
                                              *out = '\0' at the end
```

**Ask:** "Why `memcpy` and not `strcpy(out, p + 1)`?" → The piece doesn't end with `'\0'`. strcpy would keep copying the next header, the next piece, and so on, until the end of all the data.
**Ask:** "What about `strncpy(out, p + 1, len)`?" → It works, because it copies exactly len bytes when there's no `'\0'` among them. But it never writes a `'\0'` (seminar 04), and memcpy says what we mean: "copy len bytes".
**Ask:** "Last year used `strncat(result, p + 1, len)`. Is that OK?" → It works, but it walks `result` from the start every time to find the end: O(n²), same as strcat in seminar 06.

---

## 5. `char **` and ownership

**Board:**

```
before:

  main                                 Decompress
  char *data  o---+                    char **data  o---> main's variable `data`
                  |
                  v
  heap:   [ 0x12 | a | 0x23 | b | c | 0x11 | x | \0 ]       malloc'd by main


after:

  char *data  o---+
                  v
  heap:   [ a | a | b | c | b | c | b | c | x | \0 ]       malloc'd by Decompress, main frees it

          [ 0x12 | a | 0x23 | ... ]                         freed by Decompress
```

```c
  free(*data);      // "takes ownership": the old block is ours now, so we free it
  *data = result;   // change main's pointer: "ownership is passed back"
}
```

- `*data` is **main's pointer**. That's how we can free main's block and point main at the new one.

**Ask:** "Why `char **` and not `char *`?" → C passes everything by value. With `void Decompress(char *data)` we get a copy of main's pointer, and `data = result` changes only the copy. main still points to the old block, which we just freed. ASan:

```
ERROR: AddressSanitizer: heap-use-after-free
```

It's the same reason `swap` needs `int *`.

**Ask:** "What if we forget `free(*data)`?" → All 9 tests still pass. But:

```
make asan && ./decompress_asan

  9 / 9 passed
ERROR: LeakSanitizer: detected memory leaks
SUMMARY: AddressSanitizer: 127 byte(s) leaked in 9 allocation(s).
```

127 bytes is exactly the 9 compressed inputs. "Takes ownership" in a comment means "you have to free it."

**Common mistake:** `free(*data)` too early. We read the compressed data in both passes, so free it only after the second pass.

`./decompress` → 9/9, and `./decompress_asan` → 9/9 with no leaks.

---

## 6. Other ways, and last year's code

**realloc as you go** (instead of the first pass):

```c
void Decompress(char **data) {
  unsigned char *p = (unsigned char *)*data;
  size_t size = 0;
  char *result = malloc(1);
  assert(result != NULL);

  while (*p != '\0') {
    int len = *p >> 4;
    int run = *p & 0x0F;
    result = realloc(result, size + len * run + 1);
    assert(result != NULL);
    for (int i = 0; i < run; i++) {
      memcpy(result + size, p + 1, len);
      size += len;
    }
    p += 1 + len;
  }
  result[size] = '\0';

  free(*data);
  *data = result;
}
```

- realloc may **move** the block, so never keep an old pointer into it. Here `result + size` is recomputed every time, which is why we don't keep an `out` pointer.
- `result = realloc(result, ...)`: if realloc fails it returns NULL, and the old block is lost (a leak). The assert stops the program there anyway.

**Last year's solutions** (`realloc` + `strncat`) pass all 9 tests, but:
- neither one frees `*data`, so ASan reports the same 127-byte leak;
- one of them printed debug lines into the output;
- `strncat` makes it O(n²).
