/**
 * row.c - Row data structure for storing CSV row data
 * 
 * Provides a structure to store a single CSV row with multiple cells.
 * Each cell value is stored as a dynamically allocated string.
 * 
 * AUTHOR: Billy
 * DATE: 2025-11-11
 * VERSION: v0.0.1
 */

#include "../include/row.h"
#include <stdlib.h>
#include <string.h>

// Row structure: stores array of cell values (strings)
struct Row {
    char **cells;  // array of string pointers
    int num_cols;  // number of columns in this row
};

/* 
 * Creates a new row with the specified number of columns
 * All cells are initialized to NULL
 *
 * parameters:
 * - num_cols: number of columns in the row (must be > 0)
 *
 * RETURN: pointer to new Row on success, NULL on allocation failure
 */
Row *row_new(int num_cols) {

    // bad parameters
    if (num_cols <= 0) return NULL;
    
    // allocate row structure
    Row *row = malloc(sizeof(Row));
    if (row == NULL) { // allocation failed
        return NULL;
    }
    
    // allocate array of string pointers, initialize to NULL
    row->cells = calloc(num_cols, sizeof(char *));
    if (row->cells == NULL) { // allocation failed
        free(row);
        return NULL;
    }
    
    row->num_cols = num_cols;
    return row;
}

/* 
 * Sets the cell value at the specified column inde
 * The value string is copied internally, so the caller can free the original
 *
 * parameters:
 * - row: row to modify
 * - col: column index (0-based)
 * - value: string value to set (will be copied)
 *
 * RETURN: 0 on success, -1 on failure (invalid row/col or allocation failed).
 */
int row_set_cell(Row *row, int col, const char *value) {
    // bad parameters
    if (row == NULL || col < 0 || col >= row->num_cols) return -1;
    
    // free existing cell value if already exists
    if (row->cells[col] != NULL) {
        free(row->cells[col]);
        row->cells[col] = NULL;
    }
    
    if (value == NULL) return 0;
    
    // copy value string (caller can free the original)
    size_t len = strlen(value);
    row->cells[col] = malloc(len + 1); // allow space for '\0' terminator
    if (row->cells[col] == NULL) { // allocation failed
        return -1;
    }
    
    strcpy(row->cells[col], value);
    return 0;
}

/* 
 * Retrieves the cell value at the specified column index.
 *
 * parameters:
 * - row: row to query
 * - col: column index (0-based)
 *
 * RETURN: pointer to cell string if valid, NULL if invalid index or row is NULL.
 */
const char *row_get_cell(const Row *row, int col) {
    // bad parameters
    if (row == NULL || col < 0 || col >= row->num_cols) return NULL;

    return row->cells[col]; // may be NULL if cell itself is NULL
}

/* 
 * Returns the number of columns in the row
 *
 * parameters:
 * - row: row to query
 *
 * RETURN: number of columns, 0 if row is NULL
 */
int row_num_cells(const Row *row) {
    return row == NULL ? 0 : row->num_cols;
}

/* 
 * Frees all resources associated with the row
 * All cell strings are freed, then the row structure itself
 *
 * parameters:
 * - row: row to free (safe to pass NULL)
 *
 * RETURN: void (no return value).
 */
void row_free(Row *row) {

    if (row == NULL) return;
    
    // free all cell strings if exists
    if (row->cells != NULL) {
        for (int i = 0; i < row->num_cols; i++) {
            if (row->cells[i] != NULL) {
                free(row->cells[i]);
            }
        }
        free(row->cells);
    }
    
    free(row);
}

