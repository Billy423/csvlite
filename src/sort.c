/*
 * Implements basic sorting for CSV rows using qsort().
 * Uses a global comparator with the target column + direction
 * so qsort does not need additional context.
 * Sorting supports both numeric and text ordering based on content.
 * 
 * AUTHOR: Vivek Patel
 * DATE: November 17, 2025
 * VERSION: v2.0.0
 */


#include "../include/sort.h"
#include "../include/vec.h"
#include "../include/row.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// global temporary variables for comparator
static int g_sort_col = 0;
static int g_sort_ascending = 1;  // 1 for ascending, 0 for descending

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
    // Must have at least 1 digit
    if (!*s) return 0;
    
    // Check all characters
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

    int result;
    if (a_int && b_int) {
        long ia = strtol(sa, NULL, 10);
        long ib = strtol(sb, NULL, 10);
        result = (ia > ib) - (ia < ib);
    } else {
        result = strcmp(sa, sb);
    }
    
    // Flip direction for descending
    return g_sort_ascending ? result : -result;
}

/* Sorts rows by column and returns a new sorted vector.
 * The original vector is not modified.
 * 
 * PARAMETERS:
 *   rows      - Vec* of Row*
 *   col_index - column index to sort by 
 *   ascending - 1 for ascending order, 0 for descending order
 *
 * RETURNS:
 *   A new Vec* containing the sorted rows.
 *   NULL on invalid input or memory failure.
 */
Vec *sort_by_column(Vec *rows, int col_index, int ascending) {
    if (!rows || col_index < 0)
        return NULL;

    size_t len = vec_length(rows);
    if (len == 0)
        return NULL;
    
    // Single row, create new vector with the row
    if (len == 1) {
        Vec *sorted = vec_new(1);
        if (sorted == NULL) { // allocation failed
            return NULL;
        }

        Row *row = vec_get(rows, 0);
        if (row == NULL) { // row is NULL
            vec_free(sorted);
            return NULL;
        }
        
        if (vec_push(sorted, row) != 0) { // failed to add row
            vec_free(sorted);
            return NULL;
        }
        return sorted;
    }

    Row *first = vec_get(rows, 0);
    if (!first)
        return NULL;

    if (col_index >= row_num_cells(first))
        return NULL;

    // Buffer used by qsort
    Row **tmp = malloc(sizeof(Row *) * len);
    if (!tmp)
        return NULL;

    // Copy rows into temp array
    for (size_t i = 0; i < len; i++) {
        Row *current = vec_get(rows, i);
        if (!current) {
            free(tmp);
            return NULL;
        }
        tmp[i] = current;
    }

    // Comparator configuration
    g_sort_col = col_index;
    g_sort_ascending = ascending;
    
    // Preform sorting
    qsort(tmp, len, sizeof(Row *), rowptr_compare);

    // Build a NEW vector with sorted rows
    Vec *sorted = vec_new(len);
    if (!sorted) {
        free(tmp);
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        if (vec_push(sorted, tmp[i]) != 0) {
            vec_free(sorted);
            free(tmp);
            return NULL;
        }
    }

    free(tmp);
    return sorted;
}