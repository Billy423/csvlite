/* Basic unit test for the group-by module. 
 * 
 * Note: 
 * Coverage is not able to go above 60% since hmap is unable to be NULL
 * free_group_result() will not be covered since it also is only called when hmap is NULL

 * AUTHOR: Vivek Patel
 * DATE: November 11, 2025
 * VERSION: v2.0.0
 */

#include "group.h"
#include "vec.h"
#include "row.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * Helper, creates a Row* from a single CSV string.
 *
 * PARAMETERS:
 *   csv — a null-terminated string containing comma-separated values
 *
 * RETURNS:
 *   Pointer to a newly allocated Row containing parsed cell values, or NULL if allocation fails.
 */
static Row* make_row(const char *csv) {
    int cols = 1;
    for (const char *p = csv; *p; p++)
        if (*p == ',') cols++;

    Row *r = row_new(cols);

    char buffer[256];
    int col = 0;
    const char *start = csv;
    const char *c = csv;

    while (*c) {
        if (*c == ',') {
            int len = c - start;
            memcpy(buffer, start, len);
            buffer[len] = '\0';
            row_set_cell(r, col++, buffer);
            start = c + 1;
        }
        c++;
    }
    row_set_cell(r, col, start);

    return r;
}

/* Tests whether group_by_column() correctly reduces duplicate entries.
 * Verifies that grouping by the first column produces unique groups.
 * Prints success message to stdout on test pass.
 *
 * PARAMS:
 * None
 * 
 * RETURNS:
 *  None
 */
void test_group_by_column_unique() {
    Vec* rows = vec_new(4);
    vec_push(rows, make_row("CS,John,85"));
    vec_push(rows, make_row("CS,Alice,92"));
    vec_push(rows, make_row("SE,Bob,88"));
    vec_push(rows, make_row("SE,Emma,91"));

    Vec* grouped = group_by_column(rows, 0);

    assert(grouped != NULL);
    assert(vec_length(grouped) == 2);
    
    // Clean
    for (size_t i = 0; i < vec_length(rows); i++) {
        row_free(vec_get(rows, i));
    }
    vec_free(rows);
    vec_free(grouped);

    printf("Test 1: group_by_column() unique groups - Complete\n\n");
}

// Test 2: rows is empty, return NULL
void test_group_empty_vector() {
    Vec *rows = vec_new(0);

    Vec *grouped = group_by_column(rows, 0);
    assert(grouped == NULL);

    vec_free(rows);
    printf("Test 2: empty vector handled correctly\n\n");
}

// Test 3: invalid column index, return NULL
void test_group_invalid_column() {
    Vec *rows = vec_new(1);
    vec_push(rows, make_row("A,B,C"));

    Vec *grouped = group_by_column(rows, 10);
    assert(grouped == NULL);

    row_free(vec_get(rows, 0));
    vec_free(rows);
    printf("Test 3: invalid column handled correctly\n\n");
}

// Test 4: NULL row inside vector, skip safely
void test_group_null_row_inside() {
    Vec *rows = vec_new(3);
    vec_push(rows, make_row("CS,John"));
    vec_push(rows, NULL);
    vec_push(rows, make_row("CS,Alice"));

    Vec *grouped = group_by_column(rows, 0);

    assert(grouped != NULL);
    assert(vec_length(grouped) == 1);

    // Clean
    for (size_t i = 0; i < vec_length(rows); i++) {
        Row *r = vec_get(rows, i);
        if (r) row_free(r);
    }
    vec_free(rows);
    vec_free(grouped);

    printf("Test 4: NULL row inside handled correctly\n\n");
}

// Test 5: First row is NULL
void test_group_null_first_row() {
    Vec *rows = vec_new(2);

    // Vec_push(NULL) fails, row is NOT added
    assert(vec_push(rows, NULL) == -1);

    // Only valid row added
    vec_push(rows, make_row("A,B"));

    // Grouping should succeed with 1 row
    Vec *grouped = group_by_column(rows, 0);
    assert(grouped != NULL);
    assert(vec_length(grouped) == 1);

    // Clean
    row_free(vec_get(rows, 0));
    vec_free(rows);
    vec_free(grouped);

    printf("Test 5: NULL first row handled\n\n");
}

// Test 6: Internal NULL row
void test_group_null_middle_row() {
    Vec *rows = vec_new(3);
    vec_push(rows, make_row("CS,John"));
    vec_push(rows, NULL);
    vec_push(rows, make_row("CS,Alice"));

    Vec *grouped = group_by_column(rows, 0);

    assert(grouped != NULL);
    assert(vec_length(grouped) == 1);

    // Clean
    for (size_t i = 0; i < vec_length(rows); i++) {
        Row *r = vec_get(rows, i);
        if (r) row_free(r);
    }
    vec_free(rows);
    vec_free(grouped);

    printf("Test 6: Internal NULL row handled\n\n");
}

// Test 7: Invalid column index
void test_group_invalid_col_index() {
    Vec *rows = vec_new(1);
    vec_push(rows, make_row("A,B"));

    Vec *grouped = group_by_column(rows, 5);

    assert(grouped == NULL);

    row_free(vec_get(rows, 0));
    vec_free(rows);

    printf("Test 7: Invalid column index handled\n\n");
}

// Test 8: Group with multiple unique keys
void test_group_two_keys() {
    Vec *rows = vec_new(3);
    vec_push(rows, make_row("A,1"));
    vec_push(rows, make_row("B,2"));
    vec_push(rows, make_row("C,3"));

    Vec *grouped = group_by_column(rows, 0);
    assert(grouped != NULL);
    assert(vec_length(grouped) == 3);

    for (size_t i = 0; i < vec_length(rows); i++) {
        row_free(vec_get(rows, i));
    }

    vec_free(rows);
    vec_free(grouped);

    printf("Test 8: Multiple unique keys handled correctly\n\n");
}

/* Entry point for the test program.
 * Runs unit tests for the group module.
 * 
 * EXIT CODES:
 *  EXIT_SUCCESS, if all assertions pass
 *  EXIT_FAILURE, if any assertion fails
 */
int main() {
    printf("=== Group Unit Tests ===\n\n");
    
    test_group_by_column_unique();
    test_group_empty_vector();
    test_group_invalid_column();
    test_group_null_row_inside();
    test_group_null_first_row();
    test_group_null_middle_row();
    test_group_invalid_col_index();
    test_group_two_keys();
    
    printf("=== Test Summary ===\n");
    printf("Tests run: 8\n");
    printf("Tests passed: 8\n");
    printf("Tests failed: 0\n");
    
    return EXIT_SUCCESS;
}