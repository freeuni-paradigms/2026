export GL_LIB_PATH ?= $(CURDIR)/../../gl
export OUT_DIR ?= $(CURDIR)

include $(GL_LIB_PATH)/makefile_base

.DEFAULT_GOAL := six-degrees

CXX ?= g++
CXX_STANDARD := -std=c++17
GL_INCLUDE := -I$(GL_LIB_PATH)/..
override CXXFLAGS += $(filter-out -std=% -Wall -Werror,$(CFLAGS)) \
	$(GL_INCLUDE) -Wall $(CXX_STANDARD) -Werror
GL_CFLAGS := $(filter-out -std=% -Wall -Werror,$(CFLAGS)) \
	$(GL_INCLUDE) -Wall -std=gnu99 -Werror

CORE_OBJECTS := \
	$(OUTDIR)/imdb.o \
	$(OUTDIR)/path.o \
	$(OUTDIR)/six-degrees.o

LUNIT_OBJECTS := \
	$(OUTDIR)/lunit.o \
	$(OUTDIR)/cmp.o \
	$(OUTDIR)/list.o

$(CORE_OBJECTS) $(OUTDIR)/main.o $(OUTDIR)/tests.o: | $(OUTDIR)

$(OUTDIR):
	mkdir -p $@

.PHONY: six-degrees six_degrees_lib run_tests clean_six_degrees

clean: clean_six_degrees

clean_six_degrees:
	rm -rvf $(OUTDIR)/six-degrees $(OUTDIR)/run_tests $(OUTDIR)/tests.o

six_degrees_lib: $(CORE_OBJECTS)

six-degrees: six_degrees_lib $(OUTDIR)/main.o
	$(CXX) $(LDFLAGS) $(OUTDIR)/main.o $(CORE_OBJECTS) -o $(OUTDIR)/$@

$(OUTDIR)/tests.o: tests.cc
	$(CXX) $(CXXFLAGS) -Wno-write-strings -c $< -o $@

run_tests: six_degrees_lib $(OUTDIR)/tests.o
	$(MAKE) -C $(GL_LIB_PATH)/lunit OUT_DIR=$(OUTDIR) \
		CFLAGS="$(GL_CFLAGS)" lunit
	$(CXX) $(LDFLAGS) $(OUTDIR)/tests.o $(CORE_OBJECTS) \
		$(LUNIT_OBJECTS) -o $(OUTDIR)/$@

$(OUTDIR)/%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@
