.text

addi sp, sp, -4
li x10, 4
sw x10, 0(sp)
call fact
addi sp, sp, 4
li x10, 1
mv x11, x20
ecall
li x10, 10
ecall

fact:
    addi sp, sp, -4
    sw ra, 0(sp)
	lw x10, 4(sp)
	bne x10, x0, abovezero
	addi x20, x0, 1
    lw ra, 0(sp)
    addi sp, sp, 4
	ret
abovezero:
	lw x10, 4(sp)
	addi x10, x10, -1
	addi sp, sp, -4
	sw x10, 0(sp)
	call fact
    addi sp, sp, 4
	lw x10, 4(sp)
	mul x20, x20, x10
    lw ra, 0(sp)
    addi sp, sp, 4
	ret
