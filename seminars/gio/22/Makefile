CC = gcc
CFLAGS = -Wall -g -O0 -pthread
FILES = robot.c tests.c robot.h

all: competition

# the exam's way: gcc tests.c competition.c robot.c -pthread
competition: competition.c competition.h $(FILES)
	$(CC) $(CFLAGS) competition.c robot.c tests.c -o competition

# The solution is in solution/. Its competition.h has the same include guard as
# ./competition.h, and -include reads it first, so tests.c gets the solution's struct.
solution: solution/competition.c solution/competition.h $(FILES)
	$(CC) $(CFLAGS) -I. -include solution/competition.h solution/competition.c robot.c tests.c -o competition_solution

# ThreadSanitizer: reports two threads using the same memory without a lock,
# and a mutex unlocked by a thread that didn't lock it.
# Linux / WSL only (not MinGW). Run: make tsan && ./competition_tsan
tsan: competition.c competition.h $(FILES)
	$(CC) $(CFLAGS) -fsanitize=thread competition.c robot.c tests.c -o competition_tsan

clean:
	rm -f competition competition_solution competition_tsan competition.exe competition_solution.exe

.PHONY: all solution tsan clean
