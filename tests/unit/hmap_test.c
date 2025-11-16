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
    TEST(map != NULL, "new() succeeds", "new() fails");
    TEST(hmap_size(map) == 0, "Size is 0", "Size is not 0");
    hmap_free(map);
    printf("Test 1: new() and free() - Complete\n\n");
}

// Test 2: Put and get
void test_hmap_put_and_get(void) {
    HMap *map = hmap_new(8);
    int val1 = 42, val2 = 100;
    
    hmap_put(map, "key1", &val1);
    hmap_put(map, "key2", &val2);
    TEST(hmap_size(map) == 2, "Size correct", "Size incorrect");
    TEST(*(int *)hmap_get(map, "key1") == 42, "key1 retrievable", "key1 fails");
    TEST(*(int *)hmap_get(map, "key2") == 100, "key2 retrievable", "key2 fails");
    
    hmap_free(map);
    printf("Test 2: put() and get() - Complete\n\n");
}

// Test 3: Update existing key
void test_hmap_update(void) {
    HMap *map = hmap_new(8);
    int val1 = 114514, val2 = 12345;
    
    hmap_put(map, "key1", &val1);
    hmap_put(map, "key1", &val2);
    TEST(*(int *)hmap_get(map, "key1") == 12345, "Updated value correct", "Updated value incorrect");
    TEST(hmap_size(map) == 1, "Size unchanged after update", "Size changed");
    
    hmap_free(map);
    printf("Test 3: update - Complete\n\n");
}

// Test 4: Remove
void test_hmap_remove(void) {
    HMap *map = hmap_new(8);
    int val = 666;
    
    hmap_put(map, "key1", &val);
    TEST(hmap_remove(map, "key1") == &val, "remove returns correct value", "remove fails");
    TEST(hmap_size(map) == 0, "Size correct after remove", "Size incorrect");
    TEST(hmap_get(map, "key1") == NULL, "Removed key returns NULL", "Removed key exists");
    
    hmap_free(map);
    printf("Test 4: remove() - Complete\n\n");
}

// Test 5: get_or_default
void test_hmap_get_or_default(void) {
    HMap *map = hmap_new(8);
    int val = 114514;
    int default_val = 999;
    
    TEST(hmap_get_or_default(map, "nonexistent", &default_val) == &default_val,
         "get_or_default returns default", "get_or_default fails");
    
    hmap_put(map, "key1", &val);
    TEST(hmap_get_or_default(map, "key1", &default_val) == &val,
         "get_or_default returns actual value", "get_or_default fails");
    TEST(hmap_get_or_default(NULL, "key", &default_val) == &default_val,
         "get_or_default with NULL map returns default", "get_or_default fails");
    
    hmap_free(map);
    printf("Test 5: get_or_default - Complete\n\n");
}

// Test 6: Collision handling
void test_hmap_collision(void) {
    HMap *map = hmap_new(1);  // force all keys into same bucket
    int val1 = 10, val2 = 20, val3 = 30;
    
    hmap_put(map, "key1", &val1);
    hmap_put(map, "key2", &val2);
    hmap_put(map, "key3", &val3);
    
    TEST(hmap_size(map) == 3, "Size correct with collisions", "Size incorrect");
    TEST(*(int *)hmap_get(map, "key1") == 10, "key1 retrievable", "key1 fails");
    TEST(*(int *)hmap_get(map, "key2") == 20, "key2 retrievable", "key2 fails");
    TEST(*(int *)hmap_get(map, "key3") == 30, "key3 retrievable", "key3 fails");
    
    hmap_remove(map, "key2");
    TEST(hmap_size(map) == 2, "Size correct after remove", "Size incorrect");
    TEST(hmap_get(map, "key1") != NULL, "key1 still accessible", "key1 fails");
    TEST(hmap_get(map, "key3") != NULL, "key3 still accessible", "key3 fails");
    TEST(hmap_get(map, "key2") == NULL, "key2 removed", "key2 still exists");
    
    hmap_free(map);
    printf("Test 6: collision handling - Complete\n\n");
}

// Test 7: put() return value
void test_hmap_put_return_value(void) {
    HMap *map = hmap_new(8);
    int val1 = 23, val2 = 7321, val3 = 312390;
    
    TEST(hmap_put(map, "key1", &val1) == NULL, "put new key returns NULL", "put fails");
    TEST(hmap_put(map, "key1", &val2) == &val1, "put update returns previous", "put fails");
    TEST(hmap_put(map, "key1", &val3) == &val2, "put second update returns previous", "put fails");
    TEST(*(int *)hmap_get(map, "key1") == 312390, "Final value correct", "Final value incorrect");
    
    hmap_free(map);
    printf("Test 7: put() return value - Complete\n\n");
}

// Test 8: Empty string key
void test_hmap_empty_string_key(void) {
    HMap *map = hmap_new(8);
    int val1 = 12345, val2 = 666666;
    
    hmap_put(map, "", &val1);
    TEST(hmap_size(map) == 1, "Size correct", "Size incorrect");
    TEST(*(int *)hmap_get(map, "") == 12345, "Empty key retrievable", "Empty key fails");
    
    TEST(hmap_put(map, "", &val2) == &val1, "Update empty key returns previous", "Update fails");
    TEST(*(int *)hmap_get(map, "") == 666666, "Updated value correct", "Updated value incorrect");
    
    hmap_free(map);
    printf("Test 8: empty string key - Complete\n\n");
}

// Test 9: Very long key
void test_hmap_long_key(void) {
    HMap *map = hmap_new(8);
    char long_key[1001];
    for (int i = 0; i < 1000; i++) {
        long_key[i] = 'a' + (i % 26);
    }
    long_key[1000] = '\0';
    
    int val = 42;
    hmap_put(map, long_key, &val);
    TEST(hmap_size(map) == 1, "Size correct", "Size incorrect");
    TEST(*(int *)hmap_get(map, long_key) == 42, "Long key retrievable", "Long key fails");
    
    hmap_free(map);
    printf("Test 9: very long key - Complete\n\n");
}

// Test 10: Capacity = 1 (all keys collide)
void test_hmap_capacity_one(void) {
    HMap *map = hmap_new(1);
    int val1 = 10, val2 = 20, val3 = 30, val4 = 40;
    
    hmap_put(map, "a", &val1);
    hmap_put(map, "b", &val2);
    hmap_put(map, "c", &val3);
    hmap_put(map, "d", &val4);
    
    TEST(hmap_size(map) == 4, "Size correct with capacity 1", "Size incorrect");
    TEST(*(int *)hmap_get(map, "a") == 10, "Key 'a' retrievable", "Key 'a' fails");
    TEST(*(int *)hmap_get(map, "b") == 20, "Key 'b' retrievable", "Key 'b' fails");
    TEST(*(int *)hmap_get(map, "c") == 30, "Key 'c' retrievable", "Key 'c' fails");
    TEST(*(int *)hmap_get(map, "d") == 40, "Key 'd' retrievable", "Key 'd' fails");
    
    hmap_free(map);
    printf("Test 10: capacity = 1 - Complete\n\n");
}

int main(void) {
    printf("=== HMap Unit Tests ===\n\n");
    
    test_hmap_new_and_free();
    test_hmap_put_and_get();
    test_hmap_update();
    test_hmap_remove();
    test_hmap_get_or_default();
    test_hmap_collision();
    test_hmap_put_return_value();
    test_hmap_empty_string_key();
    test_hmap_long_key();
    test_hmap_capacity_one();
    
    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    return (tests_run == tests_passed) ? 0 : 1;
}

