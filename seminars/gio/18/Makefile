CC = gcc
CFLAGS = -Wall -g -O0 -pthread

all: arr_sum

arr_sum: arr_sum.c
	$(CC) $(CFLAGS) arr_sum.c -o arr_sum

solution: arr_sum_solution.c
	$(CC) $(CFLAGS) arr_sum_solution.c -o arr_sum_solution

# ThreadSanitizer: reports two threads using the same memory without a lock,
# even when the sum comes out right. With it the two big tests are 100 times smaller.
# Linux / WSL only (not MinGW). Run: make tsan && ./arr_sum_tsan
tsan: arr_sum.c
	$(CC) $(CFLAGS) -fsanitize=thread arr_sum.c -o arr_sum_tsan

clean:
	rm -f arr_sum arr_sum_solution arr_sum_tsan arr_sum.exe arr_sum_solution.exe

.PHONY: all solution tsan clean
