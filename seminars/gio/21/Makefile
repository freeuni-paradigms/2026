CC = gcc
CFLAGS = -Wall -g -O0 -pthread
FILES = helper.c tests.c dj_school.h helper.h

all: dj_school

# the exam's way: gcc dj_school.c helper.c tests.c -pthread
dj_school: dj_school.c $(FILES)
	$(CC) $(CFLAGS) dj_school.c helper.c tests.c -o dj_school

solution: dj_school_solution.c $(FILES)
	$(CC) $(CFLAGS) dj_school_solution.c helper.c tests.c -o dj_school_solution

# ThreadSanitizer: reports two threads using the same memory without a lock,
# and a mutex unlocked by a thread that didn't lock it.
# Linux / WSL only (not MinGW). Run: make tsan && ./dj_school_tsan
tsan: dj_school.c $(FILES)
	$(CC) $(CFLAGS) -fsanitize=thread dj_school.c helper.c tests.c -o dj_school_tsan

clean:
	rm -f dj_school dj_school_solution dj_school_tsan dj_school.exe dj_school_solution.exe

.PHONY: all solution tsan clean
