/*
* Tests to verify select clauses
*
*
* AUTHOR: Nadeem Mohamed
* DATE: November 23, 2025
* VERSION: v0.0.1
*/

#include "../../include/select.h"
#include "../../include/row.h"
#include "../../include/vec.h"
#include "../../include/hmap.h"
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

/* ===================== ORIGINAL TESTS (slightly cleaned) ===================== */

// Test 1: Parse column indices from string (by name)
void test_select_parse_indices_by_name(void) {
    HMap *hdr = hmap_new(8);
    hmap_put(hdr, "name", (void*)(long)(0 + 1));
    hmap_put(hdr, "age",  (void*)(long)(1 + 1));
    hmap_put(hdr, "gpa",  (void*)(long)(2 + 1));

    int *idx = NULL;
    int nidx = 0;

    int result = select_parse_indices("name,gpa", hdr, 3, &idx, &nidx);
    TEST(result == 0, "select_parse_indices() by name succeeds", "select_parse_indices() by name fails");
    TEST(nidx == 2, "Correct number of indices parsed", "Incorrect number of indices");
    TEST(idx != NULL, "Indices array allocated", "Indices array not allocated");

    if (idx) {
        TEST(idx[0] == 0, "First index is 0 (name)", "First index incorrect");
        TEST(idx[1] == 2, "Second index is 2 (gpa)", "Second index incorrect");
        free(idx);
    }

    hmap_free(hdr);
    printf("Test: select_parse_indices_by_name - Complete\n\n");
}

// Test 2: Project rows to selected columns
void test_select_project_rows_basic(void) {
    // Create header map
    HMap *hdr = hmap_new(8);
    hmap_put(hdr, "name", (void*)(long)(0 + 1));
    hmap_put(hdr, "age",  (void*)(long)(1 + 1));
    hmap_put(hdr, "gpa",  (void*)(long)(2 + 1));

    // Create sample rows
    Vec *rows = vec_new(2);
    Row *r1 = row_new(3);
    row_set_cell(r1, 0, "Alice");
    row_set_cell(r1, 1, "20");
    row_set_cell(r1, 2, "3.9");
    vec_push(rows, r1);

    Row *r2 = row_new(3);
    row_set_cell(r2, 0, "Bob");
    row_set_cell(r2, 1, "19");
    row_set_cell(r2, 2, "3.5");
    vec_push(rows, r2);

    // Parse indices for "name,gpa"
    int *idx = NULL;
    int nidx = 0;
    int parse_result = select_parse_indices("name,gpa", hdr, 3, &idx, &nidx);
    TEST(parse_result == 0, "select_parse_indices() for projection succeeds", "select_parse_indices() for projection fails");

    // Project rows
    Vec *proj = select_project_rows(rows, idx, nidx);
    TEST(proj != NULL, "select_project_rows() succeeds", "select_project_rows() fails");
    TEST(vec_length(proj) == 2, "Projected vector has 2 rows", "Projected vector has incorrect length");

    // Verify first row: Alice,3.9
    if (proj && vec_length(proj) >= 1) {
        Row *proj_r1 = vec_get(proj, 0);
        TEST(proj_r1 != NULL, "First projected row exists", "First projected row is NULL");
        if (proj_r1) {
            TEST(row_num_cells(proj_r1) == 2, "First row has 2 cells", "First row has incorrect cell count");
            const char *cell0 = row_get_cell(proj_r1, 0);
            const char *cell1 = row_get_cell(proj_r1, 1);
            TEST(cell0 != NULL && strcmp(cell0, "Alice") == 0, "First row first cell is 'Alice'", "First row first cell incorrect");
            TEST(cell1 != NULL && strcmp(cell1, "3.9") == 0, "First row second cell is '3.9'", "First row second cell incorrect");
        }
    }

    // Verify second row: Bob,3.5
    if (proj && vec_length(proj) >= 2) {
        Row *proj_r2 = vec_get(proj, 1);
        TEST(proj_r2 != NULL, "Second projected row exists", "Second projected row is NULL");
        if (proj_r2) {
            TEST(row_num_cells(proj_r2) == 2, "Second row has 2 cells", "Second row has incorrect cell count");
            const char *cell0 = row_get_cell(proj_r2, 0);
            const char *cell1 = row_get_cell(proj_r2, 1);
            TEST(cell0 != NULL && strcmp(cell0, "Bob") == 0, "Second row first cell is 'Bob'", "Second row first cell incorrect");
            TEST(cell1 != NULL && strcmp(cell1, "3.5") == 0, "Second row second cell is '3.5'", "Second row second cell incorrect");
        }
    }

    // Cleanup
    if (proj) {
        int nproj = (int)vec_length(proj);
        for (int i = 0; i < nproj; i++) {
            Row *r = vec_get(proj, i);
            row_free(r);
        }
        vec_free(proj);
    }

    int nrows = (int)vec_length(rows);
    for (int i = 0; i < nrows; i++) {
        Row *r = vec_get(rows, i);
        row_free(r);
    }
    vec_free(rows);
    hmap_free(hdr);
    free(idx);

    printf("Test: select_project_rows_basic - Complete\n\n");
}

