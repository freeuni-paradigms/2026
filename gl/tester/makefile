include $(GL_LIB_PATH)/makefile_base

# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

run_tests: run_tests.c student problem
	$(CC) $(CFLAGS) -c run_tests.c -o $(OUTDIR)/$@.o
	OUT_DIR=$(OUTDIR) make -C $(GL_LIB_PATH)/containers list
	OUT_DIR=$(OUTDIR) make -C $(GL_LIB_PATH)/threads pool
	$(LD) $(LDFLAGS) $(OUTDIR)/$@.o $(OUTDIR)/student.o $(OUTDIR)/problem.o \
		 $(OUTDIR)/list.o $(OUTDIR)/pool.o -pthread -o $(OUTDIR)/run_tests

student: student.c student.h
	$(CC) $(CFLAGS) -c student.c -o $(OUTDIR)/$@.o

problem: problem.c problem.h
	$(CC) $(CFLAGS) -c problem.c -o $(OUTDIR)/$@.o

