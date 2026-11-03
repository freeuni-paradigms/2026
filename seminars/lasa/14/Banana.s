Apple:
addi sp, sp, -12

# Banana(scores, i);
addi sp, sp, -12
# save ra on stack
la a0, after_Banana
sw a0, 0(sp)

# save params on stack
add a0, sp, 12 # a0 = sp  + 12
sw a0, 4(sp)

lw a0, 20(sp) # i
sw a0, 8(sp)

call Banana

after_Banana:
addi sp, sp, 12
addi sp, sp, 12

lw ra, 0(sp)
ret

Banana:
lw a0, 4(sp) # base addr of scores
lw a1, 8(sp) # n

li a2, 2
mul a1, a1, a2 # a1 = n * 2, offset
add a0, a0, a1 # base + offset

li a2, 10
sh a2, 0(a0) # scores[n] = 10

lw ra, 0(sp)
ret