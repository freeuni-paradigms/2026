# Handout 16, pages 1-2: Add, and main calling it the way the handout's Caller does.
# Run: java -jar venus.jar add.s
# main is ready. The handout's function below it is empty: we write it in class.

.data
num_msg:   .string "Add(10, 45) = "

.text
# main does what the handout's Caller does:
#     int num = 10;
#     num = Add(num, 45);
#
#   main's frame   offset
#   num               0   <- sp
main:
    addi sp, sp, -4        # SP = SP - 4          space for num
    li   x10, 10           # M[SP] = 10           num = 10
    sw   x10, 0(sp)

    lw   x10, 0(sp)        # R1 = M[SP]           x10 = num, before sp moves
    addi sp, sp, -8        # SP = SP - 8          Add's parameters
    li   x11, 45           # M[SP + 4] = 45       two = 45
    sw   x11, 4(sp)
    sw   x10, 0(sp)        # M[SP] = R1           one = num

    addi sp, sp, -4        # CALL <Add>           push our ra (the Saved PC box)
    sw   ra, 0(sp)
    call Add               #                      jump to Add, ra = the next line
    lw   ra, 0(sp)         #                      back here: pop the saved ra
    addi sp, sp, 4

    addi sp, sp, 8         # SP = SP + 8          pop Add's parameters
    sw   x10, 0(sp)        # M[SP] = RV           num = what Add returned

    # print num
    la   x11, num_msg
    li   x10, 4            # ecall 4: print the string at x11
    ecall
    lw   x11, 0(sp)
    li   x10, 1            # ecall 1: print the int in x11
    ecall
    li   x11, '\n'
    li   x10, 11           # ecall 11: print the char in x11
    ecall

    addi sp, sp, 4         # SP = SP + 4          pop num
    li   x10, 10           # ecall 10: exit. Without it we'd fall into Add below.
    ecall


# static int Add(int one, int two)
# {
#     int temp, sum;
#     sum = one + two;
#     return sum;
# }
#
#   Add's AR       offset
#   two              16
#   one              12
#   saved ra          8
#   temp              4
#   sum               0   <- sp
Add:
  addi sp, sp, -8
  
  lw x10, 12(sp)
  lw, x11, 16(sp)

  add x12, x10, x11

  sw x12, 0(sp)

  lw x10, 0(sp)

  addi sp, sp, 8

  ret

