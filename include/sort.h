/*
* AUTHOR: Vivek Patel
* DATE: November 17, 2025
* VERSION: v2.0.0
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
 *   ascending - 1 for ascending order, 0 for descending order
 *
 * RETURNS:
 *   Vec*  - newly allocated sorted vector
 *   NULL  - on invalid arguments or memory failure
 */
Vec *sort_by_column(Vec *rows, int col_index, int ascending);

#endif