/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif

/*
 *
 *  ! ~  && & || | ^  << >>
 *
 *  !x = 0
 *  !0 = 1
 *  !ნებისმიერი რიცხვი = 0
 *
 *
 *  not and bitwise not => ! ~ 
 *
 * int -> 4 byte = 4 * 8 = 32 bit
 *
 * a = 00000000 00000000 00000000 00000100 = 4
 *
 * ~a = 11111111 11111111 11111111 11111011
 *
 *  
 *  and and bitwise and (x, y)
 *
 *  x = 00000000 00000000 00000000 00001100 = 12
 *  y = 00000000 00000000 00000000 00000110 = 6
 *
 *  a = x & y 
 *
 *  a = 00000000 00000000 00000000 00000100 = 4
 *
 *
 *  ^ bitwise xor (x, y)
 *
 *
 *  x = 00000000 00000000 00000000 00001100 = 12
 *  y = 00000000 00000000 00000000 00000110 = 6
 *
 *  a = x ^ y
 *
 *  a = 00000000 00000000 00000000 00001010 = 10
 *   
 *  or and bitwise or (x, y)
 *
 *
 *  x = 00000000 00000000 00000000 00001100 = 12
 *  y = 00000000 00000000 00000000 00000110 = 6
 *
 *  a = x | y
 *
 *  a = 00000000 00000000 00000000 00001110
 *
 *
 *
 *
 * left shift <<
 *
 *
 *  x = 00000000 00000000 00000000 00001100 = 12
 *  y = 2
 *
 *  a = x << y
 *
 *  a = 00000000 00000000 00000000 00110000 = 48
 *
 *
 *  right shift >> 
 *
 *
 *  x = 00000000 00000000 00000000 00001100 = 12
 *  y = 2
 *
 *  a = x >> y
 *
 *  a = 00000000 00000000 00000000 00000011 = 3
 *
 */




//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 *
 *
 * x ^ y = (NOT both 1) and (NOT both 0)
 *          ~(x & y)   &    ~(~x & ~y)
 *
 *
 */
int bitXor(int x, int y) {
  return ~(x & y) & ~(~x & ~y);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {

  return 1 << 31;

}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 *
 *   x + 1 = ~x if x is Tmax
 *
 *   this equation is true in case of -1 aswell
 *
 */
int isTmax(int x) {
  int next = x + 1;
  return !(next ^ ~x) & !!next;
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int even = 0x55 | (0x55 << 8); // 0101 ..
  even  = even | (even << 16);
  return !~(x | even);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int minus30 = ~0x30 + 1;
  int check1 = x + minus30;       // x - 0x30, must be >= 0
  int minusX = ~x + 1;
  int check2 = 0x39 + minusX;     // 0x39 - x, must be >= 0
  return !(check1 >> 31) & !(check2 >> 31);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int mask = ~!!x + 1;  // x != 0 -> 1 -> all ones;  x == 0 -> 0 -> all zeros
  return (mask & y) | (~mask & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  // different signs: the answer is "is x negative" (y - x could overflow)
  // same signs: y - x cannot overflow, so just look at its sign bit
  int signX = (x >> 31) & 1;
  int signY = (y >> 31) & 1;
  int diffSign = signX ^ signY;
  int signDiff = ((y + ~x + 1) >> 31) & 1;   // sign of y - x
  return (diffSign & signX) | (!diffSign & !signDiff);
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  // x != 0 -> x or -x is negative -> sign bit of (x | -x) is 1 -> >> 31 gives -1 -> + 1 = 0
  // x == 0 -> everything stays 0 -> + 1 = 1
  return ((x | (~x + 1)) >> 31) + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  // negative x needs as many bits as ~x, then binary-search the highest 1 bit, + 1 for the sign
  int b16, b8, b4, b2, b1, b0;
  x = x ^ (x >> 31);       // x >= 0 -> x;  x < 0 -> ~x
  b16 = !!(x >> 16) << 4;  // any 1 above bit 15? then count 16 and shift them away
  x = x >> b16;
  b8 = !!(x >> 8) << 3;
  x = x >> b8;
  b4 = !!(x >> 4) << 2;
  x = x >> b4;
  b2 = !!(x >> 2) << 1;
  x = x >> b2;
  b1 = !!(x >> 1);
  x = x >> b1;
  b0 = x;
  return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned sign = uf & 0x80000000;
  unsigned exp = (uf >> 23) & 0xFF;
  unsigned frac = uf & 0x7FFFFF;
  if (exp == 0xFF)          // NaN or infinity: 2 * inf = inf, NaN stays NaN
    return uf;
  if (exp == 0)             // denormalized: shift frac; a carry into exp normalizes it, which is correct
    return sign | (frac << 1);
  exp = exp + 1;            // normalized: 2 * (1.frac * 2^E) = 1.frac * 2^(E+1)
  if (exp == 0xFF)          // overflow -> infinity (frac must be 0)
    return sign | 0x7F800000;
  return sign | (exp << 23) | frac;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  int sign = uf >> 31;
  int exp = (uf >> 23) & 0xFF;
  int frac = uf & 0x7FFFFF;
  int E = exp - 127;
  int M;
  if (E < 0)                // |f| < 1 (includes 0 and denormalized) -> 0
    return 0;
  if (E >= 31)              // |f| >= 2^31, infinity, NaN (also exactly right for -2^31)
    return 0x80000000u;
  M = frac | 0x800000;      // put the implicit leading 1 back: M = 1.frac * 2^23
  if (E > 23)               // value = M * 2^(E - 23)
    M = M << (E - 23);
  else
    M = M >> (23 - E);      // dropping the fraction bits = truncation toward 0
  if (sign)
    return -M;
  return M;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  if (x > 127)              // too large -> +infinity
    return 0x7F800000;
  if (x >= -126)            // normalized: M = 1.0 so frac = 0, exp = x + 127
    return (x + 127) << 23;
  if (x >= -149)            // denormalized: value = frac * 2^-149, so frac = 2^(x + 149)
    return 1 << (x + 149);
  return 0;                 // too small -> 0
}
