include $(GL_LIB_PATH)/makefile_base

# TODO(giolekva): remove hardcoded list
%_test: %_test.c list
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@.o
	OUT_DIR=$(OUTDIR) make -C $(GL_LIB_PATH)/lunit lunit
	$(LD) $(OUTDIR)/$@.o $(OUTDIR)/list.o $(OUTDIR)/lunit.o $(OUTDIR)/cmp.o -o $(OUTDIR)/$@
	./$@

list: list.c
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@.o
