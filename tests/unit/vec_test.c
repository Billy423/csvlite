#include "../../include/vec.h"
#include "../../include/row.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

// Macro for testing assertions
#define TEST(condition, success_message, failure_message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("PASS: %s\n", success_message); \
        } else { \
            printf("FAIL: %s\n", failure_message); \
        } \
    } while(0)

// Test 1: Create and free empty vector
void test_vec_new_and_free(void) {
    Vec *vec = vec_new(10);

    TEST(vec != NULL, "vec_new() returns non-NULL", "vec_new() returns NULL");
    TEST(vec_length(vec) == 0, "New vector has length 0", "New vector has length != 0");
    TEST(vec_capacity(vec) == 10, "New vector has correct capacity", "New vector has incorrect capacity");
    
    vec_free(vec);
    printf("Test 1: vec_new() and vec_free() - Complete\n\n");
}

// Test 2: Create and free vector with zero capacity
void test_vec_zero_capacity(void) {
    Vec *vec = vec_new(0);

    TEST(vec != NULL, "vec_new() returns non-NULL", "vec_new() returns NULL");
    TEST(vec_length(vec) == 0, "New vector has length 0", "New vector has length != 0");
    TEST(vec_capacity(vec) == 1, "New vector has correct capacity", "New vector has incorrect capacity");
    
    vec_free(vec);
    printf("Test 2: vec_zero_capacity() - Complete\n\n");
}

// Test 3.1: Push and get with Row objects
void test_vec_push_and_get(void) {
    Vec *vec = vec_new(2);
    TEST(vec != NULL, "vec_new() succeeds", "vec_new() fails");
    
    // create new rows
    Row *row1 = row_new(2);
    Row *row2 = row_new(2);
    TEST(row1 != NULL && row2 != NULL, "row_new() succeeds", "row_new() fails");
    
    // set row dummy values
    row_set_cell(row1, 0, "value1");
    row_set_cell(row1, 1, "value2");
    row_set_cell(row2, 0, "value3");
    row_set_cell(row2, 1, "value4");
    
    // push rows to vector
    TEST(vec_push(vec, row1) == 0, "vec_push() succeeds", "vec_push() fails");
    TEST(vec_length(vec) == 1, "Length is 1 after first push", "Length is != 1");
    
    TEST(vec_push(vec, row2) == 0, "vec_push() succeeds", "vec_push() fails");
    TEST(vec_length(vec) == 2, "Length is 2 after second push", "Length is != 2");
    
    // Get rows back
    Row *get1 = vec_get(vec, 0);
    TEST(get1 == row1, "vec_get() returns correct row", "vec_get() returns incorrect row");
    TEST(strcmp(row_get_cell(get1, 0), "value1") == 0,
        "Retrieved row has correct cell value",
        "Retrieved row has incorrect cell value"
    );
    
    Row *get2 = vec_get(vec, 1);
    TEST(get2 == row2, "vec_get() returns correct row", "vec_get() returns incorrect row");
    TEST(strcmp(row_get_cell(get2, 0), "value3") == 0,
        "Retrieved row has correct cell value",
        "Retrieved row has incorrect cell value"
    );
    
    // free rows and vector
    row_free(row1);
    row_free(row2);
    vec_free(vec);
    printf("Test 3: vec_push() and vec_get() with Row objects - Complete\n\n");
}

// Test 3.2: Push validation (NULL checks)
void test_vec_push_validation(void) {
    Vec *vec = vec_new(2);
    TEST(vec != NULL, "vec_new() succeeds", "vec_new() fails");
    
    // Test pushing NULL item (should fail)
    int result = vec_push(vec, NULL);
    TEST(result == -1, "vec_push() with NULL item returns -1", "vec_push() with NULL item returns != -1");
    TEST(vec_length(vec) == 0, "Length remains 0 after failed push", "Length changed after failed push");
    
    // Test pushing to NULL vector (should fail)
    Row *row = row_new(1);
    result = vec_push(NULL, row);
    TEST(result == -1, "vec_push() with NULL vector returns -1", "vec_push() with NULL vector returns != -1");
    
    row_free(row);
    vec_free(vec);
    printf("Test 3b: vec_push() validation - Complete\n\n");
}

