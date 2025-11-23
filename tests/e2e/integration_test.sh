#!/bin/bash

# Minimal integration test for CSVLite
# Tests core functionality end-to-end

# Test data file
TEST_FILE="test_integration_data.csv"

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

# Test 6: Stdin input
test "Stdin input" \
    "cat $TEST_FILE | $BINARY -" \
    "Should read CSV from stdin"

echo "=== Tests Complete ==="
