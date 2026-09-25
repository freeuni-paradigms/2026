include $(GL_LIB_PATH)/makefile_base

%_test: %_test.c lunit
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@.o
	$(LD) $(LDFLAGS) $(OUTDIR)/$@.o lunit.o $(OUTDIR)/list.o $(OUTDIR)/cmp.o -o $(OUTDIR)/$@
	./$@

cmp: cmp.c
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@.o

lunit: lunit.c cmp
	OUT_DIR=$(OUTDIR) make -C $(GL_LIB_PATH)/containers list
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/lunit.o
