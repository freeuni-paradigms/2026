CC = gcc
CFLAGS = -Wall -g -O0

all: sortedset sortedset_solution

sortedset: sortedset.c
	$(CC) $(CFLAGS) sortedset.c -o sortedset

sortedset_solution: sortedset_solution.c
	$(CC) $(CFLAGS) sortedset_solution.c -o sortedset_solution

# AddressSanitizer: finds a write into the old block after realloc moved it
# (heap-use-after-free), a node written past the end of the block, leaks.
# -fno-sanitize=alignment: the handout packs everything with no gaps, so on
# 64-bit node 0's element starts at byte 4 (a double or a char * wants 8).
# x86 doesn't mind.
# Linux / WSL / macOS only (not MinGW). Run: make asan && ./sortedset_asan
asan: sortedset.c
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-sanitize=alignment sortedset.c -o sortedset_asan

clean:
	rm -f sortedset sortedset_solution sortedset_asan

.PHONY: all clean asan
