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

// Test 1: Parse column indices from string
void test_select_parse_indices(void) {
    HMap *hdr = hmap_new(8);
    hmap_put(hdr, "name", (void*)(long)(0 + 1));
    hmap_put(hdr, "age", (void*)(long)(1 + 1));
    hmap_put(hdr, "gpa", (void*)(long)(2 + 1));

    int *idx = NULL;
    int nidx = 0;
    
    // Test parsing "name,gpa"
    int result = select_parse_indices("name,gpa", hdr, 3, &idx, &nidx);
    TEST(result == 0, "select_parse_indices() succeeds", "select_parse_indices() fails");
    TEST(nidx == 2, "Correct number of indices parsed", "Incorrect number of indices");
    TEST(idx != NULL, "Indices array allocated", "Indices array not allocated");
    
    if (idx) {
        // Check indices (accounting for +1 offset in hmap)
        TEST(idx[0] == 0, "First index is 0 (name)", "First index incorrect");
        TEST(idx[1] == 2, "Second index is 2 (gpa)", "Second index incorrect");
        free(idx);
    }
    
    hmap_free(hdr);
    printf("Test 1: select_parse_indices() - Complete\n\n");
}

// Test 2: Project rows to selected columns
void test_select_project_rows(void) {
    // Create header map
    HMap *hdr = hmap_new(8);
    hmap_put(hdr, "name", (void*)(long)(0 + 1));
    hmap_put(hdr, "age", (void*)(long)(1 + 1));
    hmap_put(hdr, "gpa", (void*)(long)(2 + 1));

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
    select_parse_indices("name,gpa", hdr, 3, &idx, &nidx);

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
            row_free(vec_get(proj, i));
        }
        vec_free(proj);
    }
    
    int nrows = (int)vec_length(rows);
    for (int i = 0; i < nrows; i++) {
        row_free(vec_get(rows, i));
    }
    vec_free(rows);
    hmap_free(hdr);
    free(idx);
    
    printf("Test 2: select_project_rows() - Complete\n\n");
}

// Test 3: NULL handling
void test_select_null_handling(void) {
    Vec *proj = select_project_rows(NULL, NULL, 0);
    TEST(proj == NULL, "select_project_rows() with NULL rows returns NULL", "select_project_rows() with NULL rows should return NULL");
    
    printf("Test 3: NULL handling - Complete\n\n");
}

int main(void) {
    printf("=== Select Unit Tests ===\n\n");
    
    test_select_parse_indices();
    test_select_project_rows();
    test_select_null_handling();
    
    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    return (tests_run == tests_passed) ? 0 : 1;
}

