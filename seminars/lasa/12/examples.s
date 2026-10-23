.data
pi: .float 3.14159

.text
# int i;
addi sp, sp, -4
# i = 6;
li a0, 6
sw a0, 0(sp)

# i++;
lw a0, 0(sp)
addi a0, a0, 1
sw a0, 0(sp)

addi sp, sp, 4


addi sp, sp, -1
# ch = 'A';
li a0, 'A'
sb a0, 0(sp)

addi sp, sp, 1


addi sp, sp, -5
# ch = 'A';
li a0, 'A'
sb a0, 4(sp)

# i = ch;
lb a0, 4(sp)
sw a0, 0(sp)

addi sp, sp, 5

addi sp, sp, -5
# i = 1025;
li a0, 1025
sw a0, 0(sp)
# ch = i;
lw a0, 0(sp)
sb a0, 4(sp)

addi sp, sp, 5

addi sp, sp, -8
# f = 3.14159
la a0, pi
flw f0, 0(a0)
fsw f0, 0(sp)

# i = f;
flw f0, 0(sp)
fcvt.w.s a0, f0
sw a0, 4(sp)

addi sp, sp, 8


addi sp, sp, -4
lw a0, 0(sp)

blt a0, x0, end
# i *= 2;
lw a0, 0(sp)
li a1, 2
mul a0, a1, a0
sw a0, 0(sp)

end:
li a0, 10
sw a0, 0(sp)
addi sp, sp, 4


addi sp, sp, -5

# for (i = 0; i < 10; i++)
sw x0, 0(sp)

loop:
lw a0, 0(sp) # i
li a1, 10

bge a0, a1, finish
# ch = 'a';
li a1, 'a'
sb a1, 4(sp)
# i++
lw a0, 0(sp) # i
addi a0, a0, 1
sw a0, 0(sp)
j loop

finish:
# i = 25;
li a1, 25
sw a1, 0(sp)
addi sp, sp, 5

addi sp, sp, -10
# x.e = 'A';
li a0, 'A'
sb a0, 7(sp)

# x.f = x.a;
lw a0, 0(sp)
sh a0, 8(sp)

addi sp, sp, 10

















