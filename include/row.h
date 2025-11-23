/*
* AUTHOR: Billy
* DATE: November 11, 2025
* VERSION: v1.0.0
*/

#ifndef ROW_H
#define ROW_H

#include <stddef.h>

// Row structure for storing CSV row data
typedef struct Row Row;

// Create a new row with num_cols columns
// - returns NULL if failed
Row *row_new(int num_cols);

// Set cell at column col (0-based) to value
// - returns 0 on success, -1 if failed
int row_set_cell(Row *row, int col, const char *value);

// Get cell value at column col (0-based)
// - returns NULL if invalid index
const char *row_get_cell(const Row *row, int col);

// Get number of columns in row
int row_num_cells(const Row* row);

// Free row and all its resources
void row_free(Row* row);

#endif

