# Light3D build. Every target recompiles all sources in one command:
# the project is small, and this keeps the Makefile easy to read.

CC      ?= cc
CFLAGS   = -std=c11 -Wall -Wextra -Wpedantic -Werror
OPT      = -O2 -g
DEBUG    = -O0 -g -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
LDLIBS   = -lm

HEADERS  = $(wildcard src/*.h)
LIB_SRC  = $(filter-out src/main.c,$(wildcard src/*.c))  # everything except main
TESTS    = $(wildcard tests/test_*.c)                     # one program per test file

TEST_BINS       = $(patsubst tests/%.c,build/tests/%,$(TESTS))
DEBUG_TEST_BINS = $(patsubst tests/%.c,build/debug-tests/%,$(TESTS))

.PHONY: all run debug test debug-test clean

all: build/rt

build/rt: src/main.c $(LIB_SRC) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OPT) -o $@ src/main.c $(LIB_SRC) $(LDLIBS)

build/rt-debug: src/main.c $(LIB_SRC) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG) -o $@ src/main.c $(LIB_SRC) $(LDLIBS)

build/tests/%: tests/%.c tests/test.h $(LIB_SRC) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OPT) -Isrc -o $@ $< $(LIB_SRC) $(LDLIBS)

build/debug-tests/%: tests/%.c tests/test.h $(LIB_SRC) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG) -Isrc -o $@ $< $(LIB_SRC) $(LDLIBS)

run: build/rt
	./build/rt out.ppm

debug: build/rt-debug

# Run every test program, then fail if any of them failed.
test: $(TEST_BINS)
	@fail=0; for t in $^; do ./$$t || fail=1; done; exit $$fail

debug-test: $(DEBUG_TEST_BINS)
	@fail=0; for t in $^; do ./$$t || fail=1; done; exit $$fail

clean:
	rm -rf build out.ppm
