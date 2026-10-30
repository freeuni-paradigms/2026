# Seminar 14: a recursive function, fibonacci (fibonacci.c in RISC-V).
# Same rules as seminar 13: the caller pushes the parameters and its ra.
# Run: java -jar venus.jar fibonacci.s
# main is ready. fibonacci below it is empty: we write it in class.

.data
fib_msg:   .string "fibonacci(0..10) = "

.text
# main prints fibonacci(n) for n = 0, 1, ..., 10:
#     for (int n = 0; n <= 10; n++)
#         printf("%d ", fibonacci(n));
#
# n lives on the stack, not in a register: fibonacci overwrites the registers.
#
#   main's frame   offset
#   n                 0   <- sp
main:
    la   x11, fib_msg
    li   x10, 4            # ecall 4: print the string at x11
    ecall

    addi sp, sp, -4        # SP = SP - 4          space for n
    sw   x0, 0(sp)         # M[SP] = 0            n = 0
main_loop:
    lw   x10, 0(sp)        # R1 = M[SP]           x10 = n
    addi sp, sp, -4        # SP = SP - 4          fibonacci's parameter
    sw   x10, 0(sp)        # M[SP] = R1           i = n
    addi sp, sp, -4        # CALL <fibonacci>     push our ra (the Saved PC box)
    sw   ra, 0(sp)
    call fibonacci         #                      jump to fibonacci, ra = the next line
    lw   ra, 0(sp)         #                      back here: pop the saved ra
    addi sp, sp, 4
    addi sp, sp, 4         # SP = SP + 4          pop the parameter

    mv   x11, x10          # print fibonacci(n), then a space
    li   x10, 1
    ecall
    li   x11, ' '
    li   x10, 11
    ecall

    lw   x10, 0(sp)        # n++
    addi x10, x10, 1
    sw   x10, 0(sp)
    li   x11, 10
    ble  x10, x11, main_loop       # while n <= 10

    li   x11, '\n'
    li   x10, 11
    ecall
    addi sp, sp, 4         # SP = SP + 4          pop n
    li   x10, 10           # ecall 10: exit. Without it we'd fall into fibonacci below.
    ecall



# int fibonacci(int i){
#   if(i <= 1){
#     return i;
#   }
#
#   int fib1 = fibonacci(i - 1);
#   int fib2 = fibonacci(i - 2);
#   return fib1 + fib2;
# }
#
#  i        12
#  saved ra 8
#  fib1     4
#  fib2     0 <- sp

fibonacci:
    addi sp, sp, -8
    
    # if(i <= 1) return i;

    lw x10, 12(sp)  # x10 = i
    li x11, 1
    bgt x10, x11, fib_recurse # i > 1 jump over the return
    
    addi sp, sp, 8  #return i, i is already in x10
    ret

fib_recurse:
  # int fib1 = fibonacci(i - 1);
  lw x10, 12(sp) # x10 = i
  addi x10, x10, -1 # x10 = i - 1
  addi sp, sp, -4
  sw x10,  0(sp)
  addi sp, sp, -4
  sw, ra, 0(sp)
  call fibonacci
  lw ra, 0(sp)
  addi sp, sp, 4
  addi sp, sp, 4
  sw x10, 4(sp) # fib1 = what it returned

  #int fib2 = fibonacci(i-2)
  lw x10, 12(sp) # x10 = i
  addi x10, x10, -2 # x10 = i - 2
  addi sp, sp, -4
  sw x10,  0(sp)
  addi sp, sp, -4
  sw, ra, 0(sp)
  call fibonacci
  lw ra, 0(sp)
  addi sp, sp, 4
  addi sp, sp, 4
  sw x10, 0(sp) # fib2 = what it returned

  #return fib1 + fib2
  lw x10, 4(sp) #x10 = fib1
  lw x11, 0(sp) #x11 = fib2
  add x10, x10, x11 # x10 = fib1 + fib2
  addi sp, sp,  8 # pop fib1 and fib2
  ret