// Test 4: Edge cases and NULL handling
void test_vec_edge_cases(void) {
    // Test NULL with all functions
    TEST(vec_length(NULL) == 0, "vec_length(NULL) returns 0", "vec_length(NULL) returns != 0");
    TEST(vec_capacity(NULL) == 0, "vec_capacity(NULL) returns 0", "vec_capacity(NULL) returns != 0");
    TEST(vec_get(NULL, 0) == NULL, "vec_get(NULL, 0) returns NULL", "vec_get(NULL, 0) returns != NULL");
    TEST(vec_get(NULL, 100) == NULL, "vec_get(NULL, 100) returns NULL", "vec_get(NULL, 100) returns != NULL");
    
    Vec *vec = vec_new(5);
    TEST(vec != NULL, "vec_new() succeeds", "vec_new() fails");
    
    // Test invalid index on empty vector
    TEST(vec_get(vec, 0) == NULL,
        "vec_get() on empty vector returns NULL",
        "vec_get() on empty vector returns != NULL"
    );
    TEST(vec_get(vec, 100) == NULL,
        "vec_get() with invalid index returns NULL",
        "vec_get() with invalid index returns != NULL"
    );
    
    vec_free(vec);
    printf("Test 4: Edge cases - Complete\n\n");
}

// Test 5: Different capacity values
void test_vec_different_capacities(void) {
    // Test different capacity values
    Vec *vec1 = vec_new(1);
    TEST(vec1 != NULL, "vec_new(1) succeeds", "vec_new(1) fails");
    TEST(vec_capacity(vec1) == 1, "Capacity is 1", "Capacity is != 1");
    vec_free(vec1);
    
    Vec *vec10 = vec_new(10);
    TEST(vec10 != NULL, "vec_new(10) succeeds", "vec_new(10) fails");
    TEST(vec_capacity(vec10) == 10, "Capacity is 10", "Capacity is != 10");
    vec_free(vec10);
    
    Vec *vec1000 = vec_new(1000);
    TEST(vec1000 != NULL, "vec_new(1000) succeeds", "vec_new(1000) fails");
    TEST(vec_capacity(vec1000) == 1000, "Capacity is 1000", "Capacity is != 1000");
    vec_free(vec1000);
    
    printf("Test 5: Different capacities - Complete\n\n");
}

// Test 6: Auto-resize when capacity is full
void test_vec_auto_resize(void) {
    Vec *vec = vec_new(2);
    TEST(vec != NULL, "vec_new() succeeds", "vec_new() fails");
    TEST(vec_capacity(vec) == 2, "Initial capacity is 2", "Initial capacity is != 2");
    
    // push more than capacity to trigger resize
    size_t num_rows = 5;
    Row *rows[num_rows];
    for (size_t i = 0; i < num_rows; i++) {
        rows[i] = row_new(1);
        row_set_cell(rows[i], 0, "test");
        TEST(vec_push(vec, rows[i]) == 0, "vec_push() succeeds", "vec_push() fails");
    }
    
    TEST(vec_length(vec) == num_rows, "Length is num_rows after num_rows pushes", "Length is != num_rows");
    TEST(vec_capacity(vec) >= num_rows, "Capacity increased after resize", "Capacity did not increase");
    
    // Verify all rows are still accessible
    for (size_t i = 0; i < num_rows; i++) {
        Row *get = vec_get(vec, i);
        TEST(get == rows[i], "vec_get() returns correct row after resize",
             "vec_get() returns incorrect row after resize");
    }
    
    // free rows and vector
    for (size_t i = 0; i < num_rows; i++) {
        row_free(rows[i]);
    }
    vec_free(vec);
    printf("Test 6: vec_auto_resize() - Complete\n\n");
}

int main(void) {
    printf("=== Vec Unit Tests ===\n\n");
    
    test_vec_new_and_free();
    test_vec_zero_capacity();
    test_vec_push_and_get();
    test_vec_push_validation();
    test_vec_edge_cases();
    test_vec_different_capacities();
    test_vec_auto_resize();
    
    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    return (tests_run == tests_passed) ? 0 : 1;
}

