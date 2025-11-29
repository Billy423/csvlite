# Makefile for CSVlite project
# 
# AUTHOR: Team 21
# DATE: November 29, 2025
# VERSION: v2.0.0
#################################

# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
INCLUDES = -Iinclude

# Target executable
TARGET = csvlite

# Source files
SOURCES = src/main.c src/cli.c src/csv.c src/row.c src/vec.c src/hmap.c src/select.c src/sort.c src/group.c src/where.c
OBJECTS = $(SOURCES:.c=.o)

# Unit tests configuration
UNIT_TEST_DIR = tests/unit

# Main application build target
all: $(TARGET)

# Main application
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run all tests
test-unit: test-row test-vec test-hmap test-csv test-cli test-select test-sort test-group test-where
test: test-unit test-e2e

# Special handling for vec which depends on row
test-vec: $(UNIT_TEST_DIR)/vec_test.c
	@echo "================================================"
	@echo "Building and running vec tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_vec $< src/vec.c src/row.c
	@./test_vec
	@rm -f test_vec

# Test csv
test-csv: $(UNIT_TEST_DIR)/csv_test.c
	@echo "================================================"
	@echo "Building and running csv tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_csv $< src/csv.c src/row.c src/vec.c src/hmap.c
	@./test_csv
	@rm -f test_csv

# Test cli
test-cli: $(UNIT_TEST_DIR)/cli_test.c
	@echo "================================================"
	@echo "Building and running cli tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_cli $< src/cli.c src/csv.c src/row.c src/vec.c src/hmap.c
	@./test_cli
	@rm -f test_cli

# Test select
test-select: $(UNIT_TEST_DIR)/select_test.c
	@echo "================================================"
	@echo "Building and running select tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_select $< src/select.c src/vec.c src/row.c src/hmap.c
	@./test_select
	@rm -f test_select

# Test group 
test-group: $(UNIT_TEST_DIR)/group_test.c
	@echo "================================================"
	@echo "Building and running group tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_group $< src/group.c src/row.c src/vec.c src/hmap.c
	@./test_group
	@rm -f test_group
	
# Test sort
test-sort: $(UNIT_TEST_DIR)/sort_test.c
	@echo "================================================"
	@echo "Building and running sort tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_sort $< src/sort.c src/vec.c src/row.c
	@./test_sort
	@rm -f test_sort

# Test where
test-where: $(UNIT_TEST_DIR)/where_test.c
	@echo "================================================"
	@echo "Building and running where tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_where $< src/where.c src/vec.c src/row.c
	@./test_where
	@rm -f test_where

# Test general module
test-%: $(UNIT_TEST_DIR)/%_test.c
	@echo "================================================"
	@echo "Building and running $* tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_$* $< src/$*.c
	@./test_$*
	@rm -f test_$*


# Coverage testing with gcov
coverage: clean
	@echo "================================================"
	@echo "Step 1: Compiling with coverage flags..."
	@echo "================================================"
	@$(MAKE) test-unit CFLAGS="$(CFLAGS) --coverage"
	@echo ""
	@echo "Building main executable with coverage flags..."
	@$(CC) $(CFLAGS) --coverage $(INCLUDES) -o $(TARGET) $(SOURCES)
	@echo "Running integration tests to generate coverage for main.c..."
	@bash tests/e2e/integration_test.sh 2>&1 || true
	@echo ""
	@echo "================================================"
	@echo "Step 2: Tests executed (generated .gcda files)"
	@echo "================================================"
	@echo ""
	@echo "================================================"
	@echo "Step 3: Generating coverage reports with gcov..."
	@echo "================================================"
	@echo "Coverage reports:"
	@for file in $(SOURCES); do \
		bash scripts/generate_coverage.sh $$file; \
	done
	@echo ""
	@echo "Coverage reports generated. Check coverage_reports/ directory for .gcov files."


# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -f test_* $(UNIT_TEST_DIR)/*.o src/*.o
	rm -f *.gcno *.gcda *.gcov *.exe
	rm -rf coverage_reports

# Run integration tests
test-e2e: $(TARGET)
	@echo "Running integration tests..."
	@bash tests/e2e/integration_test.sh

# Phony targets
.PHONY: all test test-vec test-sort test-% test-e2e coverage clean