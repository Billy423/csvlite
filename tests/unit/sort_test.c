/* Unit tests for sort.c
 * Tests basic single-column sorting using the sort_rows() function.
 * Ensures that rows are sorted correctly in ascending order based
 * on the specified column index.
 *
 * Vivek Patel, November 17, 2025, v1.0.0
 */

#include "sort.h"
#include "vec.h"
#include "row.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Tests that sort_rows() correctly sorts values in a numeric column.
 * Rows contain exam scores in column index 1.
 * The sort should reorder them from lowest to highest.
 *
 * RETURNS:
 *   None (asserts used for validation)
 */
void test_sort_numeric_column() {
    Vec *rows = vec_new(4);

    vec_push(rows, row_from_string("Alice,92"));
    vec_push(rows, row_from_string("Bob,80"));
    vec_push(rows, row_from_string("Carol,88"));
    vec_push(rows, row_from_string("Dave,75"));

    // Sort by numeric column 1 (scores)
    sort_rows(rows, 1);

    // Verify order: 75, 80, 88, 92
    Row *r0 = vec_get(rows, 0);
    Row *r1 = vec_get(rows, 1);
    Row *r2 = vec_get(rows, 2);
    Row *r3 = vec_get(rows, 3);

    assert(atoi(row_get_cell(r0, 1)) == 75);
    assert(atoi(row_get_cell(r1, 1)) == 80);
    assert(atoi(row_get_cell(r2, 1)) == 88);
    assert(atoi(row_get_cell(r3, 1)) == 92);

    printf("sort_rows(): numeric column sorted correctly\n");

    vec_free(rows);
}

/* Tests sorting on a text column.
 *
 * Sort rows alphabetically by name in column index 0.
 *
 * RETURNS:
 *   None
 */
void test_sort_text_column() {
    Vec *rows = vec_new(4);

    vec_push(rows, row_from_string("Charlie,3"));
    vec_push(rows, row_from_string("Alice,1"));
    vec_push(rows, row_from_string("Bob,2"));
    vec_push(rows, row_from_string("David,4"));

    sort_rows(rows, 0);

    assert(strcmp(row_get_cell(vec_get(rows, 0), 0), "Alice") == 0);
    assert(strcmp(row_get_cell(vec_get(rows, 1), 0), "Bob") == 0);
    assert(strcmp(row_get_cell(vec_get(rows, 2), 0), "Charlie") == 0);
    assert(strcmp(row_get_cell(vec_get(rows, 3), 0), "David") == 0);

    printf("sort_rows(): text column sorted correctly\n");

    vec_free(rows);
}

/* Entry point for the sort test suite.
 *
 * EXIT CODES:
 *   EXIT_SUCCESS on all pass
 *   EXIT_FAILURE if any assertion fails
 */
int main() {
    test_sort_numeric_column();
    test_sort_text_column();

    printf("sort_test: PASS\n");
    return EXIT_SUCCESS;
}
