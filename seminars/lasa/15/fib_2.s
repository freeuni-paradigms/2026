main:
addi sp, sp, -4

li a0, 5
sw a0, 0(sp)

call fib

addi sp, sp, 4

mv a1, a0
li a0, 1
ecall

li a0, 10
ecall

fib:
addi sp, sp, -4
sw ra, 0(sp)

lw a0, 4(sp)
beq a0, x0, return_zero

li a1, 1
beq a0, a1, return_one

# fib(n-1)
addi sp, sp, -4

lw a0, 8(sp) # n
addi a0, a0, -1 # n - 1
sw a0, 0(sp)
call fib

addi sp, sp, 4

addi sp, sp, -4
sw a0, 0(sp) # save RV from fib(n-1)

# fib(n-2)
addi sp, sp, -4

lw a0, 12(sp) # n
addi a0, a0, -2 # n -2
sw a0, 0(sp)
call fib

addi sp, sp, 4

lw a1, 0(sp) # saved RV from fib(n-1)
addi sp, sp, 4
add a0, a0, a1 # fib(n-2) + fib(n-1)

lw ra, 0(sp)
addi sp, sp, 4
ret

return_zero:
li a0, 0
lw ra, 0(sp)
addi sp, sp, 4
ret

return_one:
li a0, 1
lw ra, 0(sp)
addi sp, sp, 4
ret
