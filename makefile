override CFLAGS = -ansi -Werror -Ishared -Lbin/lib
override CC = gcc

# $@ target
# $^ prerequisites
# $< first prerequisite

SHARED = shared/*.h

MAIN = compiler vm
LIBS = console

MAIN_BINARIES = $(addprefix bin/,$(MAIN))
LIBS_BINARIES = $(addprefix bin/lib/,$(LIBS))

all: $(MAIN_BINARIES)

bin/sandbox: sandbox/*.c | makedir
	$(CC) $(CFLAGS) $< -o $@

bin/%: %/*.c %/*.h $(SHARED) | makedir
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

bin/compiler: compiler/*.c compiler/*.h console
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@ -l:console

# Directive for making any library
%: shared/impl/%.c shared/lib/%.h | makedir
	$(CC) $(CFLAGS) -c -o bin/lib/$@ $<

# Simple clean directive
clean:
	echo -n "Cleaning up.."
	rm -rf bin
	echo " done."

# Creates the output directory
makedir:
	mkdir -p bin/lib

.SILENT: clean makedir