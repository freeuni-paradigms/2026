
# gooddeeds - other *
# conformity - short *
# ra
BeginCalm:
addi sp, sp, -4
sw ra, 0(sp)

# conformity[*conformity] = 0;
lw a0, 4(sp) # conformity
lh a1, 0(a0) # *conformity

li a2, 2
mul a1, a1, a2

add a0, a0, a1 # & conformity[*conformity]
sh x0, 0(a0)

# gooddeeds += ((struct human *) (gooddeeds->henrietta[0].emmyjo))->doug;
lw a0, 8(sp) # gooddeeds
lw a1, 12(a0) # gooddeeds->henrietta

addi a1, a1, 4 # gooddeeds->henrietta[0].emmyjo

lw a2, 0(a1) # ((struct human *) (gooddeeds->henrietta[0].emmyjo))->doug;
li a1, 16
mul a2, a2, a1

lw a0, 8(sp) # gooddeeds
add a0, a0, a2
sw a0, 8(sp)

# return AskingQuestions((struct human *) &gooddeeds);
addi a0, sp, 8 # &gooddeeds

addi sp, sp, -4

sw a0, 0(sp)
call AskingQuestions

addi sp, sp, 4

lw ra, 0(sp)
addi sp, sp, 4
ret


# heroes
# ra
# a
# b
# c
AskingQuestions:
addi sp, sp, -4
sw ra, 0(sp)

addi sp, sp, -12

# b = (short)c;
lh a0, 0(sp)
sw a0, 4(sp)

lw a0, 16(sp) # heroes
lw a1, 0(a0) # heroes->doug

bne a1, x0, end
# a += 8;
lw a0, 8(sp)
addi a0, a0, 8
sw a0, 8(sp)

end:
# return &heroes;
addi a0, sp, 16 # &heroes

addi sp, sp, 12

lw ra, 0(sp)
addi sp, sp, 4
ret