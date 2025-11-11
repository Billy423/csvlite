# Makefile for CSVlite

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
INCLUDES = -Iinclude

TARGET = csvlite

# source files
SOURCES = src/main.c src/vec.c src/hmap.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


# Unit tests configuration
UNIT_TEST_DIR = tests/unit

# Build and run individual unit test (e.g. make test-vec)
test-%: $(UNIT_TEST_DIR)/%_test.c
	@echo "Building and running $* tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_$* $< src/$*.c
	@./test_$*
	@rm -f test_$*

# Run all tests
test: test-vec test-hmap

coverage:
	@echo "Coverage not yet implemented"

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -f test_* $(UNIT_TEST_DIR)/*.o
	rm -f *.gcno *.gcda *.gcov *.exe

.PHONY: all test test-% coverage clean