main:
addi sp, sp, -8
# save ra
la a0, after_fib
sw a0, 0(sp)
# save param
li a0, 7
sw a0, 4(sp)

call fib

after_fib:
addi sp, sp, 8

mv a1, a0
li a0, 1
ecall

li a0, 10
ecall


# n
# ra
fib:
lw a0, 4(sp) # n

beq a0, x0, return_zero

li a1, 1
beq a0, a1, return_one

# fib(n-1)
addi sp, sp, -8
# save ra on stack
la a0, after_fib1
sw a0, 0(sp)
# save n-1
lw a0, 12(sp) # n
addi a0, a0, -1
sw a0, 4(sp)

call fib

after_fib1:
addi sp, sp, 8

addi sp, sp, -4
sw a0, 0(sp) # save RV from fib(n-1) on stack

# fib(n-2)
addi sp, sp, -8
# save ra
la a0, after_fib2
sw a0, 0(sp)
# save n-2 on stack
lw a0, 16(sp) # n
addi a0, a0, -2
sw a0, 4(sp)

call fib

after_fib2:
addi sp, sp, 8

lw a1, 0(sp) # saved RV from fib(n-1)
addi sp, sp, 4
add a0, a0, a1 # fib(n-2) + fib(n-1)

lw ra, 0(sp)
ret

return_zero:
li a0, 0
lw ra, 0(sp)
ret

return_one:
li a0, 1
lw ra, 0(sp)
ret
