CC = gcc
CFLAGS = -Wall -g -O0

all: scheme scheme_solution

scheme: scheme.c
	$(CC) $(CFLAGS) scheme.c -o scheme

scheme_solution: scheme_solution.c
	$(CC) $(CFLAGS) scheme_solution.c -o scheme_solution

# AddressSanitizer: finds leaked intermediate strings, free() of a string literal
# or of a pointer into a node, use after free.
# -fno-sanitize=alignment: on 64-bit the two addresses of a List node start at
# offset 4, not on an 8-byte boundary (the handout packs them right after the tag).
# x86 doesn't mind.
# Linux / WSL / macOS only (not MinGW). Run: make asan && ./scheme_asan
asan: scheme.c
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-sanitize=alignment scheme.c -o scheme_asan

clean:
	rm -f scheme scheme_solution scheme_asan

.PHONY: all clean asan
