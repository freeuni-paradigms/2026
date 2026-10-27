# Handout 16, pages 6-8: a local array (Apple) and an array parameter (Banana).
# Run: java -jar venus.jar arrays.s
# main is ready. The handout's function below it is empty: we write it in class.

.data
scores_msg: .string "scores = "

.text
# main has Apple's locals and does what Apple does on pages 6 and 8:
#     int i = 1;
#     short scores[4] = {0, 0, 0, 0};
#     scores[i] = 10;          // page 6
#     Banana(scores, 2);       // page 8 passes i, we pass 2 so it writes somewhere else
#
#   main's frame   offset
#   i                 8
#   scores[3]         6
#   scores[2]         4
#   scores[1]         2
#   scores[0]         0   <- sp
main:
    addi sp, sp, -12       # SP = SP - 12         space for i and scores
    li   x10, 1            # i = 1 (the handout leaves i and scores uninitialized)
    sw   x10, 8(sp)
    sw   x0, 0(sp)         # scores[0] = scores[1] = 0: two shorts in one word
    sw   x0, 4(sp)         # scores[2] = scores[3] = 0

    # scores[i] = 10;
    lw   x10, 8(sp)        # R1 = M[SP + 8]       x10 = i
    slli x11, x10, 1       # R2 = R1 * 2          i * sizeof(short)
    add  x12, sp, x11      # R3 = SP + R2         &scores[i]: the array starts at sp
    li   x13, 10           # M[R3] =.2 10         scores[i] = 10, only 2 bytes
    sh   x13, 0(x12)

    # Banana(scores, 2);
    li   x10, 2            # R1 = 2               (page 8: R1 = M[SP + 8], that's i)
    mv   x11, sp           # R2 = SP              x11 = scores: the address of the array
    addi sp, sp, -8        # SP = SP - 8          Banana's parameters
    sw   x10, 4(sp)        # M[SP + 4] = R1       n = 2
    sw   x11, 0(sp)        # M[SP] = R2           scores = &scores[0]
    addi sp, sp, -4        # CALL <Banana>
    sw   ra, 0(sp)
    call Banana
    lw   ra, 0(sp)
    addi sp, sp, 4
    addi sp, sp, 8         # SP = SP + 8          pop Banana's parameters

    # print the four shorts
    la   x11, scores_msg
    li   x10, 4
    ecall
    li   x12, 0            # k = 0
print_loop:
    slli x13, x12, 1       # k * 2
    add  x13, sp, x13      # x13 = &scores[k]
    lh   x11, 0(x13)
    li   x10, 1
    ecall
    li   x11, ' '
    li   x10, 11
    ecall
    addi x12, x12, 1       # k++
    li   x13, 4
    blt  x12, x13, print_loop    # while k < 4
    li   x11, '\n'
    li   x10, 11
    ecall

    addi sp, sp, 12        # SP = SP + 12         pop i and scores
    li   x10, 10
    ecall


# static void Banana(short scores[], int n)
# {
#     scores[n] = 10;
# }
#
#   Banana's AR    offset
#   n                 8
#   scores            4    (a short *, not the array)
#   saved ra          0   <- sp
Banana:
  
  lw x10, 8(sp)

  slli x11, x10, 1 # x11 = x10 * 2

  lw x12, 4(sp)

  add x13, x12, x11 # &scores[n]

  li x14, 10

  sh x14, 0(x13)

  ret

