Muppets:
addi sp, sp, -4

# ((struct fraction *)bert.next)->denominator = 0;
lw a0, 16(sp) # bert.next
sw x0, 4(a0)

# ernie = &bert;
add a0, sp, 8 # &bert
sw a0, 20(sp)

# oscar = &ernie;
add a0, sp, 20
sw a0, 0(sp)

# (**oscar).next = ernie;
lw a0, 0(sp)
lw a1, 0(a0)

lw a2, 20(sp) # ernie
sw a2, 8(a1)

# return bert.age;
lb a0, 8(sp)

addi sp, sp, 4
lw ra, 0(sp)
ret