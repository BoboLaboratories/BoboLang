override CFLAGS = -ansi -Werror
override CC = gcc

# $@ target
# $^ prerequisites
# $< first prerequisite

SHARED = shared/*.h

ALL = compiler vm sandbox
ALL_BINS = $(addprefix bin/,$(ALL))

all: $(ALL_BINS)

bin/sandbox: sandbox/*.c | makedir
	$(CC) $(CFLAGS) $< -o $@

bin/%: %/*.c %/*.h $(SHARED) | makedir
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

# Simple clean directive
clean:
	echo -n "Cleaning up.."
	rm -rf bin
	echo " done."

# Creates the output directory
makedir:
	mkdir -p bin

.SILENT: clean makedir