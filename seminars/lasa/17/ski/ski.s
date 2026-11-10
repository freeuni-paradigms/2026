.text
# CALLS MAIN FUNCTION; DO NOT REMOVE
call main

########## YOUR CODE STARTS HERE ##########
#ski
#ra
off_piste:
addi sp, sp, -4
sw ra, 0(sp)

lw a0, 4(sp) # ski
lb a1, 14(a0) # ski->style

bne a1, x0, ELSE
# return -1;
li a0, -1
lw ra, 0(sp)
addi sp, sp, 4
ret

ELSE:
# return ski->boots[1] + *((char *)ski->bindings + 1);
# ski->boots[1]
lw a0, 4(sp) # ski
lw a1, 4(a0) # ski->boots[1]

lw a2, 8(a0) # ski->bindings
addi a2, a2, 1 # ((char *)ski->bindings + 1);
lb a3, 0(a2) # *((char *)ski->bindings + 1);

add a0, a1, a3
lw ra, 0(sp)
addi sp, sp, 4
ret

# trail
# people - short
# ski
# ra
# result
# i
start_skiing:
########## implement start_skiing here ##########
addi sp, sp, -4
sw ra, 0(sp)

# int result = 0;
addi sp, sp, -4
sw x0, 0(sp)

# int i = 0;
addi sp, sp, -4
sw x0, 0(sp)

LOOP:
# i < people
lw a0, 0(sp) # i
lh a1, 16(sp) # people

bge a0, a1, END
# result += skis[i].radius + trail(skis + i);
lw a1, 12(sp) # skis 
li a2, 15
mul a2, a2, a0 # i * sizeof(Ski)

add a1, a1, a2 # skis + i
lw a3, 18(sp) # trail

addi sp, sp, -4
sw a1, 0(sp)
jalr a3
addi sp, sp, 4

# skis[i].radius
lw a1, 0(sp) # i
lw a2, 12(sp) # skis
li a3, 15
mul a3, a3, a1 # i *sizeof(ski)
add a2, a2, a3 # &skis[i]
lh a3, 12(a2) # skis[i].radius

add a0, a0, a3 # skis[i].radius + trail(skis + i)
lw a1, 4(sp) # result 
add a1, a1, a0 # result + skis[i].radius + trail(skis + i)
sw a1, 4(sp)

#i++
lw a0, 0(sp)
addi a0, a0, 1
sw a0, 0(sp)

j LOOP

END:
# return result;
lh a0, 4(sp)
addi sp, sp, 8
lw ra, 0(sp)
addi sp, sp, 4
ret
########## YOUR CODE ENDS   HERE ##########

call_off_piste:
        addi    sp,sp,-32
        sw      ra,28(sp)
        sw      s0,24(sp)
        addi    s0,sp,32
        sw      a0,-20(s0)
        lw      a0,-20(s0)

        addi    sp,sp,-8
        sw      ra,4(sp)
        sw      a0,0(sp)
        call    off_piste
        lw      ra,4(sp)
        addi    sp,sp,8

        mv      a5,a0
        mv      a0,a5
        lw      ra,28(sp)
        lw      s0,24(sp)
        addi    sp,sp,32
        jr      ra
call_start_skiing:
        addi    sp,sp,-32
        sw      ra,28(sp)
        sw      s0,24(sp)
        addi    s0,sp,32
        sw      a0,-20(s0)
        sw      a1,-24(s0)
        sw      a2,-28(s0)
        lw      a5,-24(s0)
        slli    a5,a5,16
        srai    a5,a5,16

        addi    sp,sp,-14
        sw      ra,10(sp)
        sw      a0,0(sp)
        sh      a1,4(sp)
        sw      a2,6(sp)
        call    start_skiing
        lw      ra,10(sp)
        addi    sp,sp,14

        mv      a5,a0
        mv      a0,a5
        lw      ra,28(sp)
        lw      s0,24(sp)
        addi    sp,sp,32
        jr      ra
