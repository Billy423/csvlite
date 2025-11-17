/* Basic unit test for the group-by module. Ensures that grouping by
 * a column correctly produces unique groups from duplicate input rows.
 *
 * Vivek Patel, November 11, 2025, v1.0.0
 */

#include "group.h"
#include "vec.h"
#include "row.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Tests whether group_by_column() correctly reduces duplicate entries.
 * Verifies that grouping by the first column produces unique groups.
 * Prints success message to stdout on test pass.
 *
 * RETURNS:
 *  None
 */
void test_group_by_column_unique() {
    Vec* rows = vec_new(4);
    vec_push(rows, row_from_string("CS,John,85"));
    vec_push(rows, row_from_string("CS,Alice,92"));
    vec_push(rows, row_from_string("SE,Bob,88"));
    vec_push(rows, row_from_string("SE,Emma,91"));

    Vec* grouped = group_by_column(rows, 0);
    assert(grouped != NULL);
    assert(vec_length(grouped) == 2);

    printf("group_by_column(): produced 2 unique groups successfully\n");
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