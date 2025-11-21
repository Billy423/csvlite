/* Header file defining the interface for group-by operations in CSVLite.
 * Provides function declarations for grouping and future aggregation logic.
 *
 * Vivek Patel, November 11, 2025, v0.0.3
 */

#ifndef GROUP_H
#define GROUP_H

#include "vec.h"
#include "row.h"
#include "hmap.h"

/* Groups rows by a specific column index.
 * Returns a new Vec* containing one representative Row* per group.
 *
 * MEMORY OWNERSHIP:
 * - Returns a new Vec* that the caller must free with vec_free()
 * - Reuses Row* pointers from input (does NOT copy Row objects)
 * - Caller must free Row objects separately (they are shared)
 * - Does NOT free the input Vec or Row objects
 *
 * PARAMETERS:
 *  rows, a Vec* containing Row* elements
 *  col_index, the column index to group by
 *
 * RETURNS:
 *  Vec*, a new vector of grouped rows (caller must free)
 */
Vec* group_by_column(Vec* rows, int col_index);

/* Placeholder for aggregation functionality.
 * To be implemented. 
 *
 * PARAMETERS:
 *  grouped_rows, a Vec* containing grouped rows
 *
 * RETURNS:
 *  void
 */
void group_aggregate(Vec* grouped_rows);

#endif
