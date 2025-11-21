CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
INCLUDES = -Iinclude

TARGET = csvlite

# Source files
SOURCES = src/main.c src/cli.c src/csv.c src/row.c src/vec.c src/hmap.c src/select.c src/sort.c
OBJECTS = $(SOURCES:.c=.o)

# Unit tests configuration
UNIT_TEST_DIR = tests/unit

all: $(TARGET)

# Main application
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run all tests
test: test-row test-vec test-hmap test-csv test-cli test-select test-sort

# Special handling for vec which depends on row
test-vec: $(UNIT_TEST_DIR)/vec_test.c
	@echo "Building and running vec tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_vec $< src/vec.c src/row.c
	@./test_vec
	@rm -f test_vec

# Test csv
test-csv: $(UNIT_TEST_DIR)/csv_test.c
	@echo "Building and running csv tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_csv $< src/csv.c src/row.c src/vec.c src/hmap.c
	@./test_csv
	@rm -f test_csv

# Test cli
test-cli: $(UNIT_TEST_DIR)/cli_test.c
	@echo "Building and running cli tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_cli $< src/cli.c src/csv.c src/row.c src/vec.c src/hmap.c
	@./test_cli
	@rm -f test_cli


# Special handling for select which depends on row, vec, and hmap
test-select: $(UNIT_TEST_DIR)/select_test.c
	@echo "Building and running select tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_select $< src/select.c src/vec.c src/row.c src/hmap.c
	@./test_select
	@rm -f test_select

# Test sort
test-sort: $(UNIT_TEST_DIR)/sort_test.c
	@echo "Building and running sort tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_sort $< src/sort.c src/vec.c src/row.c
	@./test_sort
	@rm -f test_sort
 
# Build and run individual unit test (e.g. make test-vec)
test-%: $(UNIT_TEST_DIR)/%_test.c
	@echo "Building and running $* tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_$* $< src/$*.c
	@./test_$*
	@rm -f test_$*

coverage:
	@echo "Coverage not yet implemented"

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -f test_* $(UNIT_TEST_DIR)/*.o src/*.o
	rm -f *.gcno *.gcda *.gcov *.exe

.PHONY: all test test-vec test-sort test-% coverage clean
