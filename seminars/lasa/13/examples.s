addi sp, sp, -8

# s = &i;
sw sp, 4(sp) # sp + 4 -> [sp]
# s++;
lw a0, 4(sp) # s
addi a0, a0, 4 # a0 = a0 + 4
sw a0, 4(sp)

addi sp, sp, 8


addi sp, sp, -4

# *(arr + 10) = 5;
lw a1, 0(sp) # start of arr

li a2, 5
sw a2, 40(a1)

addi sp, sp, 4

addi sp, sp, -8

# x->f = 6;
lw a0, 0(sp)
li a1, 6
sh a1, 8(a0)

# (**y).f = 7;
lw a0, 4(sp)
lw a1, 0(a0)

li a2, 7
sh a2, 8(a1)

addi sp, sp, 8

addi sp, sp, -4

lw a0, 0(sp)
li a1, 'a'

sb a1, 0(a0)

addi sp, sp, 4


addi sp, sp, -12

lw a0, 8(sp) # total
lw a1, 4(sp) # count

fcvt.s.w f0, a0 # (float)total
fcvt.s.w f1, a1 # (float)count

fdiv.s f0, f0, f1 # f0 = f0 / f1
fsw f0, 0(sp)

addi sp, sp, 12


addi sp, sp, -5

# s = (int *)&ch;
addi a0, sp, 4 # &ch -> sp + 4
sw a0, 0(sp)

# *s = (int)s;
lw a0, 0(sp)
sw a0, 0(a0)

addi sp, sp, 5


addi sp, sp, -4

lw a0, 0(sp)
li a1, 'A'
sb a1, 4(a0)

addi sp, sp, 4