red_trail:
        lw      t0,0(sp)
        lw      t1,0(t0)
        lw      t2,8(t0)
        lb      t2,0(t2)
        li      t3,88
        li      t4,88
        li      t5,88
        li      t6,88
        li      a1,88
        li      a2,88
        li      a3,88
        li      a4,88
        li      a5,88
        li      a6,88
        li      a7,88
        add     a0,t1,t2
        ret
ecall:
        addi    sp,sp,-32
        sw      ra,28(sp)
        sw      s0,24(sp)
        addi    s0,sp,32
        sw      a0,-20(s0)
        sw      a1,-24(s0)
        lw      a5,-20(s0)
        mv      a0,a5
        lw      a5,-24(s0)
        mv      a1,a5
        ecall
        nop
        lw      ra,28(sp)
        lw      s0,24(sp)
        addi    sp,sp,32
        jr      ra
.data
bindings_0:
        .byte   100
        .byte   50
ski_0:
        .word   100
        .word   200
        .word   bindings_0
        .half   0
        .byte   0
bindings_1:
        .byte   10
        .byte   25
ski_1:
        .word   500
        .word   300
        .word   bindings_1
        .half   0
        .byte   1
bindings_2:
        .byte   5
        .byte   15
ski_2:
        .word   -100
        .word   150
        .word   bindings_2
        .half   0
        .byte   1
bindings_3:
        .byte   20
        .byte   30
ski_3:
        .word   10
        .word   20
        .word   bindings_3
        .half   5
        .byte   1
bindings_4:
        .byte   15
        .byte   25
ski_4:
        .word   30
        .word   40
        .word   bindings_4
        .half   10
        .byte   1
bindings_5:
        .byte   8
        .byte   12
ski_5:
        .word   50
        .word   60
        .word   bindings_5
        .half   15
        .byte   1
bindings_6:
        .byte   1
        .byte   2
ski_6:
        .word   100
        .word   200
        .word   bindings_6
        .half   50
        .byte   1
bindings_7:
        .byte   3
        .byte   4
ski_7:
        .word   300
        .word   400
        .word   bindings_7
        .half   100
        .byte   1
bindings_8:
        .byte   0
        .byte   0
ski_8:
        .word   30000
        .word   0
        .word   bindings_8
        .half   10000
        .byte   1
.LC0:
        .string "[OFF_PISTE TEST #1] result="
        .string ""
.LC1:
        .string "[OFF_PISTE TEST #2] result="
        .string ""
.LC2:
        .string "[OFF_PISTE TEST #3] result="
        .string ""
.LC3:
        .string "[START_SKIING TEST #1] result="
        .string ""
.LC4:
        .string "[START_SKIING TEST #2] result="
        .string ""
.LC5:
        .string "[START_SKIING TEST #3] result="
        .string ""
.LC6:
        .string "[START_SKIING TEST #4] result="
        .string ""
