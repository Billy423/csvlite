/* Implements the group-by functionality for the CSVLite project.
 * This module groups CSV rows by a specified column index. It uses
 * the Lead Developer’s hash map (hmap) to track unique column values
 * and returns a vector containing one representative row per group.
 *
 * Vivek Patel, November 11, 2025, v1.0.0
 */

#include "group.h"
#include "vec.h"
#include "row.h"
#include "hmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Groups rows by a specific column index. Each unique column value
 * (from the specified index) is stored in a hash map, ensuring that
 * only one representative row per group is kept.
 *
 * PARAMETERS:
 *  rows, a Vec* containing Row* elements (input dataset)
 *  col_index, the index of the column to group by
 *
 * RETURNS:
 *  A new Vec* containing one representative Row* per unique group.
 *  Returns NULL if an invalid argument or allocation failure occurs.
 */
Vec* group_by_column(Vec* rows, int col_index) {
    if (!rows || col_index < 0) return NULL;

    Vec* grouped = vec_new(10);
    if (!grouped) return NULL;

    HMap* seen = hmap_new(64);
    if (!seen) return NULL;

    for (size_t i = 0; i < vec_length(rows); i++) {
        Row* row = vec_get(rows, i);
        // provided by row.c
        const char* key = row_get_cell(row, col_index); 

        if (!hmap_contains(seen, key)) {
            // first occurrence of this key, store representative row
            hmap_put(seen, key, row);
            vec_push(grouped, row);
        }
    }

    hmap_free(seen);
    return grouped;
}

/* Placeholder for aggregation functions (e.g., count, sum, average).
 * Currently unimplemented — to be added in Increment 2.
 *
 * PARAMETERS:
 *  grouped_rows - a Vec* containing grouped Row* entries
 *
 * RETURNS:
 *  None
 */
void group_aggregate(Vec* grouped_rows) {
    // suppress unused variable warning
    (void)grouped_rows;
}
