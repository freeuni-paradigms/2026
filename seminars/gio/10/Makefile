CC = gcc
CFLAGS = -Wall -g -O0

all: packed_vector

packed_vector: packed_vector.c
	$(CC) $(CFLAGS) packed_vector.c -o packed_vector

solution: packed_vector_solution.c
	$(CC) $(CFLAGS) packed_vector_solution.c -o packed_vector_solution

# AddressSanitizer: finds leaks, double free, use after free, writes past the block.
# -fno-sanitize=alignment: the elements are packed with no gaps, so a FreeFn or a
# double inside the block often starts at an address that isn't a multiple of 8.
# x86 doesn't mind.
# Linux / WSL / macOS only (not MinGW). Run: make asan && ./packed_vector_asan
asan: packed_vector.c
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-sanitize=alignment packed_vector.c -o packed_vector_asan

clean:
	rm -f packed_vector packed_vector_solution packed_vector_asan

.PHONY: all solution asan clean