.text
main:   
        addi    sp,sp,-96
        sw      ra,92(sp)
        sw      s0,88(sp)
        addi    s0,sp,96
        la      a5,ski_3
        lw      a2,0(a5)
        lw      a3,4(a5)
        lw      a4,8(a5)
        lh      a6,12(a5)
        lb      a7,14(a5)
        sw      a2,-64(s0)
        sw      a3,-60(s0)
        sw      a4,-56(s0)
        sh      a6,-52(s0)
        sb      a7,-50(s0)
        la      a5,ski_4
        lw      a2,0(a5)
        lw      a3,4(a5)
        lw      a4,8(a5)
        lh      a6,12(a5)
        lb      a7,14(a5)
        sw      a2,-49(s0)
        sw      a3,-45(s0)
        sw      a4,-41(s0)
        sh      a6,-37(s0)
        sb      a7,-35(s0)
        la      a5,ski_5
        lw      a2,0(a5)
        lw      a3,4(a5)
        lw      a4,8(a5)
        lh      a6,12(a5)
        lb      a7,14(a5)
        sw      a2,-34(s0)
        sw      a3,-30(s0)
        sw      a4,-26(s0)
        sh      a6,-22(s0)
        sb      a7,-20(s0)
        la      a5,ski_6
        lw      a2,0(a5)
        lw      a3,4(a5)
        lw      a4,8(a5)
        lh      a6,12(a5)
        lb      a7,14(a5)
        sw      a2,-95(s0)
        sw      a3,-91(s0)
        sw      a4,-87(s0)
        sh      a6,-83(s0)
        sb      a7,-81(s0)
        la      a5,ski_7
        lw      a2,0(a5)
        lw      a3,4(a5)
        lw      a4,8(a5)
        lh      a6,12(a5)
        lb      a7,14(a5)
        sw      a2,-80(s0)
        sw      a3,-76(s0)
        sw      a4,-72(s0)
        sh      a6,-68(s0)
        sb      a7,-66(s0)
        #################### OFF_PISTE TEST 1
        la      a5,.LC0
        mv      a1,a5
        li      a0,4
        call    ecall
        la      a0,ski_0
        call    call_off_piste
        mv      a5,a0
        addi    a5,a5,1
        seqz    a5,a5
        andi    a5,a5,0xff
        mv      a1,a5
        li      a0,1
        call    ecall
        li      a1,10
        li      a0,11
        call    ecall
        #################### OFF_PISTE TEST 2
        la      a5,.LC1
        mv      a1,a5
        li      a0,4
        call    ecall
        la      a0,ski_1
        call    call_off_piste
        mv      a5,a0
        addi    a5,a5,-325
        seqz    a5,a5
        andi    a5,a5,0xff
        mv      a1,a5
        li      a0,1
        call    ecall
        li      a1,10
        li      a0,11
        call    ecall
        #################### OFF_PISTE TEST 3
        la      a5,.LC2
        mv      a1,a5
        li      a0,4
        call    ecall
        la      a0,ski_2
        call    call_off_piste
        mv      a5,a0
        addi    a5,a5,-165
        seqz    a5,a5
        andi    a5,a5,0xff
        mv      a1,a5
        li      a0,1
        call    ecall
        li      a1,10
        li      a0,11
        call    ecall
        ##################### START_SKIING TEST 1
        la      a5,.LC3
        mv      a1,a5
        li      a0,4
        call    ecall
        addi    a4,s0,-64
        la      a2,red_trail
        li      a1,1
        mv      a0,a4
        call    call_start_skiing
        mv      a5,a0
        addi    a5,a5,-35
        seqz    a5,a5
        andi    a5,a5,0xff
        mv      a1,a5
        li      a0,1
        call    ecall
        li      a1,10
        li      a0,11
        call    ecall
        ##################### START_SKIING TEST 2
        la      a5,.LC4
        mv      a1,a5
        li      a0,4
        call    ecall
        addi    a4,s0,-95
        la      a2,red_trail
        li      a1,2
        mv      a0,a4
        call    call_start_skiing
        mv      a5,a0
        addi    a5,a5,-554
        seqz    a5,a5
        andi    a5,a5,0xff
        mv      a1,a5
        li      a0,1
        call    ecall
        li      a1,10
        li      a0,11
        call    ecall
        ##################### START_SKIING TEST 3
        la      a5,.LC5
        mv      a1,a5
        li      a0,4
        call    ecall
        addi    a4,s0,-64
        la      a2,red_trail
        li      a1,3
        mv      a0,a4
        call    call_start_skiing
        mv      a5,a0
        addi    a5,a5,-163
        seqz    a5,a5
        andi    a5,a5,0xff
        mv      a1,a5
        li      a0,1
        call    ecall
        li      a1,10
        li      a0,11
        call    ecall
        ##################### START_SKIING TEST 4
        # 10000 + (30000 + 0) = 40000; (short)40000 = -25536.
        # Compare the full return register to check truncation and sign extension.
        la      a1,.LC6
        li      a0,4
        call    ecall
        la      a0,ski_8
        li      a1,1
        la      a2,red_trail
        call    call_start_skiing
        li      a5,-25536
        sub     a5,a0,a5
        seqz    a5,a5
        mv      a1,a5
        li      a0,1
        call    ecall
        li      a1,10
        li      a0,11
        call    ecall
        li      a5,0
        mv      a0,a5
        lw      ra,92(sp)
        lw      s0,88(sp)
        addi    sp,sp,96
        li      a0,10
        call    ecall
