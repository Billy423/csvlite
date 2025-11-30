/*
* Unit tests for row data structure
*
* AUTHOR: Billy Wu
* DATE: December 1, 2025
* VERSION: v2.0.0
*/

#include "../../include/row.h"
#include <stdio.h>
#include <stdlib.h>
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

// Test 1: Create and free
void test_row_new_and_free(void) {
     Row *row = row_new(3);
     TEST(row != NULL, "row_new() returns non-NULL", "row_new() returns NULL");
     TEST(row_num_cells(row) == 3, "New row has 3 cells", "New row has != 3 cells");
     row_free(row);
     printf("Test 1: row_new() and row_free() - Complete\n\n");
}

// Test 2: Set and get cells
void test_row_set_and_get(void) {
     Row *row = row_new(3);
     TEST(row != NULL, "row_new() succeeds", "row_new() fails");
     
     TEST(row_set_cell(row, 0, "value1") == 0, "row_set_cell() succeeds", "row_set_cell() fails");
     TEST(row_set_cell(row, 1, "value2") == 0, "row_set_cell() succeeds", "row_set_cell() fails");
     TEST(row_set_cell(row, 2, "value3") == 0, "row_set_cell() succeeds", "row_set_cell() fails");

     const char *val1 = row_get_cell(row, 0);
     TEST(val1 != NULL && strcmp(val1, "value1") == 0,
          "row_get_cell() returns correct value", 
          "row_get_cell() returns incorrect value"
     );

     const char *val2 = row_get_cell(row, 1);
     TEST(val2 != NULL && strcmp(val2, "value2") == 0,
          "row_get_cell() returns correct value",
          "row_get_cell() returns incorrect value"
     );

     const char *val3 = row_get_cell(row, 2);
     TEST(val3 != NULL && strcmp(val3, "value3") == 0,
          "row_get_cell() returns correct value",
          "row_get_cell() returns incorrect value"
     );
     row_free(row);
     printf("Test 2: row_set_cell() and row_get_cell() - Complete\n\n");
}

// Test 3: Update cell
void test_row_update(void) {
     Row *row = row_new(2);
     TEST(row != NULL, "row_new() succeeds", "row_new() fails");
     
     row_set_cell(row, 0, "old");
     row_set_cell(row, 0, "new");  // Update
     
     const char *val = row_get_cell(row, 0);
     TEST(val != NULL && strcmp(val, "new") == 0,
          "Updated value is correct", 
          "Updated value is incorrect"
     );
     
     row_free(row);
     printf("Test 3: row_set_cell() update - Complete\n\n");
}

// Test 4: Invalid index handling
void test_row_invalid_index(void) {
     Row *row = row_new(2);
     TEST(row != NULL, "row_new() succeeds", "row_new() fails");
     
     TEST(row_set_cell(row, -1, "test") == -1,
          "row_set_cell() with negative index returns -1",
          "row_set_cell() with negative index returns != -1"
     );

     TEST(row_set_cell(row, 2, "test") == -1,
          "row_set_cell() with out-of-bounds index returns -1",
          "row_set_cell() with out-of-bounds index returns != -1"
     );
     
     TEST(row_get_cell(row, -1) == NULL,
          "row_get_cell() with negative index returns NULL",
          "row_get_cell() with negative index returns != NULL"
     );

     TEST(row_get_cell(row, 2) == NULL,
          "row_get_cell() with out-of-bounds index returns NULL",
          "row_get_cell() with out-of-bounds index returns != NULL"
     );
     
     row_free(row);
     printf("Test 4: Invalid index handling - Complete\n\n");
}

// Test 5: NULL handling
void test_row_null_handling(void) {
     TEST(row_new(0) == NULL, "row_new(0) returns NULL", "row_new(0) returns != NULL");
     TEST(row_new(-1) == NULL, "row_new(-1) returns NULL", "row_new(-1) returns != NULL");
     
     TEST(row_set_cell(NULL, 0, "test") == -1,
          "row_set_cell(NULL, ...) returns -1",
          "row_set_cell(NULL, ...) returns != -1"
     );

     TEST(row_get_cell(NULL, 0) == NULL,
          "row_get_cell(NULL, ...) returns NULL",
          "row_get_cell(NULL, ...) returns != NULL"
     );

     TEST(row_num_cells(NULL) == 0,
          "row_num_cells(NULL) returns 0",
          "row_num_cells(NULL) returns != 0"
     );
     
     row_free(NULL);  // safe to pass NULL
     
     Row *row = row_new(2);
     TEST(row != NULL, "row_new() succeeds", "row_new() fails");
     
     // Setting NULL value should be allowed (clears cell)
     TEST(row_set_cell(row, 0, NULL) == 0,
          "row_set_cell() with NULL value succeeds",
          "row_set_cell() with NULL value fails"
     );
     TEST(row_get_cell(row, 0) == NULL,
          "row_get_cell() returns NULL for unset cell",
          "row_get_cell() returns != NULL for unset cell"
     );
     
     row_free(row);
     printf("Test 5: NULL handling - Complete\n\n");
}

int main(void) {
     printf("=== Row Unit Tests ===\n\n");
     
     test_row_new_and_free();
     test_row_set_and_get();
     test_row_update();
     test_row_invalid_index();
     test_row_null_handling();
     
     printf("=== Test Summary ===\n");
     printf("Tests run: %d\n", tests_run);
     printf("Tests passed: %d\n", tests_passed);
     printf("Tests failed: %d\n", tests_run - tests_passed);
     
     return (tests_run == tests_passed) ? 0 : 1;
}

