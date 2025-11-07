#include "vec.h"
#include "row.h"
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

// Test 3: Push validation (NULL checks only)
// Note: Full push/get testing will be done when Row is implemented
void test_vec_push_validation(void) {
    Vec *vec = vec_new(2);
    TEST(vec != NULL, "vec_new() succeeds", "vec_new() fails");
    
    // Test pushing NULL item (should fail)
    int result = vec_push(vec, NULL);
    TEST(result == -1, "vec_push() with NULL item returns -1", "vec_push() with NULL item returns != -1");
    TEST(vec_length(vec) == 0, "Length remains 0 after failed push", "Length changed after failed push");
    
    // Test pushing to NULL vector (should fail)
    result = vec_push(NULL, (Row *)0x1000);
    TEST(result == -1, "vec_push() with NULL vector returns -1", "vec_push() with NULL vector returns != -1");
    
    vec_free(vec);
    printf("Test 3: vec_push() validation - Complete\n\n");
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
    TEST(vec_get(vec, 0) == NULL, "vec_get() on empty vector returns NULL", "vec_get() on empty vector returns != NULL");
    TEST(vec_get(vec, 100) == NULL, "vec_get() with invalid index returns NULL", "vec_get() with invalid index returns != NULL");
    
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

int main(void) {
    printf("=== Vec Unit Tests ===\n\n");
    
    test_vec_new_and_free();
    test_vec_zero_capacity();
    test_vec_push_validation();
    test_vec_edge_cases();
    test_vec_different_capacities();
    
    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    return (tests_run == tests_passed) ? 0 : 1;
}

