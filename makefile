override CFLAGS = -ansi -Werror -Lbin/lib -Ishared
override CC = gcc

# $@ target
# $^ prerequisites
# $< first prerequisite

MAIN = boboc bobo
LIBS = console string_utils list

MAIN_BINARIES = $(addprefix bin/,$(MAIN))
LIBS_BINARIES = $(addprefix bin/lib/,$(LIBS))

# Directive for building the compiler
COMPILER_SHARED_LIBS = console string_utils data/list
COMPILER_SHARED_LIBS_PATHS = $(addprefix bin/lib/,$(COMPILER_SHARED_LIBS))
# COMPILER_LIBS = compiler/lib/*/*.c compiler/lib/*/*.h
boboc: compiler/* compiler/*/* compiler/*/*/* $(COMPILER_SHARED_LIBS_PATHS)
	$(CC) $(CFLAGS) -Icompiler/include $(filter %.c,$^) $(addprefix -l:,$(COMPILER_SHARED_LIBS)) -o bin/$@ -lm

bobo: vm/*
	$(CC) $(CFLAGS) $(filter %.c,$^) -o bin/$@

# Data structure libraries
bin/lib/data/%: shared/data/%/* | makedir
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@

# Misc libraries
bin/lib/%: shared/lib/%/* | makedir
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@

# Simple clean directive
clean:
	echo -n "Cleaning up.."
	rm -rf bin
	echo " done."

# Creates the output directory
makedir:
	mkdir -p bin/lib/data

.SILENT: clean makedir