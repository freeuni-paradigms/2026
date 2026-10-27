# Handout 16, pages 11-12: pointers and typecasts (Muppets).
# Run: java -jar venus.jar muppets.s
# main is ready. The handout's function below it is empty: we write it in class.

.data
ret_msg:  .string "Muppets returned bert.age as a char: 300 -> "
den_msg:  .string "f.denominator after the call = "

.text
# main makes a fraction f = {1, 7} and bert = {300, 5, (struct person *)&f},
# then calls Muppets(bert, NULL).
#
#   main's frame   offset
#   f.denominator     4
#   f.numerator       0   <- sp
main:
    addi sp, sp, -8        # struct fraction f
    li   x10, 1
    sw   x10, 0(sp)        # f.numerator = 1
    li   x10, 7
    sw   x10, 4(sp)        # f.denominator = 7
    mv   x11, sp           # x11 = &f

    addi sp, sp, -16       # Muppets' parameters: bert (12 bytes) and ernie (4)
    sw   x0, 12(sp)        # ernie = NULL (Muppets overwrites it anyway)
    sw   x11, 8(sp)        # bert.next = &f, a fraction pretending to be a person
    li   x10, 5
    sw   x10, 4(sp)        # bert.id = 5
    li   x10, 300
    sw   x10, 0(sp)        # bert.age = 300
    addi sp, sp, -4        # CALL <Muppets>
    sw   ra, 0(sp)
    call Muppets
    lw   ra, 0(sp)
    addi sp, sp, 4
    addi sp, sp, 16        # pop Muppets' parameters

    # print what Muppets returned, and f.denominator
    mv   x12, x10          # keep the result: printing needs x10
    la   x11, ret_msg
    li   x10, 4
    ecall
    mv   x11, x12
    li   x10, 1
    ecall
    li   x11, '\n'
    li   x10, 11
    ecall
    la   x11, den_msg
    li   x10, 4
    ecall
    lw   x11, 4(sp)
    li   x10, 1
    ecall
    li   x11, '\n'
    li   x10, 11
    ecall

    addi sp, sp, 8         # pop f
    li   x10, 10
    ecall


# struct person {
#     int age, id;
#     struct person *next;
# };
#
# static char Muppets(struct person bert, struct person *ernie)
# {
#     struct person **oscar;
#
#     ((struct fraction *)bert.next)->denominator = 0;
#     ernie = &bert;
#     oscar = &ernie;
#     (**oscar).next = ernie;
#     return bert.age;
# }
#
#   Muppets' AR    offset
#   ernie            20
#   bert.next        16
#   bert.id          12
#   bert.age          8
#   saved ra          4
#   oscar             0   <- sp
Muppets:
    addi sp, sp, -4        # SP = SP - 4          space for oscar

    # ((struct fraction *)bert.next)->denominator = 0;
    lw   x10, 16(sp)       # R1 = M[SP + 16]      x10 = bert.next
    sw   x0, 4(x10)        # M[R1 + 4] = 0        denominator is 4 bytes into a fraction

    # ernie = &bert;  oscar = &ernie;
    addi x10, sp, 20       # R1 = SP + 20         x10 = &ernie
    addi x11, sp, 8        # R2 = SP + 8          x11 = &bert
    sw   x11, 20(sp)       # M[SP + 20] = R2      ernie = &bert
    sw   x10, 0(sp)        # M[SP] = R1           oscar = &ernie

    # (**oscar).next = ernie;
    lw   x10, 0(sp)        # R1 = M[SP]           x10 = oscar        (struct person **)
    lw   x10, 0(x10)       # R1 = M[R1]           x10 = *oscar       (struct person *)
    lw   x11, 20(sp)       # R2 = M[SP + 20]      x11 = ernie
    sw   x11, 8(x10)       # M[R1 + 8] = R2       (**oscar).next = ernie

    # return bert.age;
    lb   x10, 8(sp)        # R3 = M[SP + 8]; RV =.1 R3    load only the lowest byte

    addi sp, sp, 4         # SP = SP + 4          pop oscar
    ret                    # RET
