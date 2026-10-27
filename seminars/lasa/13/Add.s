Caller:

# int num = 10;
addi sp, sp, -4
li a0, 10
sw a0, 0(sp) # num = 10

addi sp, sp, -12
# write return address on stack
la a0, after_add
sw a0, 0(sp)

# num = Add(num, 45);
# write params on stack
lw a0, 12(sp) # num
sw a0, 4(sp)

li a0, 45
sw a0, 8(sp)

call Add

after_add:
addi sp, sp, 12
sw a0, 0(sp)

li a0, 100
sw a0, 0(sp)

addi sp, sp, 4

lw ra, 0(sp)
ret


Add:
addi sp, sp, -8

# sum = one + two;
lw a0, 12(sp) # one
lw a1, 16(sp) # two
add a0, a0, a1
sw a0, 0(sp)

lw a0, 0(sp)

addi sp, sp, 8
lw ra, 0(sp)
ret # pc = ra
