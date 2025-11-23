/* Basic unit test for the group-by module. Ensures that grouping by
 * a column correctly produces unique groups from duplicate input rows.
 *
 * Vivek Patel, November 11, 2025, v0.0.2
 */

#include "group.h"
#include "vec.h"
#include "row.h"
#include <assert.h>
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

    printf("group_by_column(): PASS, correctly grouped to 2 groups\n");
}

/* Entry point for the test program.
 * Runs unit tests for the group module.
 * 
 * EXIT CODES:
 *  EXIT_SUCCESS, if all assertions pass
 *  EXIT_FAILURE, if any assertion fails
 */
int main() {
    test_group_by_column_unique();
    return EXIT_SUCCESS;
}