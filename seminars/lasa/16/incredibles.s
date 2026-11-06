
# elastigirl.superboy - char[4]
# elastigirl.dash[1] - char *
# elastigirl.dash[0] - char*
# elastigirl.violet - int
# frozone - short *
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
li a2, 2
mul a1, a1, a2 # a1 *= 2

lw a0, 4(sp) # frozone
add a0, a0, a1
sw a0, 4(sp)

# ((superhero *)((superhero *) elastigirl.dash[0])->dash)->violet = 400;
lw a0, 12(sp) # elastigirl.dash[0]
addi a0, a0, 4 # ((superhero *) elastigirl.dash[0])->dash)

li a1, 400
sw a1, 0(a0)

# return *pixar(&elastigirl) + 10;
addi a0, sp, 8 # &elastigirl

addi sp, sp, -4

sw a0, 0(sp)
call pixar

addi sp, sp, 4

lw a1, 0(a0) # *pixar(&elastigirl)
addi a0, a1, 160 # *pixar(&elastigirl) + 160

lw ra, 0(sp)
addi sp, sp, 4
ret