/* Implements basic sorting for CSV rows using qsort().
 * Sorting is based on a specific column index. 
 * The comparison function uses strcmp() on the cell strings.
 *
 * Vivek Patel, November 17, 2025, v0.0.2
 */


#include "../include/sort.h"
#include "../include/vec.h"
#include "../include/row.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// global temporary column index for comparator
static int g_sort_col = 0;

/* Checks whether a C-string represents a valid integer literal.
 * Accepts optional leading '+' or '-' sign followed by digits.)
 * 
 * PARAMETERS:
 *   s  - input string
 *
 * RETURNS:
 *   1  if s is a valid integer 
 *   0  otherwise
*/
static int is_int_str(const char *s) {
    if (!s) return 0;
    if (*s == '+' || *s == '-') s++;
    // must have at least 1 digit
    if (!*s) return 0;
    
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

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
static int rowptr_compare(const void *a, const void *b) {
    
    // Unwrap Row* from qsort
    const Row *ra = *(Row * const *)a;
    const Row *rb = *(Row * const *)b;

    if (!ra || !rb)
        return 0;

    // Get target column values
    const char *sa = row_get_cell(ra, g_sort_col);
    const char *sb = row_get_cell(rb, g_sort_col);

    // Handle missing cells
    if (!sa && !sb) return 0;
    if (!sa) return -1;
    if (!sb) return 1;

    // Numeric comparison if both look like integers
    int a_int = is_int_str(sa);
    int b_int = is_int_str(sb);

    if (a_int && b_int) {
        long ia = strtol(sa, NULL, 10);
        long ib = strtol(sb, NULL, 10);
        return (ia > ib) - (ia < ib);
    }

    return strcmp(sa, sb);
}

/* Sorts rows in-place by column.
 * 
 * PARAMETERS:
 *   rows      - Vec* of Row*
 *   col_index - column index to sort by 
 *
 * RETURNS:
 *   0  on success
 *  -1  on error (NULL vec, invalid column, allocation failure)
 */
int sort_by_column(Vec *rows, int col_index) {
    if (!rows || col_index < 0)
        return -1;

    size_t len = vec_length(rows);
    if (len <= 1)
        return 0;

    Row *first = vec_get(rows, 0);
    if (!first)
        return -1;

    if (col_index >= row_num_cells(first))
        return -1;

    // Validate no NULL rows inside vector
    for (size_t i = 0; i < len; i++) {
        if (vec_get(rows, i) == NULL)
            return -1;
    }

    g_sort_col = col_index;

    // Allocate temporary array of pointers 
    Row **tmp = malloc(len * sizeof(Row *));
    if (!tmp)
        return -1;

    // Copy vector items into array
    for (size_t i = 0; i < len; i++) {
        tmp[i] = vec_get(rows, i);
    }

    // Sort using qsort
    qsort(tmp, len, sizeof(Row *), rowptr_compare);

    // Copy sorted pointers back into vector
    for (size_t i = 0; i < len; i++) {
        rows->items[i] = tmp[i];
    }

    free(tmp);
    return 0;
}