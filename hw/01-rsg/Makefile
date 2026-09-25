GL_LIB_PATH ?= ../../gl
OUT_DIR ?= $(CURDIR)

include $(GL_LIB_PATH)/makefile_base

.DEFAULT_GOAL := rsg

CXX ?= g++
CXX_STANDARD := -std=c++17
override CXXFLAGS += $(filter-out -std=% -Wall -Werror,$(CFLAGS)) \
	-Wall $(CXX_STANDARD) -Werror
GL_CFLAGS := $(filter-out -Wall -Werror,$(CFLAGS)) -Wall -Werror

CORE_OBJECTS := \
	$(OUTDIR)/rsg.o \
	$(OUTDIR)/grammar.o \
	$(OUTDIR)/definition.o \
	$(OUTDIR)/production.o \
	$(OUTDIR)/random.o

LUNIT_OBJECTS := \
	$(OUTDIR)/lunit.o \
	$(OUTDIR)/cmp.o \
	$(OUTDIR)/list.o

.PHONY: rsg rsg_lib run_tests clean_rsg

clean: clean_rsg

clean_rsg:
	rm -rvf $(OUTDIR)/rsg $(OUTDIR)/run_tests $(OUTDIR)/tests.o

rsg_lib: $(CORE_OBJECTS)

rsg: rsg_lib $(OUTDIR)/main.o
	$(CXX) $(LDFLAGS) $(OUTDIR)/main.o $(CORE_OBJECTS) -o $(OUTDIR)/$@

$(OUTDIR)/tests.o: tests.c
	$(CXX) $(CXXFLAGS) -x c++ -c $< -o $@

run_tests: $(CORE_OBJECTS) $(OUTDIR)/tests.o
	$(MAKE) -C $(GL_LIB_PATH)/lunit OUT_DIR=$(OUTDIR) \
		CFLAGS="$(GL_CFLAGS)" lunit
	$(CXX) $(LDFLAGS) $(OUTDIR)/tests.o $(CORE_OBJECTS) $(LUNIT_OBJECTS) -o $(OUTDIR)/$@

$(OUTDIR)/%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@
