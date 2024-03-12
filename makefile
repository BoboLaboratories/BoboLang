override CFLAGS = -ansi -Werror -Lbin/lib -Ishared
override CC = gcc

# $@ target
# $^ prerequisites
# $< first prerequisite

SHARED = shared/*.h

MAIN = vm
LIBS = console string_utils list

MAIN_BINARIES = $(addprefix bin/,$(MAIN))
LIBS_BINARIES = $(addprefix bin/lib/,$(LIBS))

all: $(MAIN_BINARIES) boboc

bin/%: %/*.c %/*.h $(SHARED) | makedir
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

# Directive for building the compiler
COMPILER_SHARED_LIBS = console string_utils list
COMPILER_LIBS = compiler/lib/*/*.c compiler/lib/*/*.h
boboc: compiler/*.c compiler/*/*.c compiler/include/*/*.h $(COMPILER_LIBS) $(COMPILER_SHARED_LIBS)
	$(CC) $(CFLAGS) -Icompiler/include $(filter %.c,$^) $(addprefix -l:,$(COMPILER_SHARED_LIBS)) -o bin/$@ -lm

list: shared/impl/structures/list.c shared/lib/structures/list.h | makedir
	$(CC) $(CFLAGS) -c $< -o bin/lib/$@

# Directive for making any library
%: shared/impl/%.c shared/lib/%.h | makedir
	$(CC) $(CFLAGS) -c $< -o bin/lib/$@

# Simple clean directive
clean:
	echo -n "Cleaning up.."
	rm -rf bin
	echo " done."

# Creates the output directory
makedir:
	mkdir -p bin/lib

.SILENT: clean makedir