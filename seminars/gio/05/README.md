

---

## 0. Rules for the board (write these first and keep them up)

Sizes (32-bit, like the handout):

```
char 1    short 2    int 4    any pointer 4
```

How we draw (this is the official solution's style):

```
       higher addresses
              ^
      +---------------+
      |               |     int / pointer: one box (4 bytes)
      +---+---+---+---+
      |   |   |   |   |     char[4]: 4 cells, left -> right = low -> high address
      +---+---+---+---+
      |       |       |     short[2]: two halves
      +-------+-------+
      |       o-------|-->  pointer: a dot + an arrow to the FIRST byte it points to
      +---------------+
       lower addresses  (the struct's first field = the bottom row)
```

- One row = 4 bytes.
- A struct is a column. Its **first field is at the bottom**, and addresses grow **upwards**.
- Inside a row, addresses grow **from left to right**.
- `array[i + 1]` is the next column to the right. Memory goes up one column and continues at the bottom of the next one.
- `char` arrays get 1-byte cells, `short` arrays get 2-byte halves, and an `int` or pointer gets one full box.
- A pointer is a **dot** in its box plus an **arrow to the first byte** of whatever it points to. In our picture, that's the bottom-left corner.
- Uninitialized memory is garbage, so leave the box empty.
- Memory that isn't ours (out of bounds) gets a **dashed** box.

The five rules we use on every line:

```
1. a[i]  ==  *(a + i)      one step = sizeof(*a) bytes. There is never a bounds check.
2. p->f  ==  (*p).f
3. An array's name inside an expression = the address of its element 0.
   (x.suid is &x.suid[0])
4. A pointer cast changes NO bytes. It only changes the step size, and what * reads or writes.
5. strcpy(dst, src) copies byte by byte, up to and including the '\0'.
   It knows nothing about fields, structs, or where an array ends.
```

The recipe for each line (say it out loud every time):

```
LEFT side:   which address?  which type? (= how many bytes get written)
RIGHT side:  which value?    (a number, an address, or a run of bytes)
Then draw it.
```

**Tip:** before running any code, write the byte offsets into the empty boxes (the "address maps" below). Every question then becomes "find box 36".

---

## Part 1 - Warm-up: `student friends[4]` (25 min)

```c
typedef struct {
    char *name;
    char suid[8];
    int numUnits;
} student;

student friends[4];
friends[0].name = friends[2].suid + 3;
friends[5].numUnits = 21;
strcpy(friends[1].suid, "4041554");
strcpy(friends->name, "Tiger Woods");
strcpy((char *) &friends[0].numUnits, (const char *) &friends[2].numUnits);
```

### Step 0 - one `student`

**Ask:** "How many bytes is one student?" → 4 + 8 + 4 = **16**.

**Board:**

```
        +---------------+
        | 12            |  numUnits    (int)
        +---+---+---+---+
        | 8 | 9 |10 |11 |  suid[4..7]
        +---+---+---+---+
        | 4 | 5 | 6 | 7 |  suid[0..3]  (char[8] = two rows)
        +---+---+---+---+
        | 0             |  name        (char *)
        +---------------+
                           sizeof(student) = 4 + 8 + 4 = 16
```

**Say:** "`name` is only a pointer, 4 bytes. The actual name lives somewhere else. `suid` is different: the 8 chars themselves live *inside* the struct."

If someone asks about their laptop: on 64-bit a pointer is 8 bytes, so a student is 24. We draw 32-bit like the handout, and the method is the same.

### Step 1 - `student friends[4];`

**Draw** 4 columns side by side. Then write the byte offsets in small numbers, counting from the start of `friends`:

```
               friends[0]         friends[1]         friends[2]          friends[3]
           +---------------+  +---------------+  +---------------+   +---------------+
numUnits   | 12            |  | 28            |  | 44            |   | 60            |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[4..7] | 8 | 9 |10 |11 |  |24 |25 |26 |27 |  |40 |41 |42 |43 |   |56 |57 |58 |59 |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[0..3] | 4 | 5 | 6 | 7 |  |20 |21 |22 |23 |  |36 |37 |38 |39 |   |52 |53 |54 |55 |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
name       | 0             |  | 16            |  | 32            |   | 48            |
           +---------------+  +---------------+  +---------------+   +---------------+
```

**Ask:** "What's in there right now?" → Garbage, because nothing is initialized. The numbers are addresses, not contents.

**Point out:** memory goes up column 0 and then continues at the bottom of column 1. Byte 15 is the top-right of `friends[0]`, and byte 16 is the bottom-left of `friends[1]`.

### Step 2 - `friends[0].name = friends[2].suid + 3;`

- **Left:** `friends[0].name` is box 0, a `char *` (4 bytes). We're storing a pointer, so draw a **dot**.
- **Right:** `friends[2].suid` is an array, so it turns into the address of `suid[0]`: 32 + 4 = **36**. `+ 3` on a `char *` moves 3 bytes, so the result is **39**.
- **Draw:** a dot in `friends[0].name` and an arrow to cell 39, which is `friends[2].suid[3]`. That's the last cell of the lower suid row in `friends[2]`.

```
               friends[0]         friends[1]         friends[2]          friends[3]
           +---------------+  +---------------+  +---------------+   +---------------+
numUnits   |               |  |               |  |               |   |               |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[4..7] |   |   |   |   |  |   |   |   |   |  |   |   |   |   |   |   |   |   |   |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[0..3] |   |   |   |   |  |   |   |   |   |  |   |   |   |   |<+ |   |   |   |   |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+ | +---+---+---+---+
name       |       o       |  |               |  |               | | |               |
           +-------|-------+  +---------------+  +---------------+ | +---------------+
                   +-----------------------------------------------+
```

**Ask:** "Could I write `friends[2].suid = ...`?" → No, you can't assign to an array. You can *use* one, and then it's just an address.

**Common mistake:** pointing the arrow at the start of `suid`, or at `friends[2]` as a whole. Aim for the exact byte.

### Step 3 - `friends[5].numUnits = 21;`

**Ask:** "`friends` has 4 elements. Do we get a compile error? A crash?" → Neither one, at most a warning. `friends[5]` is `*(friends + 5)`, which is just 5 × 16 = 80 bytes from the start. C doesn't store an array's length anywhere.

- **Left:** 80 + 12 = **92**, an `int`.
- **Right:** 21.
- **Draw:** two dashed columns to the right of `friends[3]`: `friends[4]` (bytes 64..79) and `friends[5]` (bytes 80..95). Write 21 in the top box of `friends[5]`. Nothing else in the dashed boxes changes.

```
               friends[3]          friends[4]          friends[5]
           +---------------+   + - - - - - - - +   + - - - - - - - +
numUnits   |               |   :               :   :       21      :  <- byte 92
           +---------------+   + - - - - - - - +   + - - - - - - - +
suid[4..7] |               |   :               :   :               :
           +---------------+   + - - - - - - - +   + - - - - - - - +
suid[0..3] |               |   :               :   :               :
           +---------------+   + - - - - - - - +   + - - - - - - - +
name       |               |   :               :   :               :
           +---------------+   + - - - - - - - +   + - - - - - - - +
                 48..63              64..79              80..95
                                         dashed = not ours
```

**Say:** "Whatever lived at byte 92 is now 21. It could have been another variable, or something the function needs in order to return. This is undefined behaviour. It might crash much later, somewhere completely different."

### Step 4 - `strcpy(friends[1].suid, "4041554");`

- **Dest:** `friends[1].suid` → 16 + 4 = **20**.
- **Src:** `"4041554"` is 7 chars **+ `'\0'`** = 8 bytes, so it fills `suid` exactly (bytes 20..27).
- **Draw:** start with the lower row (lower addresses), left to right, then fill the row above it.

```
               friends[0]         friends[1]         friends[2]          friends[3]
           +---------------+  +---------------+  +---------------+   +---------------+
numUnits   |               |  |               |  |               |   |               |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[4..7] |   |   |   |   |  |'5'|'5'|'4'|\0 |  |   |   |   |   |   |   |   |   |   |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[0..3] |   |   |   |   |  |'4'|'0'|'4'|'1'|  |   |   |   |   |<+ |   |   |   |   |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+ | +---+---+---+---+
name       |       o       |  |               |  |               | | |               |
           +-------|-------+  +---------------+  +---------------+ | +---------------+
                   +-----------------------------------------------+
```

**Ask:** "How many bytes did strcpy write?" → 8. The `'\0'` counts too.
**Ask:** "What if the SUID had 8 digits?" → The `'\0'` would land in byte 28, which is `friends[1].numUnits`. No error.

### Step 5 - `strcpy(friends->name, "Tiger Woods");`

- `friends->name` = `(*friends).name` = `friends[0].name`. That's the pointer from step 2, so the value is **39**.
- **Ask first:** "Where does the `'T'` go: into the name box, or where the arrow points?" → Where the arrow points. strcpy receives the *value* of `name`, which is 39. The name box itself doesn't change.
- `"Tiger Woods"` is 11 chars + `'\0'` = **12 bytes**, so it covers bytes 39..50.

| bytes | chars | whose memory |
|---|---|---|
| 39 | `'T'` | `friends[2].suid[3]` |
| 40-43 | `'i' 'g' 'e' 'r'` | `friends[2].suid[4..7]` |
| 44-47 | `' ' 'W' 'o' 'o'` | `friends[2].numUnits` |
| 48-50 | `'d' 's' '\0'` | `friends[3].name` (byte 51 isn't touched) |

```
               friends[0]         friends[1]         friends[2]          friends[3]
           +---------------+  +---------------+  +---------------+   +---------------+
numUnits   |               |  |               |  |' ' 'W' 'o' 'o'|   |               |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[4..7] |   |   |   |   |  |'5'|'5'|'4'|\0 |  |'i'|'g'|'e'|'r'|   |   |   |   |   |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+
suid[0..3] |   |   |   |   |  |'4'|'0'|'4'|'1'|  |   |   |   |'T'|<+ |   |   |   |   |
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+ | +---+---+---+---+
name       |       o       |  |               |  |               | | |'d' 's' \0     |
           +-------|-------+  +---------------+  +---------------+ | +---------------+
                   +-----------------------------------------------+
```

**Say:** "strcpy walked out of `suid`, through an int, out of `friends[2]`, and into the *pointer* of `friends[3]`. It has no idea what a field or a struct is."

For `numUnits` and `name`, write the chars into the full box and don't split it into cells. It's still an int (or a pointer). It just holds bytes that happen to be letters now. The official solution draws it the same way.

### Step 6 - `strcpy((char *) &friends[0].numUnits, (const char *) &friends[2].numUnits);`

**Ask:** "Why the casts?" → `&friends[0].numUnits` is an `int *`, and strcpy wants a `char *`. The cast only swaps the glasses. No bytes change (seminar 04).

- **Src:** byte **44**. Read until the `'\0'`: `' ' 'W' 'o' 'o' 'd' 's' '\0'`. That's the string `" Woods"`, 6 chars + `'\0'` = 7 bytes. Note that the *source* also crosses from `friends[2]` into `friends[3]`.
- **Dest:** byte **12** (`friends[0].numUnits`). The 7 bytes go to 12..18:
  - 12-15: `' ' 'W' 'o' 'o'` → `friends[0].numUnits`
  - 16-18: `'d' 's' '\0'` → `friends[1].name` (byte 19 isn't touched)

**Ask:** "Did we break `friends[1].suid`?" → No. It starts at byte 20, and we stopped at 18.

### Final picture (warm-up)

```
               friends[0]         friends[1]         friends[2]          friends[3]        [4]        [5]
           +---------------+  +---------------+  +---------------+   +---------------+  + - - - +  + - - - +
numUnits   |' ' 'W' 'o' 'o'|  |               |  |' ' 'W' 'o' 'o'|   |               |  :       :  :  21   :
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+  + - - - +  + - - - +
suid[4..7] |   |   |   |   |  |'5'|'5'|'4'|\0 |  |'i'|'g'|'e'|'r'|   |   |   |   |   |  :       :  :       :
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+   +---+---+---+---+  + - - - +  + - - - +
suid[0..3] |   |   |   |   |  |'4'|'0'|'4'|'1'|  |   |   |   |'T'|<+ |   |   |   |   |  :       :  :       :
           +---+---+---+---+  +---+---+---+---+  +---+---+---+---+ | +---+---+---+---+  + - - - +  + - - - +
name       |       o       |  |'d' 's' \0     |  |               | | |'d' 's' \0     |  :       :  :       :
           +-------|-------+  +---------------+  +---------------+ | +---------------+  + - - - +  + - - - +
                   +-----------------------------------------------+                    past the end
```


---

## Part 2 - Meet the Flintstones (45 min)

```c
typedef struct rubble { // need tag name for self-reference
    int betty;
    char barney[4];
    struct rubble *bammbamm;
} rubble;

typedef struct {
    short *wilma[2];
    short fred[2];
    rubble dino;
} flintstone;

rubble *simpsons;
flintstone jetsons[4];

simpsons = &jetsons[0].dino;
jetsons[1].wilma[3] = (short *) &simpsons;
strcpy(simpsons[2].barney, "Bugs Bunny");
((flintstone *)(jetsons->fred))->dino.bammbamm = simpsons;
*(char **)jetson[4].fred = simpsons->barney + 4;
```

**Heads-up:** the last line says `jetson`. That's a typo in the handout for `jetsons` (as written, it doesn't compile). Tell the students before they get stuck on it.

### Step 0 - the two structs

**Draw `rubble`:**

```
      +---------------+
   8  |               |  bammbamm   (rubble *)
      +---+---+---+---+
   4  | 4 | 5 | 6 | 7 |  barney[4]  (char)
      +---+---+---+---+
   0  |               |  betty      (int)
      +---------------+
                         sizeof(rubble) = 4 + 4 + 4 = 12
```

**Ask:** "Why does rubble need the tag name `struct rubble`?" → Inside the braces, the typedef name `rubble` doesn't exist yet. The tag does.
**Ask:** "Could a rubble contain a `rubble` instead of a `rubble *`?" → No, it would be infinitely big. A pointer is always 4 bytes.

**Draw `flintstone`.** Point out that `dino` is not a pointer: the whole rubble lives inside. Draw a dashed box around it (it's the green box in the solution):

```
      +---------------+
  20  |               |  dino.bammbamm  --+
      +---+---+---+---+                   |
  16  |   |   |   |   |  dino.barney[4]   +-- dino = a whole rubble, inside (12 bytes)
      +---+---+---+---+                   |
  12  |               |  dino.betty     --+
      +-------+-------+
   8  |       |       |  fred[0] | fred[1]   (short, 2 bytes each)
      +-------+-------+
   4  |               |  wilma[1]            (short *)
      +---------------+
   0  |               |  wilma[0]            (short *)
      +---------------+
                         sizeof(flintstone) = 8 + 4 + 12 = 24
```

**Ask:** "What's sizeof(flintstone)?" → 8 + 4 + 12 = **24**.

### Step 1 - `rubble *simpsons;` and `flintstone jetsons[4];`

- `simpsons` is one 4-byte box of its own, drawn above `jetsons[0]`. It's a pointer, but it doesn't point anywhere yet (garbage).
- `jetsons` is 4 columns of 6 rows, 96 bytes. Write the offsets:

```
                 simpsons
            +---------------+
            |               |
            +---------------+

                jetsons[0]         jetsons[1]         jetsons[2]         jetsons[3]
            +---------------+  +---------------+  +---------------+  +---------------+
bammbamm    | 20            |  | 44            |  | 68            |  | 92            |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
barney      |16 |17 |18 |19 |  |40 |41 |42 |43 |  |64 |65 |66 |67 |  |88 |89 |90 |91 |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
betty       | 12            |  | 36            |  | 60            |  | 84            |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
fred        |   8   |   10  |  |   32  |   34  |  |   56  |   58  |  |   80  |   82  |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
wilma[1]    | 4             |  | 28            |  | 52            |  | 76            |
            +---------------+  +---------------+  +---------------+  +---------------+
wilma[0]    | 0             |  | 24            |  | 48            |  | 72            |
            +---------------+  +---------------+  +---------------+  +---------------+
```

**Say:** "Leave space above and to the left of `jetsons[0]`. Spoiler: every arrow in this problem lands either in `jetsons[0]` or in `simpsons`."

From here on, the pictures show an arrow as **`o X`** in the pointer's box and **`X>`** where it lands. On the board, draw a real arrow.

### Step 2 - `simpsons = &jetsons[0].dino;`

- **Left:** `simpsons`, a `rubble *`, so we draw a dot.
- **Right:** `jetsons[0]` is at 0 and `.dino` is +12, so the result is **12**. `&` gives the address 12. The type is `rubble *`, which matches, so no cast is needed.
- **Draw:** a dot in `simpsons`, and arrow **A** to the bottom-left corner of `jetsons[0].dino` (the betty row).

```
                 simpsons
            +---------------+
            |      o A      |
            +---------------+

                jetsons[0]         jetsons[1]         jetsons[2]         jetsons[3]
            +---------------+  +---------------+  +---------------+  +---------------+
bammbamm    |               |  |               |  |               |  |               |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
barney      |   |   |   |   |  |   |   |   |   |  |   |   |   |   |  |   |   |   |   |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
betty     A>|               |  |               |  |               |  |               |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
fred        |       |       |  |       |       |  |       |       |  |       |       |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
wilma[1]    |               |  |               |  |               |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
wilma[0]    |               |  |               |  |               |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
```

**Ask:** "Why aim at the bottom-left corner, and not the middle of the dino box?" → A pointer holds the address of the *first* byte, and in our picture the first byte is at the bottom left.

### Step 3 - `jetsons[1].wilma[3] = (short *) &simpsons;`

- `wilma` has 2 elements, but `[3]` doesn't care: `wilma[3]` = `*(wilma + 3)`, and one step is sizeof(short *) = 4.
- **Left:** `jetsons[1]` is at 24, and `wilma` is at +0. Count on the board, going up through `jetsons[1]`:

```
wilma + 0  ->  24   jetsons[1].wilma[0]
wilma + 1  ->  28   jetsons[1].wilma[1]
wilma + 2  ->  32   jetsons[1].fred           (out of bounds)
wilma + 3  ->  36   jetsons[1].dino.betty     (out of bounds)   <- we write here
```

- **Right:** `&simpsons` is the address of **the simpsons box itself**. Its type is `rubble **`; the cast to `short *` only makes the types match, and no bytes change.
- **Draw:** a dot in the betty row of `jetsons[1]`, and arrow **B** to the simpsons box.

```
                 simpsons
            +---------------+
          B>|      o A      |
            +---------------+

                jetsons[0]         jetsons[1]         jetsons[2]         jetsons[3]
            +---------------+  +---------------+  +---------------+  +---------------+
bammbamm    |               |  |               |  |               |  |               |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
barney      |   |   |   |   |  |   |   |   |   |  |   |   |   |   |  |   |   |   |   |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
betty     A>|               |  |      o B      |  |               |  |               |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
fred        |       |       |  |       |       |  |       |       |  |       |       |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
wilma[1]    |               |  |               |  |               |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
wilma[0]    |               |  |               |  |               |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
```

**Common mistake:** drawing arrow B to `jetsons[0].dino`, which is where simpsons *points*. That's wrong: `&simpsons` is *where simpsons lives*, not what it contains.

**Say:** "betty is an int, and now it holds an address. Memory doesn't care."

### Step 4 - `strcpy(simpsons[2].barney, "Bugs Bunny");`

- `simpsons` is a `rubble *`, so one step is sizeof(rubble) = **12**. Not 24, and not 4.
- `simpsons[2]` = 12 + 2 × 12 = **36**. Draw rubble-sized brackets on the board:

```
simpsons[0]  ->  12..23   jetsons[0].dino                        (a real rubble)
simpsons[1]  ->  24..35   jetsons[1].wilma[0], wilma[1], fred    (a fake rubble)
simpsons[2]  ->  36..47   jetsons[1].dino                        (a real rubble again!)
```

Since 2 × 12 = 24 = the size of one flintstone, `simpsons[2]` lands exactly on `jetsons[1].dino`.

- **Dest:** `simpsons[2].barney` = 36 + 4 = **40**.
- `"Bugs Bunny"` is 10 chars + `'\0'` = **11 bytes**, so it covers bytes 40..50.

| bytes | chars | whose memory |
|---|---|---|
| 40-43 | `'B' 'u' 'g' 's'` | `jetsons[1].dino.barney` |
| 44-47 | `' ' 'B' 'u' 'n'` | `jetsons[1].dino.bammbamm` (a pointer, now full of letters) |
| 48-50 | `'n' 'y' '\0'` | `jetsons[2].wilma[0]` (byte 51 isn't touched) |

```
                 simpsons
            +---------------+
          B>|      o A      |
            +---------------+

                jetsons[0]         jetsons[1]         jetsons[2]         jetsons[3]
            +---------------+  +---------------+  +---------------+  +---------------+
bammbamm    |               |  |' ' 'B' 'u' 'n'|  |               |  |               |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
barney      |   |   |   |   |  |'B'|'u'|'g'|'s'|  |   |   |   |   |  |   |   |   |   |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
betty     A>|               |  |      o B      |  |               |  |               |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
fred        |       |       |  |       |       |  |       |       |  |       |       |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
wilma[1]    |               |  |               |  |               |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
wilma[0]    |               |  |               |  |'n' 'y' \0     |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
```

**Say:** "It's the same thing as Tiger Woods. strcpy runs out of barney, through a pointer field, and into the next struct."

### Step 5 - `((flintstone *)(jetsons->fred))->dino.bammbamm = simpsons;`

Take it apart from the inside out, and write each piece on the board:

1. `jetsons->fred` = `jetsons[0].fred`. It's an array, so it becomes the address **8**.
2. `(flintstone *)` means "pretend a whole flintstone starts at 8". Draw a dashed 6-row stencil that starts at the fred row of `jetsons[0]`:

```
fake flintstone at 8:
  wilma[0]         8    jetsons[0].fred
  wilma[1]        12    jetsons[0].dino.betty
  fred            16    jetsons[0].dino.barney
  dino.betty      20    jetsons[0].dino.bammbamm
  dino.barney     24    jetsons[1].wilma[0]
  dino.bammbamm   28    jetsons[1].wilma[1]     <- we write here
```

3. `->dino.bammbamm` → 8 + 12 (dino) + 8 (bammbamm) = **28**, which is `jetsons[1].wilma[1]`.
4. **Right:** `simpsons`, meaning its value, **12**.

- **Draw:** a dot in `jetsons[1].wilma[1]`, and arrow **C** to `jetsons[0].dino`. That's the same spot arrow A points to.

```
                 simpsons
            +---------------+
          B>|      o A      |
            +---------------+

                jetsons[0]         jetsons[1]         jetsons[2]         jetsons[3]
            +---------------+  +---------------+  +---------------+  +---------------+
bammbamm    |               |  |' ' 'B' 'u' 'n'|  |               |  |               |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
barney      |   |   |   |   |  |'B'|'u'|'g'|'s'|  |   |   |   |   |  |   |   |   |   |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
betty   A,C>|               |  |      o B      |  |               |  |               |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
fred        |       |       |  |       |       |  |       |       |  |       |       |
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+
wilma[1]    |               |  |      o C      |  |               |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
wilma[0]    |               |  |               |  |'n' 'y' \0     |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
```

**Ask:** "Step 3 had `&simpsons`, and this line has `simpsons`. How do they differ on the board?" → `&simpsons` is an arrow *to the simpsons box*. `simpsons` means copying simpsons's own arrow.

**Common mistake:** computing 8 + 8 = 16, which forgets that `dino` itself starts 12 bytes into the flintstone.

### Step 6 - `*(char **)jetson[4].fred = simpsons->barney + 4;`

- **Left:**
  - `jetsons[4]` → 4 × 24 = **96**. That's past the end, since the array covers 0..95.
  - `.fred` → +8 = **104**. It's a `short[2]`, so it becomes a `short *`.
  - `(char **)` means "there's a `char *` at 104". Then `*` writes **4 bytes** of address at 104..107.
- **Right:** `simpsons->barney` is `jetsons[0].dino.barney`, the address **16** (a `char *`). `+ 4` gives **20**, which is `jetsons[0].dino.bammbamm`.
- **Draw:** a dashed box to the right of `jetsons[3]`, at the height of the fred row (that box is `jetsons[4].fred`). Put a dot in it, and draw arrow **D** to the bottom-left corner of the top row of `jetsons[0]` (bammbamm).

**Ask:** "Why does `barney + 4` end up in a different field?" → barney is 4 bytes (16..19), so +4 is one past its end, which is the next field.
**Ask:** "Why the `(char **)` cast?" → Without it, `*jetsons[4].fred` is a `short`, only 2 bytes, and a 4-byte address doesn't fit. The cast says "treat these bytes as a slot for a pointer".

### Final picture (Flintstones)

```
                 simpsons
            +---------------+
          B>|      o A      |
            +---------------+

                jetsons[0]         jetsons[1]         jetsons[2]         jetsons[3]
            +---------------+  +---------------+  +---------------+  +---------------+
bammbamm  D>|               |  |' ' 'B' 'u' 'n'|  |               |  |               |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
barney      |   |   |   |   |  |'B'|'u'|'g'|'s'|  |   |   |   |   |  |   |   |   |   |
            +---+---+---+---+  +---+---+---+---+  +---+---+---+---+  +---+---+---+---+
betty   A,C>|               |  |      o B      |  |               |  |               |  jetsons[4].fred
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+  + - - - - - - +
fred        |       |       |  |       |       |  |       |       |  |       |       |  :     o D     :
            +-------+-------+  +-------+-------+  +-------+-------+  +-------+-------+  + - - - - - - +
wilma[1]    |               |  |      o C      |  |               |  |               |  byte 104, past
            +---------------+  +---------------+  +---------------+  +---------------+  the end
wilma[0]    |               |  |               |  |'n' 'y' \0     |  |               |
            +---------------+  +---------------+  +---------------+  +---------------+
```

| arrow | from (the dot) | to | line |
|---|---|---|---|
| A | `simpsons` | `jetsons[0].dino` (byte 12) | `simpsons = &jetsons[0].dino;` |
| B | `jetsons[1].dino.betty` (byte 36) | the `simpsons` box | `jetsons[1].wilma[3] = (short *) &simpsons;` |
| C | `jetsons[1].wilma[1]` (byte 28) | `jetsons[0].dino` (byte 12) | `((flintstone *)(jetsons->fred))->dino.bammbamm = simpsons;` |
| D | `jetsons[4].fred` (byte 104, past the end) | `jetsons[0].dino.bammbamm` (byte 20) | `*(char **)jetson[4].fred = simpsons->barney + 4;` |

This matches the official solution (Handout 10S). Its dashed magenta boxes are the flintstones and its dashed green boxes are the `dino`s. Arrows A and C both land on the bottom-left of `jetsons[0]`'s green box, and D comes from the dashed box on the right.

