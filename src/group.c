/*
 * Implements the group-by functionality for the CSVLite project.
 * Groups rows by a chosen column using a hash map to track which
 * group keys have already been seen. Produces one representative
 * row per unique key (the first occurrence).
 * 
 * AUTHOR: Vivek Patel
 * DATE: November 11, 2025
 * VERSION: v2.0.0
 */

#include "group.h"
#include "vec.h"
#include "row.h"
#include "hmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/* Helper cleanup used when any part of grouping fails.
 * Frees all rows in the output Vec, ensuring no memory leaks.
 * Used so the function exits safely if something goes wrong.
 */
static void free_group_results(Vec *grouped) {
    if (!grouped) return;
    for (size_t i = 0; i < vec_length(grouped); i++) {
        Row *r = vec_get(grouped, i);
        row_free(r);
    }
    vec_free(grouped);
}

/* Groups rows by a specific column index. Each unique column value
 * (from the specified index) is stored in a hash map, ensuring that
 * only one representative row per group is kept.
 * 
 * MEMORY OWNERSHIP:
 * - Returns a new Vec* that the caller must free with vec_free()
 * - Reuses Row* pointers from input (does NOT copy Row objects)
 * - Caller must free Row objects separately (they are shared)
 * - Does NOT free the input Vec or Row objects
 *
 * PARAMETERS:
 *  rows, a Vec* containing Row* elements (input dataset)
 *  col_index, the index of the column to group by
 *
 * RETURNS:
 *  A new Vec* containing one representative Row* per unique group.
 *  Returns NULL if an invalid argument or allocation failure occurs.
 * 
 * For each unique group key, this function returns the FIRST row encountered.
 */
Vec* group_by_column(Vec* rows, int col_index)
{
    
    // Validate input rows and column index
    if (!rows || vec_length(rows) == 0) {
        return NULL;
    }

    // Protect against invalid row 0 since function depends on it
    if (vec_get(rows, 0) == NULL) {
        return NULL;
    }

    // Validate index using first row's cell count
    Row *first = vec_get(rows, 0);
    if (col_index < 0 || col_index >= row_num_cells(first)) {
        return NULL;
    }

    size_t n = vec_length(rows);

    // Reject if first row is NULL
    if (vec_get(rows, 0) == NULL) {
        return NULL;
    }

    // Hash map tracks which keys we've seen
    HMap *seen = hmap_new(16);
    if (!seen) return NULL;

    // Output vector
    Vec *grouped = vec_new(8);
    if (!grouped) {
        hmap_free(seen);
        return NULL;
    }

    // Iterate through all rows
    for (size_t i = 0; i < n; i++) {
        Row *row = vec_get(rows, i);
        // skip NULL rows
        if (!row) continue; 

        // Key used for grouping
        const char *key = row_get_cell(row, col_index);
        if (!key) key = "";

        // If key not yet recorded, make new key
        if (hmap_get(seen, key) == NULL) {

            // Insert into hashmap, skipping duplicates
            if (hmap_put(seen, key, row) != 0) {
                free_group_results(grouped);
                hmap_free(seen);
                return NULL;
            }

            // Add to result vector
            if (vec_push(grouped, row) != 0) {
                free_group_results(grouped);
                hmap_free(seen);
                return NULL;
            }
        }
    }

    // Cleanup
    hmap_free(seen);
    return grouped;
}
