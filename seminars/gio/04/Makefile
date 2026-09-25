CC = gcc
CFLAGS = -Wall -g -O0

all: strcpy strcpy_solution casting

strcpy: strcpy.c
	$(CC) $(CFLAGS) strcpy.c -o strcpy

strcpy_solution: strcpy_solution.c
	$(CC) $(CFLAGS) strcpy_solution.c -o strcpy_solution

casting: casting.c
	$(CC) $(CFLAGS) casting.c -o casting

# same code with optimizations - compare section 8 with ./casting 8
# (-fno-strict-aliasing keeps the *(int *)&f tricks of sections 10-11 working)
casting_O2: casting.c
	$(CC) -Wall -O2 -fno-strict-aliasing casting.c -o casting_O2

# the compiler knows about most of these traps - ask it
warnings:
	$(CC) -Wall -Wextra -Wconversion -fsyntax-only casting.c

clean:
	rm -f strcpy strcpy_solution casting casting_O2

.PHONY: all clean warnings
