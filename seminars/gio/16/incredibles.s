# Handout 20 (section handout), problem 1: The Incredibles. The official answer is in 20S.
# Run: java -jar venus.jar incredibles.s
# main and pixar are ready. theincredibles is empty: we write it in class.

.data
jackjack:  .word 1, 0, 0, 0          # superhero jackjack = {1, {NULL, NULL}, {0, 0, 0, 0}}
team:      .space 176                # superhero team[11]
team_ptr:  .word 0                   # superhero *team_ptr: pixar points it at team[0]
ice:       .half 2, 5, 0, 0          # short ice[4]: frozone points at ice[0], so *frozone = 2
pixar_msg: .string "pixar got: syndrome->violet = "
froz_msg:  .string ", frozone = ice + "
jj_msg:    .string "jackjack.dash[0] = "
jjv_msg:   .string ", jackjack.violet = "
ret_msg:   .string "theincredibles returned team + "
bytes_msg: .string " bytes\n"

.text
# main calls theincredibles(&ice[0], elastigirl) with
#     elastigirl = {7, {(char *)&jackjack, NULL}, {1, 2, 3, 4}}
# and jackjack.dash[0] = (char *)&jackjack. Then it prints what changed.
main:
    la   x10, jackjack     # jackjack.dash[0] = (char *)&jackjack
    sw   x10, 4(x10)

    addi sp, sp, -20       # theincredibles' parameters: frozone (4 bytes) and elastigirl (16)
    li   x10, 0x04030201   # elastigirl.superboy = {1, 2, 3, 4}: the lowest byte is superboy[0]
    sw   x10, 16(sp)
    sw   x0, 12(sp)        # elastigirl.dash[1] = NULL
    la   x10, jackjack
    sw   x10, 8(sp)        # elastigirl.dash[0] = (char *)&jackjack
    li   x10, 7
    sw   x10, 4(sp)        # elastigirl.violet = 7
    la   x10, ice
    sw   x10, 0(sp)        # frozone = &ice[0]
    addi sp, sp, -4        # CALL <theincredibles>   push our ra (the Saved PC box)
    sw   ra, 0(sp)
    call theincredibles
    lw   ra, 0(sp)
    addi sp, sp, 4
    addi sp, sp, 20        # pop theincredibles' parameters

    # print jackjack and what theincredibles returned
    mv   x12, x10          # keep the result: printing needs x10
    la   x11, jj_msg
    li   x10, 4
    ecall
    la   x13, jackjack
    lw   x11, 4(x13)       # jackjack.dash[0]
    li   x10, 1
    ecall
    la   x11, jjv_msg
    li   x10, 4
    ecall
    lw   x11, 0(x13)       # jackjack.violet
    li   x10, 1
    ecall
    li   x11, '\n'
    li   x10, 11
    ecall
    la   x11, ret_msg
    li   x10, 4
    ecall
    la   x13, team
    sub  x11, x12, x13     # returned - team, in bytes
    li   x10, 1
    ecall
    la   x11, bytes_msg
    li   x10, 4
    ecall

    li   x10, 10
    ecall


# typedef struct {
#     int violet;
#     char *dash[2];
#     char superboy[4];
# } superhero;
#
# static superhero **pixar(superhero *syndrome);
# static superhero *theincredibles(short *frozone, superhero elastigirl)
# {
#     frozone += elastigirl.superboy[*frozone];
#     ((superhero *)((superhero *) elastigirl.dash[0])->dash)->violet = 400;
#     return *pixar(&elastigirl) + 10;
# }
#
#   theincredibles' AR       offset
#   elastigirl.superboy[]      20   (4 chars: 20, 21, 22, 23)
#   elastigirl.dash[1]         16
#   elastigirl.dash[0]         12
#   elastigirl.violet           8
#   frozone                     4
#   saved ra                    0   <- sp
#
# R1-R7 = x10-x16.
theincredibles:
  
#  frozone += elastigirl.superboy[*frozone];
  lw x10, 4(sp)  # x10 = frozone
  lh x11, 0(x10) # x11 = *frozone
  addi x12, sp, 20 # &elastigirl.superboy[0]
  add x13, x12, x11 #&elastigirl.superboy[*frozone]
  lb x14, 0(x13) # elastigirl.superboy[*frozone]
  slli x15, x14, 1 # scale by size of short
  add x16, x10, x15 # the new frozone
  sw x16, 4(sp) # store it back to frozone


#  ((superhero *)((superhero *) elastigirl.dash[0])->dash)->violet = 400;
  lw x10, 12(sp) # x10 = elastigirl.dash[0]
  li x11, 400 # x11 = 400
  sw x11, 4(x10) #-> dash is 4 bytes in, -> violet is 0 bytes into the struct


#  return *pixar(&elastigirl) + 10;
  addi x10, sp, 8 # x10 = &elastigirl
  addi sp, sp, -4
  sw x10, 0(sp)
  addi sp, sp, -4
  sw ra, 0(sp)
  call pixar
  lw ra, 0(sp)
  addi sp, sp, 4
  addi sp, sp, 4
  lw x11, 0(x10) # x11 = *pixar(...)
  addi x10, x11, 160 # + 10 superheroes = 10 * 16 bytes
  ret

# static superhero **pixar(superhero *syndrome)
#
# Not from the handout: it only gives the prototype. Ours prints what it got,
# then returns &team_ptr, with team_ptr = &team[0].
# To show line 1's result it cheats: theincredibles' frozone sits right below
# elastigirl, so it's at syndrome - 4 bytes.
#
#   pixar's AR     offset
#   syndrome          4
#   saved ra          0   <- sp
pixar:
    la   x11, pixar_msg
    li   x10, 4
    ecall
    lw   x12, 4(sp)        # x12 = syndrome
    lw   x11, 0(x12)       # syndrome->violet
    li   x10, 1
    ecall
    la   x11, froz_msg
    li   x10, 4
    ecall
    lw   x11, -4(x12)      # theincredibles' frozone
    la   x13, ice
    sub  x11, x11, x13     # frozone - ice, in bytes
    li   x10, 1
    ecall
    la   x11, bytes_msg
    li   x10, 4
    ecall

    la   x10, team_ptr     # team_ptr = &team[0]
    la   x11, team
    sw   x11, 0(x10)
    ret                    # return &team_ptr
