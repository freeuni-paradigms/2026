# Handout 16, pages 4-5: a struct parameter (Binky), and main passing one
# the way the handout's Caller does.
# Run: java -jar venus.jar binky.s
# main is ready. The handout's function below it is empty: we write it in class.

.data
copy_msg:   .string "Binky's copy: param.denominator = "
actual_msg: .string "main's actual.denominator = "

.text
# main does what the handout's Caller does:
#     struct fraction actual;
#     Binky(actual);
#
#   main's frame   offset
#   actual.den        4
#   actual.num        0   <- sp
main:
    addi sp, sp, -8        # SP = SP - 8          space for actual
    li   x10, 3            # actual = {3, 4}: the handout leaves it uninitialized,
    sw   x10, 0(sp)        # we give it values so there's something to see
    li   x10, 4
    sw   x10, 4(sp)

    lw   x10, 0(sp)        # R1 = M[SP]           x10 = actual.numerator
    lw   x11, 4(sp)        # R2 = M[SP + 4]       x11 = actual.denominator
    addi sp, sp, -8        # SP = SP - 8          Binky's parameter: a copy of actual
    sw   x11, 4(sp)        # M[SP + 4] = R2
    sw   x10, 0(sp)        # M[SP] = R1

    addi sp, sp, -4        # CALL <Binky>
    sw   ra, 0(sp)
    call Binky
    lw   ra, 0(sp)
    addi sp, sp, 4

    # print the copy's denominator, before we pop it
    la   x11, copy_msg
    li   x10, 4
    ecall
    lw   x11, 4(sp)
    li   x10, 1
    ecall
    li   x11, '\n'
    li   x10, 11
    ecall

    addi sp, sp, 8         # SP = SP + 8          pop Binky's parameter

    # print actual.denominator
    la   x11, actual_msg
    li   x10, 4
    ecall
    lw   x11, 4(sp)
    li   x10, 1
    ecall
    li   x11, '\n'
    li   x10, 11
    ecall

    addi sp, sp, 8         # SP = SP + 8          pop actual
    li   x10, 10
    ecall


# struct fraction {
#     int numerator;
#     int denominator;
# };
#
# static void Binky(struct fraction param)
# {
#     struct fraction local;
#     local.denominator = 1;
#     param.denominator = 2;
# }
#
#   Binky's AR     offset
#   param.den        16
#   param.num        12
#   saved ra          8
#   local.den         4
#   local.num         0   <- sp
Binky:

  addi sp, sp, -8

  li x10, 1

  sw x10, 4(sp)

  li x10, 2

  sw x10 16(sp)
  
  addi sp, sp, 8

  ret



