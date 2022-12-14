proj := mat47

.PHONY: docs

CFLAGS = -Wall -Wextra -pedantic -c -o $@
TEST_LDFLAGS := -lcriterion
BUILD := build
SRC := src/mat47

MAT47_LOG := DEBUG ERROR
MAT47_LOG_FLAGS := $(patsubst %,-D'MAT47_LOG_%',$(MAT47_LOG))

headers := $(wildcard $(SRC)/*.h)
sources := $(wildcard $(SRC)/*.c)
objects := $(patsubst %.c,%.o,$(subst $(SRC),$(BUILD),$(sources)))

test_sources := $(wildcard tests/*.c)
test_objects := $(patsubst %.c,%.o,$(subst tests,$(BUILD),$(test_sources)))

# Manual tests (untracked)

_test: $(BUILD)/ bin/ bin/_test
	bin/_test

bin/_test: $(BUILD)/_test.o $(objects)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD)/_test.o: _test.c $(SRC)/matrix.h $(SRC)/error.h
	$(CC) $(CFLAGS) $<

$(BUILD)/%.o: $(SRC)/%.c $(headers)
	$(CC) $(CFLAGS) $(MAT47_LOG_FLAGS) $<

# Automated tests (tracked)

test: $(BUILD)/ bin/ bin/test
	bin/test --verbose -j1 -S $(TEST_FLAGS)

bin/test: $(test_objects)
	$(CC) $^ -o $@ $(TEST_LDFLAGS)

$(BUILD)/test_%.o: tests/test_%.c $(SRC)/%.c $(headers)
	$(CC) $(CFLAGS) $<

# Project management

bin/ $(BUILD)/:
	mkdir $@

docs:
	cd docs && make html

clean:
	rm -vrf bin $(BUILD)
