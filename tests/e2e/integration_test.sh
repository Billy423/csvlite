#!/bin/bash

# Minimal integration test for CSVLite
# Tests core functionality end-to-end

# Test data file
TEST_FILE="test_integration_data.csv"

TESTS_RUN=0

# Cleanup function
cleanup() {
    rm -f "$TEST_FILE"
}

trap cleanup EXIT

# Create test CSV file
create_test_data() {
    cat > "$TEST_FILE" <<- 'EOF'
        name,age,department,salary
        Alice,25,Engineering,75000
        Bob,30,Sales,55000
        Charlie,25,Engineering,80000
        Diana,28,Marketing,60000
        Eve,25,Sales,52000
EOF
}

# Test function: runs command and shows expected vs actual
test() {
    local test_name="$1"
    local command="$2"
    local expected_desc="$3"
    
    echo "Test: $test_name"
    echo "  Command: $command"
    echo "  Expected: $expected_desc"
    echo "  Output:"

    TESTS_RUN=$((TESTS_RUN + 1))

    # Run command and show output
    eval "$command" 2>&1 | sed 's/^/    /' # indent output
    
    echo ""
}

echo ""
echo "=== CSVLite Integration Tests ==="
echo ""

# Find binary
BINARY="./csvlite"
if [ ! -f "$BINARY" ]; then
    BINARY="./csvlite.exe"
fi

if [ ! -f "$BINARY" ]; then
    echo "Error: csvlite binary not found. Please build it first."
    exit 1
fi

# Create test data
create_test_data

echo "Running core functionality tests..."
echo ""

# Display CSV file contents
echo "== CSV file contents: =="
cat "$TEST_FILE"
echo "========================"
echo ""

# Test 1: Basic CSV reading
test "Basic CSV reading" \
    "$BINARY --file $TEST_FILE" \
    "Should display all rows with header"

# Test 2: SELECT operation
test "SELECT operation" \
    "$BINARY --file $TEST_FILE --select name,age" \
    "Should display only name and age columns"

# Test 3: GROUP BY operation
test "GROUP BY operation" \
    "$BINARY --file $TEST_FILE --group-by department" \
    "Should group rows by department (one row per unique department)"

# Test 4: ORDER BY operation
test "ORDER BY ascending" \
    "$BINARY --file $TEST_FILE --order-by age" \
    "Should sort rows by age in ascending order"

test "ORDER BY descending" \
    "$BINARY --file $TEST_FILE --order-by age:desc" \
    "Should sort rows by age in descending order"

# Test 5: WHERE operation
test "WHERE operation" \
    "$BINARY --file $TEST_FILE --where 'age>=25'" \
    "Should filter rows by age >= 25"

# Test 6: Combined operations
test "Combined: WHERE + GROUP BY + ORDER BY + SELECT" \
    "$BINARY --file $TEST_FILE --where 'age>=25' --group-by department --order-by salary:desc --select name,department" \
    "Should filter rows by age >= 25, group by department, sort by salary desc, and show only name and department"

# Test 7: Stdin input
test "Stdin input" \
    "cat $TEST_FILE | $BINARY -" \
    "Should read CSV from stdin"

# Test 8: ORDER BY with explicit asc
test "ORDER BY explicit ascending" \
    "$BINARY --file $TEST_FILE --order-by age:asc" \
    "Should sort rows by age in ascending order (explicit asc)"

# Test 9: Numeric column index in SELECT
test "SELECT with numeric column index" \
    "$BINARY --file $TEST_FILE --select 0,1" \
    "Should select columns by numeric index (0=name, 1=age)"

# Test 10: Numeric column index in GROUP BY
test "GROUP BY with numeric column index" \
    "$BINARY --file $TEST_FILE --group-by 2" \
    "Should group by column index 2 (department)"

# Test 11: Numeric column index in ORDER BY
test "ORDER BY with numeric column index" \
    "$BINARY --file $TEST_FILE --order-by 1" \
    "Should sort by column index 1 (age)"

# Test 12: Single row CSV (header only)
test "Single row CSV (header only)" \
    "echo 'name,age' | $BINARY -" \
    "Should handle CSV with only header row"

# Test 13: Help flag
test "Help flag" \
    "$BINARY --help" \
    "Should display help message and exit with code 0"