// Test 3: NULL handling (rows)
void test_select_null_rows_handling(void) {
    Vec *proj = select_project_rows(NULL, NULL, 0);
    TEST(proj == NULL, "select_project_rows() with NULL rows returns NULL", "select_project_rows() with NULL rows should return NULL");

    printf("Test: select_null_rows_handling - Complete\n\n");
}

/* ===================== NEW TESTS FOR BETTER COVERAGE ===================== */

// Test 4: parse_indices with NULL columns_spec
void test_select_parse_indices_null_spec(void) {
    int *idx = NULL;
    int nidx = 123; // just to see if it changes

    int result = select_parse_indices(NULL, NULL, 3, &idx, &nidx);
    TEST(result == -1, "select_parse_indices() with NULL spec returns -1", "select_parse_indices() with NULL spec should return -1");
    TEST(idx == NULL, "out_indices remains NULL on failure", "out_indices should remain NULL on failure");

    printf("Test: select_parse_indices_null_spec - Complete\n\n");
}

// Test 5: parse_indices with numeric indices and whitespace " 0 , 2 "
void test_select_parse_indices_numeric_and_whitespace(void) {
    int *idx = NULL;
    int nidx = 0;

    int result = select_parse_indices(" 0 , 2 ", NULL, 3, &idx, &nidx);
    TEST(result == 0, "select_parse_indices() with numeric indices succeeds", "select_parse_indices() with numeric indices fails");
    TEST(nidx == 2, "Correct count for numeric indices", "Incorrect count for numeric indices");
    TEST(idx != NULL, "Indices array allocated for numeric indices", "Indices array not allocated for numeric indices");

    if (idx) {
        TEST(idx[0] == 0, "First numeric index is 0", "First numeric index incorrect");
        TEST(idx[1] == 2, "Second numeric index is 2", "Second numeric index incorrect");
        free(idx);
    }

    printf("Test: select_parse_indices_numeric_and_whitespace - Complete\n\n");
}

// Test 6: parse_indices with numeric index out of range
void test_select_parse_indices_numeric_out_of_range(void) {
    int *idx = NULL;
    int nidx = 0;

    int result = select_parse_indices("0,10", NULL, 3, &idx, &nidx);
    TEST(result == -1, "select_parse_indices() fails for out-of-range index", "select_parse_indices() should fail for out-of-range index");
    TEST(idx == NULL, "out_indices remains NULL when parsing fails", "out_indices should be NULL when parsing fails");

    // nothing to free since idx should be NULL
    printf("Test: select_parse_indices_numeric_out_of_range - Complete\n\n");
}

