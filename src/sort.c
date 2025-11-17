/* Implements basic sorting for CSV rows using qsort().
 * Sorting is based on a specific column index. 
 * The comparison function uses strcmp() on the cell strings.
 *
 * Vivek Patel, November 17, 2025, v1.0.0
 */

#include "sort.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// global temporary column index for comparator (qsort limitation)
static int g_sort_col = 0;

/* qsort comparator for Row* items.
 *
 * PARAMETERS:
 *   a, b - pointers to Row*
 *
 * RETURNS:
 *   negative if a < b
 *   zero     if a == b
 *   positive if a > b
 */
static int row_compare(const void *a, const void *b) {
    const Row *ra = *(const Row **)a;
    const Row *rb = *(const Row **)b;

    const char *ca = row_get_cell(ra, g_sort_col);
    const char *cb = row_get_cell(rb, g_sort_col);

    if (!ca && !cb) return 0;
    if (!ca) return -1;
    if (!cb) return 1;

    return strcmp(ca, cb);
}

/* Sorts rows in-place by column.
 * 
 * Returns 0 on success, -1 on failure.
 */
int sort_by_column(Vec *rows, int col_index) {
    if (!rows || col_index < 0) {
        return -1;
    }

    g_sort_col = col_index;

    qsort(rows->data, rows->length, sizeof(Row *), row_compare);

    return 0;
}