# Test 14: Invalid CLI argument
test "Invalid CLI argument" \
    "$BINARY --invalid-arg 2>&1" \
    "Should show error for unknown argument"

# Test 15: Missing file path
test "Missing file path" \
    "$BINARY --file 2>&1" \
    "Should show error for missing file path"

# Test 16: Non-existent file
test "Non-existent file" \
    "$BINARY --file lol.csv 2>&1" \
    "Should show error for file that cannot be opened"

# Test 17: Invalid column in SELECT
test "Invalid column in SELECT" \
    "$BINARY --file $TEST_FILE --select invalid_col 2>&1" \
    "Should show error for invalid column selection"

# Test 18: Invalid column in GROUP BY
test "Invalid column in GROUP BY" \
    "$BINARY --file $TEST_FILE --group-by invalid_col 2>&1" \
    "Should show error for invalid GROUP BY column"

# Test 19: Invalid column in ORDER BY
test "Invalid column in ORDER BY" \
    "$BINARY --file $TEST_FILE --order-by invalid_col 2>&1" \
    "Should show error for invalid ORDER BY column"

# Test 20: Empty CSV file
test "Empty CSV file" \
    "$BINARY - <<< '' 2>&1" \
    "Should show error for empty CSV file"

# Test 22: SELECT with numeric index out of bounds
test "SELECT with out-of-bounds numeric index" \
    "$BINARY --file $TEST_FILE --select 999 2>&1" \
    "Should handle invalid numeric column index"

# Test 24: Combined operations with numeric indices
test "Combined operations with numeric indices" \
    "$BINARY --file $TEST_FILE --where 'age>=25' --group-by 2 --order-by 3:desc --select 0,2" \
    "Should work with numeric column indices in all operations"

# Test 25: No file specified (should use stdin or error)
test "No file specified" \
    "$BINARY --select name 2>&1" \
    "Should show error when no input file specified"

# Test 26: ORDER BY with only header (single row CSV)
test "ORDER BY with single row CSV" \
    "echo 'name,age' | $BINARY - --order-by age" \
    "Should handle ORDER BY with only header row (early return)"

# Test 27: Numeric index that's valid
test "GROUP BY with valid numeric index" \
    "$BINARY --file $TEST_FILE --group-by 2" \
    "Should group by valid numeric column index"

# Test 28: Numeric index that's out of bounds
test "GROUP BY with out-of-bounds numeric index" \
    "$BINARY --file $TEST_FILE --group-by 99 2>&1" \
    "Should error on out-of-bounds numeric index"

# Test 29: SELECT with empty string (edge case)
test "SELECT with empty columns" \
    "$BINARY --file $TEST_FILE --select '' 2>&1" \
    "Should handle empty SELECT clause"

# Test 30: WHERE with invalid condition
test "WHERE with potentially invalid condition" \
    "$BINARY --file $TEST_FILE --where 'invalid_condition' 2>&1" \
    "Should handle WHERE with invalid condition"

# Test 31: ORDER BY with very long column name (truncation test)
# Create a column name longer than 256 chars
FILE="test_long_col.csv"
LONG_COL_NAME=""
for i in {1..300}; do
    LONG_COL_NAME="${LONG_COL_NAME}a"
done
echo "${LONG_COL_NAME},age" > "$FILE"
echo "Alice,25" >> "$LONG_COL_FILE"
test "ORDER BY with very long column name" \
    "$BINARY --file $FILE --order-by ${LONG_COL_NAME}:desc 2>&1" \
    "Should handle very long column name (truncation)"
rm -f "$FILE"

# Test 32: Malformed CSV (missing columns in some rows)
FILE="test_malformed.csv"
echo "name,age,department" > "$FILE"
echo "Alice,25" >> "$FILE"  # Missing department column
test "Malformed CSV handling" \
    "$BINARY --file $FILE 2>&1" \
    "Should handle malformed CSV gracefully"
rm -f "$FILE"

# Test 33: Empty SELECT (NULL select_cols)
test "No SELECT clause" \
    "$BINARY --file $TEST_FILE" \
    "Should work without SELECT clause"

# Test 34: Numeric index in ORDER BY with colon syntax
test "ORDER BY numeric index with direction" \
    "$BINARY --file $TEST_FILE --order-by 1:desc" \
    "Should handle numeric index with direction in ORDER BY"

echo "=== Tests Complete ==="
echo "Tests run: $TESTS_RUN"
