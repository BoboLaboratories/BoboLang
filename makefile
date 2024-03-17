override CFLAGS = -ansi -Werror -Lbin/lib -Ishared
override CC = gcc

# $@ target
# $^ prerequisites
# $< first prerequisite

all: boboc bobo

# Directive for building the compiler
COMPILER_SHARED_LIBS = console string_utils data/arraylist data/hashtable
COMPILER_SHARED_LIBS_PATHS = $(addprefix bin/lib/,$(COMPILER_SHARED_LIBS))
COMPILER_SOURCES := $(shell find compiler -type f \( -iname \*.c -o -iname \*.h \))
boboc: $(COMPILER_SOURCES) $(COMPILER_SHARED_LIBS_PATHS)
	$(CC) $(CFLAGS) -Icompiler/include $(filter %.c,$^) $(addprefix -l:,$(COMPILER_SHARED_LIBS)) -o bin/$@ -lm
.SILENT: boboc

bobo: vm/*
	$(CC) $(CFLAGS) $(filter %.c,$^) -o bin/$@
.SILENT: bobo

# Data structure libraries
bin/lib/data/arraylist: shared/lib/data/arraylist/* | makedir
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@
.SILENT: bin/lib/data/arraylist

# Data structure libraries
bin/lib/data/hashtable: shared/lib/data/hashtable/* | makedir
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@
.SILENT: bin/lib/data/hashtable

# Misc libraries
bin/lib/console: shared/lib/console/* | makedir
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@
.SILENT: bin/lib/console

# Misc libraries
bin/lib/string_utils: shared/lib/string_utils/* | makedir
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@
.SILENT: bin/lib/string_utils

# Simple clean directive
clean:
	#echo -n "Cleaning up.."
	rm -rf bin
	#echo " done."
.SILENT: clean

# Creates the output directory
makedir:
	mkdir -p bin/lib/data
.SILENT: makedir