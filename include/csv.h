/*
* AUTHOR: Nikhil Ranjith
* DATE: November 30, 2025
* VERSION: v2.0.0
*
* CSV parsing/writing helpers:
* - csv_read trims tokens, strips newlines, pads missing trailing cells with ""
* - csv_validate_columns checks name or numeric indices in a comma list
* - csv_write accepts name or numeric selections and returns -1 on invalid selection
*/

#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include "vec.h"
#include "row.h"

#define MAX_LINE_LENGTH 2048

// Read CSV from file
Vec* csv_read(FILE *input);

// Validate column names
int csv_validate_columns(Row* header, const char* selected_cols);

// Write output (selected columns or all)
int csv_write(FILE* output, Vec* rows, const char* selected_cols);

#endif
