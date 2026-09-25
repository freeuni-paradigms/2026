## Seminar 04 - strcpy & casting

### Commands

- Compile everything  -  `make`
- strcpy tests  -  `./strcpy`
- strcpy demos (overflow, strcpy into an int array)  -  `./strcpy demo`
- Casting, all sections  -  `./casting`
- Casting, one section  -  `./casting 5`
- Section 8 with optimizations  -  `make casting_O2 && ./casting_O2 8`
- What the compiler warns about  -  `make warnings`
- Clean  -  `make clean`

### Files
- `strcpy.c` - empty `my_strcpy` + tests, we implement it together
- `strcpy_solution.c` - index / pointer / K&R versions + `strncpy`
- `casting.c` - casting examples, every variable is printed together with its bits

---

## Part 1 - strcpy

```c
char *strcpy(char *dest, const char *src);

```
Plan:
1. Write the version with indices **without** `dest[i] = '\0'` and run `./strcpy`.
   Test 2 prints `QWERTYAAAAAAAAA`: the buffer was full of `A`s and nobody wrote the `'\0'`.
2. Fix it. Then: why return `dest`? (chaining: `printf("%s", strcpy(buf, "hi"))`)
3. Rewrite it with pointers: `src[i]` is the same as `*(src + i)`.
4. K&R one-liner: `while ((*dest++ = *src++));`
5. `./strcpy demo`:
   - dest is too small: strcpy doesn't know the size and writes into the neighbor
   - `strcpy((char *)int_array, "abc")` -> `0x00636261`, strcpy only sees bytes
6. Bonus: `strncpy` doesn't write `'\0'` when src is too long; overlapping `strcpy(s + 1, s)`

---

## Part 2 - casting

| conversion | what happens to the bits |
|---|---|
| small -> big, **signed** source | sign extension (the sign bit is copied) |
| small -> big, **unsigned** source | zero extension |
| big -> small | the high bits are cut off |
| signed <-> unsigned, same size | nothing, only the reading changes |
| `(int)f` / `(float)i` | the value is converted, the bits are recomputed |
| `*(int *)&f` | nothing, the same bytes are read as another type |

Sections in `./casting`:
1. widening: sign extension vs zero extension
2. is `char` signed or unsigned? (implementation-defined), `ch == 0xFF` is false
3. narrowing: `(short)36881 = -28655`, `(char)300 = 44`
4. `-1` and `4294967295` have the same bits
5. `(unsigned)(short)-1` vs `(unsigned)(unsigned short)-1`
6. integer promotion: `char + char` is an `int`, `~` on `unsigned char`, `sizeof('a') == 4`
7. `-1 < 1u` is false, `strlen(a) - strlen(b) < 0` is never true, `unsigned` loop down to 0
8. unsigned overflow is defined, signed overflow is UB (`-O0` vs `-O2`)
9. `(int)-2.9 == -2`, `(float)16777217 == 16777216`, `0.1 + 0.2 != 0.3`
10. `(int)1.25f == 1` vs `*(int *)&f == 1067450368`, IEEE 754 layout
11. little-endian, looking at an `int` through `char *` / `short *`, pointer arithmetic

### Predict the output

```c
char c = -5;           int i = c;                     // 1
unsigned char u = 251; int j = u;                     // 2
short s = (short)40000;                               // 3
char c2 = (char)300;                                  // 4
unsigned int x = (unsigned int)-1;                    // 5
short m = -1; unsigned int y = (unsigned short)m;     // 6
-1 < 1u                                               // 7
(short)-1 < (unsigned short)1                         // 8
(int)-2.9f                                            // 9
float f = 1.25f; *(int *)&f                           // 10
int z = 0x01020304; *(char *)&z                       // 11
sizeof('a')                                           // 12
```

<details>
<summary>answers</summary>

1. `-5` - sign extension
2. `251` - zero extension
3. `-25536` - 40000 - 65536
4. `44` - 300 - 256
5. `4294967295` - same bits as -1
6. `65535` - first `unsigned short`, then zero extension
7. `false` - -1 becomes unsigned
8. `true` - both become `int`
9. `-2` - truncation toward zero
10. `1067450368` - `0 01111111 0100...0`
11. `4` - little-endian
12. `4` - `'a'` is an `int` in C

</details>

### printf format specifiers
https://en.cppreference.com/w/c/io/fprintf.html

| specifier | type |
|---|---|
| `%d` / `%u` | int / unsigned int (char and short are promoted to int) |
| `%hd` / `%hhd` | short / char printed as a number |
| `%zu` | size_t |
| `%x` | hex |
| `%f` / `%g` | double (float is promoted to double) |
| `%p` | pointer (`void *`) |

printf does **not** convert: `printf("%f", 5)` is undefined behaviour, write `printf("%f", (float)5)`.
