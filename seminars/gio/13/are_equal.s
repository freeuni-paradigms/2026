# Handout 16, pages 9-10: calling through a function pointer (AreEqual),
# and main passing one the way the handout's Caller does.
# Run: java -jar venus.jar are_equal.s
# main is ready. The handout's function below it is empty: we write it in class.

.data
str_s:     .string "paradigms"
str_t:     .string "paradigms"     # the same letters, at a different address
same_msg:  .string "same = AreEqual(s, t, CompareStrings) = "

.text
# main does what the handout's Caller does:
#     int same;
#     char *s, *t;
#     same = AreEqual(s, t, CompareStrings);
#
#   main's frame   offset
#   same              8
#   s                 4
#   t                 0   <- sp
main:
    addi sp, sp, -12       # SP = SP - 12         space for same, s, t
    la   x10, str_s        # s and t point to two strings: the handout leaves them
    sw   x10, 4(sp)        # uninitialized, we give them values so there's something to see
    la   x10, str_t
    sw   x10, 0(sp)

    lw   x10, 0(sp)        # R1 = M[SP]           x10 = t
    lw   x11, 4(sp)        # R2 = M[SP + 4]       x11 = s
    la   x12, CompareStrings     # R3 = <CompareStrings>   the address of its first instruction
    addi sp, sp, -12       # SP = SP - 12         AreEqual's parameters
    sw   x12, 8(sp)        # M[SP + 8] = R3       cmp = CompareStrings
    sw   x10, 4(sp)        # M[SP + 4] = R1       b = t
    sw   x11, 0(sp)        # M[SP] = R2           a = s
    addi sp, sp, -4        # CALL <AreEqual>
    sw   ra, 0(sp)
    call AreEqual
    lw   ra, 0(sp)
    addi sp, sp, 4
    addi sp, sp, 12        # SP = SP + 12         pop AreEqual's parameters
    sw   x10, 8(sp)        # M[SP + 8] = RV       same = RV

    # print same
    la   x11, same_msg
    li   x10, 4
    ecall
    lw   x11, 8(sp)
    li   x10, 1
    ecall
    li   x11, '\n'
    li   x10, 11
    ecall

    addi sp, sp, 12        # SP = SP + 12         pop same, s, t
    li   x10, 10
    ecall


# typedef int (*compareFn)(const void *, const void *);
#
# static bool AreEqual(const void *a, const void *b, compareFn cmp)
# {
#     if (cmp(a, b) == 0)
#         return true;
#     else
#         return false;
# }
#
#   AreEqual's AR  offset
#   cmp              12
#   b                 8
#   a                 4
#   saved ra          0   <- sp
AreEqual:
  
  lw x10, 8(sp) #x10 = b
  lw x11, 4(sp) #x11 = a

  lw x12, 12(sp) #x12 = address of cmpFn

  addi sp, sp, -8
  sw x10, 4(sp)
  sw x11, 0(sp)

  addi sp, sp, -4
  sw ra, 0(sp)

  jalr x12
  
  lw ra, 0(sp)

  addi sp, sp, 4
  addi sp, sp, 8

  bne x10, x0, AreEqual_else

  li x10, 1 # return true
  ret

AreEqual_else:
  li x10, 0 # return false
  ret




# int CompareStrings(const void *a, const void *b)
# {
#     return strcmp((char *)a, (char *)b);
# }
#
# Venus has no strcmp, so CompareStrings does strcmp's loop itself.
# Not from the handout: it's given.
#
#   CompareStrings' AR  offset
#   b                     8
#   a                     4
#   saved ra              0   <- sp
CompareStrings:
    lw   x10, 4(sp)        # x10 = a
    lw   x11, 8(sp)        # x11 = b
cmp_loop:
    lbu  x12, 0(x10)       # x12 = *a
    lbu  x13, 0(x11)       # x13 = *b
    bne  x12, x13, cmp_done      # different chars: the answer is their difference
    beq  x12, x0, cmp_done       # both are '\0': equal, the difference is 0
    addi x10, x10, 1       # a++
    addi x11, x11, 1       # b++
    j    cmp_loop
cmp_done:
    sub  x10, x12, x13     # return *a - *b
    ret
