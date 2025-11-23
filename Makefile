CC       = gcc
CFLAGS   = -std=c11 -Wall -Wextra -Werror
INCLUDES = -Iinclude
LDFLAGS  =

TARGET = csvlite

# Enable coverage with: make COVERAGE=1 test / make COVERAGE=1 all
ifeq ($(COVERAGE),1)
CFLAGS  += --coverage
LDFLAGS += --coverage
endif

# Source files
SOURCES = src/main.c src/cli.c src/csv.c src/row.c src/vec.c src/hmap.c src/select.c src/sort.c src/group.c src/where.c
OBJECTS = $(SOURCES:.c=.o)

# Unit tests configuration
UNIT_TEST_DIR = tests/unit

all: $(TARGET)

# Main application
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

# Object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run all unit tests (orchestration style)
test-unit: test-row test-vec test-hmap test-csv test-cli test-select test-sort test-group test-where

# Alias so workflow's `make unit-test` works
unit-test: test-unit

# Full test target = unit + integration
test: test-unit test-e2e

# Special handling for vec which depends on row
test-vec: $(UNIT_TEST_DIR)/vec_test.c
	@echo "================================================"
	@echo "Building and running vec tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_vec $< src/vec.c src/row.c $(LDFLAGS)
	@./test_vec
	@rm -f test_vec

# Test csv
test-csv: $(UNIT_TEST_DIR)/csv_test.c
	@echo "================================================"
	@echo "Building and running csv tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_csv $< src/csv.c src/row.c src/vec.c src/hmap.c $(LDFLAGS)
	@./test_csv
	@rm -f test_csv

# Test cli
test-cli: $(UNIT_TEST_DIR)/cli_test.c
	@echo "================================================"
	@echo "Building and running cli tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_cli $< src/cli.c src/csv.c src/row.c src/vec.c src/hmap.c $(LDFLAGS)
	@./test_cli
	@rm -f test_cli

# Special handling for select which depends on row, vec, and hmap
test-select: $(UNIT_TEST_DIR)/select_test.c
	@echo "================================================"
	@echo "Building and running select tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_select $< src/select.c src/vec.c src/row.c src/hmap.c $(LDFLAGS)
	@./test_select
	@rm -f test_select

# Test group 
test-group: $(UNIT_TEST_DIR)/group_test.c
	@echo "================================================"
	@echo "Building and running group tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_group $< src/group.c src/row.c src/vec.c src/hmap.c $(LDFLAGS)
	@./test_group
	@rm -f test_group

# Test sort
test-sort: $(UNIT_TEST_DIR)/sort_test.c
	@echo "================================================"
	@echo "Building and running sort tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_sort $< src/sort.c src/vec.c src/row.c $(LDFLAGS)
	@./test_sort
	@rm -f test_sort

# Special handling for where which depends on row and vec
test-where: $(UNIT_TEST_DIR)/where_test.c
	@echo "================================================"
	@echo "Building and running where tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_where $< src/where.c src/vec.c src/row.c $(LDFLAGS)
	@./test_where
	@rm -f test_where

# Build and run individual unit test (e.g. make test-vec)
test-%: $(UNIT_TEST_DIR)/%_test.c
	@echo "================================================"
	@echo "Building and running $* tests..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o test_$* $< src/$*.c $(LDFLAGS)
	@./test_$*
	@rm -f test_$*

# Coverage testing with gcov (orchestration version)
coverage: clean
	@echo "================================================"
	@echo "Step 1: Compiling with coverage flags..."
	@echo "================================================"
	@$(MAKE) test-unit CFLAGS="$(CFLAGS) --coverage"
	@echo ""
	@echo "Building main executable with coverage flags..."
	@$(CC) $(CFLAGS) --coverage $(INCLUDES) -o $(TARGET) $(SOURCES)
	@echo "Running integration tests to generate coverage for main.c..."
	@bash tests/e2e/integration_test.sh > /dev/null 2>&1 || true
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

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -f test_* $(UNIT_TEST_DIR)/*.o src/*.o
	rm -f *.gcno *.gcda *.gcov *.exe
	rm -rf coverage_reports

# Integration tests target
test-e2e: $(TARGET)
	@echo "Running integration tests..."
	@bash tests/e2e/integration_test.sh

.PHONY: all test test-unit unit-test test-vec test-sort test-group test-where test-% test-e2e coverage clean
