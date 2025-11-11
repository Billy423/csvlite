#include "../../include/hmap.h"
#include <stdio.h>
#include <stdlib.h>

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

// Test 1: Create and free
void test_hmap_new_and_free(void) {
    HMap *map = hmap_new(16);
    TEST(map != NULL, "hmap_new() returns non-NULL", "hmap_new() returns NULL");
    TEST(hmap_size(map) == 0, "New map has size 0", "New map has size != 0");
    hmap_free(map);
    printf("Test 1: hmap_new() and hmap_free() - Complete\n\n");
}

// Test 2: Put and get
void test_hmap_put_and_get(void) {
    HMap *map = hmap_new(8);
    TEST(map != NULL, "hmap_new() succeeds", "hmap_new() fails");
    
    int val1 = 42;
    int val2 = 100;
    
    // Put keys
    hmap_put(map, "key1", &val1);
    hmap_put(map, "key2", &val2);
    TEST(hmap_size(map) == 2, "Size is 2 after two puts", "Size is != 2");
    
    // Get values
    int *retrieved = (int *)hmap_get(map, "key1");
    TEST(retrieved != NULL && *retrieved == 42, "hmap_get() returns correct value for key1", 
         "hmap_get() returns incorrect value for key1");
    
    retrieved = (int *)hmap_get(map, "key2");
    TEST(retrieved != NULL && *retrieved == 100, "hmap_get() returns correct value for key2",
         "hmap_get() returns incorrect value for key2");
    
    hmap_free(map);
    printf("Test 2: hmap_put() and hmap_get() - Complete\n\n");
}

// Test 3: Update existing key
void test_hmap_update(void) {
    HMap *map = hmap_new(8);
    TEST(map != NULL, "hmap_new() succeeds", "hmap_new() fails");
    
    int val1 = 42;
    int val2 = 100;
    
    hmap_put(map, "key1", &val1);
    hmap_put(map, "key1", &val2);  // Update
    
    int *retrieved = (int *)hmap_get(map, "key1");
    TEST(retrieved != NULL && *retrieved == 100, "Updated value is correct", 
         "Updated value is incorrect");
    TEST(hmap_size(map) == 1, "Size remains 1 after update", "Size changed after update");
    
    hmap_free(map);
    printf("Test 3: hmap_put() update - Complete\n\n");
}

// Test 4: Remove
void test_hmap_remove(void) {
    HMap *map = hmap_new(8);
    TEST(map != NULL, "hmap_new() succeeds", "hmap_new() fails");
    
    int val = 42;
    hmap_put(map, "key1", &val);
    TEST(hmap_size(map) == 1, "Size is 1 after put", "Size is != 1");
    
    void *removed = hmap_remove(map, "key1");
    TEST(removed == &val, "hmap_remove() returns correct value", 
         "hmap_remove() returns incorrect value");
    TEST(hmap_size(map) == 0, "Size is 0 after remove", "Size is != 0 after remove");
    
    void *result = hmap_get(map, "key1");
    TEST(result == NULL, "Removed key returns NULL", "Removed key returns != NULL");
    
    hmap_free(map);
    printf("Test 4: hmap_remove() - Complete\n\n");
}

int main(void) {
    printf("=== HMap Unit Tests ===\n\n");
    
    test_hmap_new_and_free();
    test_hmap_put_and_get();
    test_hmap_update();
    test_hmap_remove();
    
    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    return (tests_run == tests_passed) ? 0 : 1;
}

