Caller:
addi sp, sp, -12

# AreEqual(s, t, CompareStrings);
addi sp, sp, -16
# save ra on stack
la a0, after_AreEqual
sw a0, 0(sp)

# save params on stack
lw a0, 20(sp) # local s
sw a0, 4(sp)

lw a1, 16(sp) # local t
sw a1, 8(sp)

la a2, CompareStrings
sw a2, 12(sp)

call AreEqual

after_AreEqual:
addi sp, sp, 16
sw a0, 8(sp)

addi sp, sp, 12

lw ra, 0(sp)
ret

CompareStrings:
addi sp, sp, -12

# save ra on stack
la a0, after_strcmp
sw a0, 0(sp)

lw a0, 16(sp) # void* a
sw a0, 4(sp) # char * a = void * a

lw a1, 20(sp)
sw a1, 8(sp) # char * b = void * b

call strcmp

after_strcmp:
addi sp, sp, 12
lw ra, 0(sp)
ret


AreEqual:
addi sp, sp, -12
# save ra on stack
la a0, after_cmp
sw a0, 0(sp)
# save params on stack
lw a0, 16(sp) # local a
lw a1, 20(sp) # local b

sw a0, 4(sp) # param a = local a
sw a1, 8(sp) # param b = local b

lw a2, 24(sp) # cmp
jalr a2

after_cmp:
addi sp, sp, 12

bne a0, x0, false
li a0, 1
lw ra, 0(sp)
ret

false:
li a0, 0
lw ra, 0(sp)
ret
