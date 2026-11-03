
# elastigirl.superboy
# elastigirl.dash[1]
# elastigirl.dash[0]
# elastigirl.violet
# frozone
# ra
theincredibles:
addi sp, sp, -4
sw ra, 0(sp)
# frozone += elastigirl.superboy[*frozone];

lw a0, 4(sp) # frozone
lh a1, 0(a0) # *frozone

addi a0, sp, 20 # base addr
add a0, a0, a1 # &elastigirl.superboy[*frozone]

lb a1, 0(a0) # elastigirl.superboy[*frozone]

li a0, 2
mul a1, a1, a0 # a1 *= 2

lw a0, 4(sp) # frozone
add a0, a0, a1
sw a0, 4(sp)