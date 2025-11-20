/* Sorting operations for CSV rows.
 * Provides functions for sorting a vector of Row pointers based on a
 * specific column index. Uses qsort() from stdlib.
 *
 * Vivek Patel, November 17, 2025, v0.0.1
 */

#ifndef SORT_H
#define SORT_H

#include "vec.h"
#include "row.h"

/* Sorts rows in-place by column index.
 *
 * PARAMETERS:
 *   rows, Vec* of Row*
 *   col_index, column to sort by
 *
 * RETURNS:
 *   0 on success, -1 on error
 */
int sort_by_column(Vec *rows, int col_index);

#endif