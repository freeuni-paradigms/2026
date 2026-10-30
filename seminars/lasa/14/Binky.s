Caller:
addi sp, sp, -8

# Binky(actual);
addi sp, sp, -12
# write ra on stack
la a0, after_Binky
sw a0, 0(sp)

# write params on stack
lw a0, 12(sp) # actual.numerator
lw a1, 16(sp) # actual.denominator

sw a0, 4(sp) # param.numerator = actual.numerator
sw a1, 8(sp) # param.denominator = actual.denominator

call Binky

after_Binky:
addi sp, sp, 12
addi sp, sp, 8

lw ra, 0(sp)
ret


Binky:
addi sp, sp, -8

# local.denominator = 1;
li a0, 1
sw a0, 4(sp)
# param.denominator = 2;
li a0, 2
sw a0, 16(sp)

addi sp, sp, 8
lw ra, 0(sp)
ret