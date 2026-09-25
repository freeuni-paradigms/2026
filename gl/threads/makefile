include $(GL_LIB_PATH)/makefile_base

# TODO(giolekva): remove hardcoded pool
%_test: %_test.c pool
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@.o
	OUT_DIR=$(OUTDIR) make -C $(GL_LIB_PATH)/lunit lunit
	$(LD) $(LDFLAGS) $@.o $(OUTDIR)/pool.o $(OUTDIR)/lunit.o $(OUTDIR)/list.o $(OUTDIR)/cmp.o -o $(OUTDIR)/$@
	./$@

pool: pool.c
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@.o
