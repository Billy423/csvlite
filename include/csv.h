/*
* AUTHOR: Nikhil Ranjith
* DATE: November 10, 2025
* VERSION: v1.0.0
*/

#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include "vec.h"
#include "row.h"

#define MAX_LINE_LENGTH 2048

/* Read CSV from file */
Vec* csv_read(FILE *input);

/* Validate column names */
int csv_validate_columns(Row* header, const char* selected_cols);

/* Write output (selected columns or all) */
int csv_write(FILE* output, Vec* rows, const char* selected_cols);

#endif