// Test 7: parse_indices by name with extra spaces and missing column
void test_select_parse_indices_name_errors(void) {
    // HMap with only some of the columns
    HMap *hdr = hmap_new(8);
    hmap_put(hdr, "name", (void*)(long)(0 + 1));
    hmap_put(hdr, "age",  (void*)(long)(1 + 1));
    // intentionally NOT adding "gpa"

    int *idx = NULL;
    int nidx = 0;

    // First: a success case with whitespace: " name , age "
    int result_ok = select_parse_indices(" name , age ", hdr, 3, &idx, &nidx);
    TEST(result_ok == 0, "select_parse_indices() trims whitespace around names", "select_parse_indices() failed with whitespace around names");
    TEST(nidx == 2, "Whitespace-trimmed names produce 2 indices", "Whitespace-trimmed names should produce 2 indices");
    if (idx) {
        TEST(idx[0] == 0, "Trimmed first name maps to index 0", "Trimmed first name index incorrect");
        TEST(idx[1] == 1, "Trimmed second name maps to index 1", "Trimmed second name index incorrect");
        free(idx);
        idx = NULL;
    }

    // Second: error case for unknown name "gpa"
    int result_err = select_parse_indices("gpa", hdr, 3, &idx, &nidx);
    TEST(result_err == -1, "select_parse_indices() fails on unknown column name", "select_parse_indices() should fail on unknown column name");
    TEST(idx == NULL, "out_indices remains NULL on unknown name error", "out_indices should remain NULL on unknown name error");

    hmap_free(hdr);
    printf("Test: select_parse_indices_name_errors - Complete\n\n");
}

// Test 8: parse_indices name lookup but NULL hmap (should fail)
void test_select_parse_indices_name_without_hmap(void) {
    int *idx = NULL;
    int nidx = 0;

    int result = select_parse_indices("name", NULL, 3, &idx, &nidx);
    TEST(result == -1, "select_parse_indices() fails when name_to_index is NULL", "select_parse_indices() should fail when name_to_index is NULL");
    TEST(idx == NULL, "out_indices remains NULL when hmap is missing", "out_indices should remain NULL when hmap is missing");

    printf("Test: select_parse_indices_name_without_hmap - Complete\n\n");
}

// Test 9: select_project_rows with NULL indices
void test_select_project_rows_null_indices(void) {
    Vec *rows = vec_new(1);
    Row *r = row_new(2);
    row_set_cell(r, 0, "X");
    row_set_cell(r, 1, "Y");
    vec_push(rows, r);

    Vec *proj = select_project_rows(rows, NULL, 1);
    TEST(proj == NULL, "select_project_rows() returns NULL when indices is NULL", "select_project_rows() should return NULL when indices is NULL");

    // cleanup
    int nrows = (int)vec_length(rows);
    for (int i = 0; i < nrows; i++) {
        Row *rr = vec_get(rows, i);
        row_free(rr);
    }
    vec_free(rows);

    printf("Test: select_project_rows_null_indices - Complete\n\n");
}

// Test 10: select_project_rows with n_indices <= 0
void test_select_project_rows_zero_indices(void) {
    Vec *rows = vec_new(1);
    Row *r = row_new(2);
    row_set_cell(r, 0, "X");
    row_set_cell(r, 1, "Y");
    vec_push(rows, r);

    int indices[1] = {0};

    Vec *proj = select_project_rows(rows, indices, 0);
    TEST(proj == NULL, "select_project_rows() returns NULL when n_indices <= 0", "select_project_rows() should return NULL when n_indices <= 0");

    // cleanup
    int nrows = (int)vec_length(rows);
    for (int i = 0; i < nrows; i++) {
        Row *rr = vec_get(rows, i);
        row_free(rr);
    }
    vec_free(rows);

    printf("Test: select_project_rows_zero_indices - Complete\n\n");
}

/* ===================== MAIN ===================== */

int main(void) {
    printf("=== Select Unit Tests ===\n\n");

    // Original tests
    test_select_parse_indices_by_name();
    test_select_project_rows_basic();
    test_select_null_rows_handling();

    // New tests for coverage
    test_select_parse_indices_null_spec();
    test_select_parse_indices_numeric_and_whitespace();
    test_select_parse_indices_numeric_out_of_range();
    test_select_parse_indices_name_errors();
    test_select_parse_indices_name_without_hmap();
    test_select_project_rows_null_indices();
    test_select_project_rows_zero_indices();

    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);

    return (tests_run == tests_passed) ? 0 : 1;
}
