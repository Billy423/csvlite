/* Sorting operations for CSV rows.
 * Provides functions for sorting a vector of Row pointers based on a
 * specific column index. Uses qsort() from stdlib.
 *
 * AUTHOR: Vivek Patel
 * DATE: November 17, 2025
 * VERSION: v0.0.3
 */

#ifndef SORT_H
#define SORT_H

#include "vec.h"
#include "row.h"

/* Sorts rows by a specified column index.
 * Returns a NEW Vec* containing sorted Row* pointers.
 * 
 * PARAMETERS:
 *   rows - Vec* of Row*
 *   col_index - column to sort by
 *
 * RETURNS:
 *   Vec*  - newly allocated sorted vector
 *   NULL  - on invalid arguments or memory failure
 */
Vec *sort_by_column(Vec *rows, int col_index);

#endif