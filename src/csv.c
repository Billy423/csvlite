/*
 * Provides functions for reading and writing CSV files.
 *   - Reads CSV data into vectors of rows
 *   - Writes CSV data from vectors of rows to stdout or files
 *   - Validating columns for SELECT operations
  * This module works closely with row.c and vec.c to represent
 * CSV rows and collections of rows in memory.
 *
 * AUTHOR: Nikhil Ranjith
 * DATE: November 16, 2025
 * VERSION: v1.0.0
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/csv.h"

/* Helper: trim leading/trailing spaces/tabs */
static void trim_inplace(char *s) {
    if (!s) return;
    /* trim left */
    char *start = s;
    while (*start && (*start == ' ' || *start == '\t')) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);

    /* trim right */
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

/* Implementation: read CSV into Vec<Row*> */
Vec* csv_read(FILE *input) {
    if (input == NULL) return NULL;

    Vec* rows = vec_new(16);
    if (rows == NULL) return NULL;

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), input)) {
        size_t len = strlen(line);
        /* remove newline */
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        /* skip empty lines */
        if (len == 0) continue;

        /* count commas to determine number of columns */
        int num_cols = 1;
        for (size_t i = 0; i < len; i++) {
            if (line[i] == ',') num_cols++;
        }

        Row *row = row_new(num_cols);
        if (row == NULL) {
            /* cleanup */
            vec_free(rows);
            return NULL;
        }
        // length plus null terminator
        char *tmp = malloc(len + 1);
        if (!tmp) {
            row_free(row);
            vec_free(rows);
            return NULL;
        }
        strcpy(tmp, line);

        // Process the columns by tokenizing
        char *tok = NULL;
        int col = 0;
        tok = strtok(tmp, ",");
        while (tok != NULL && col < num_cols) {
            trim_inplace(tok);
            if (row_set_cell(row, col, tok) != 0) {
                /* CLEANUP IF row_set_cell FAILS */
                free(tmp);
                row_free(row);

                for (size_t i = 0; i < vec_length(rows); i++)
                    row_free(vec_get(rows, i));
                vec_free(rows);
                return NULL;
            }
            col++;
            tok = strtok(NULL, ",");
        }
        /* if fewer tokens than num_cols, set remaining to empty string */
        while (col < num_cols) {
            row_set_cell(row, col, "");
            col++;
        }

        free(tmp);

        if (vec_push(rows, row) != 0) {
            /* cleanup on failure */
            row_free(row);
            /* free previously pushed rows */
            for (size_t i = 0; i < vec_length(rows); ++i) row_free(vec_get(rows, i));
            vec_free(rows);
            return NULL;
        }
    }

    return rows;
}

/* Helper: determine if token is all digits (non-empty) */
static int is_number_token(const char *tok) {
    if (!tok || *tok == '\0') return 0;
    for (const char *p = tok; *p; ++p) {
        if (!isdigit((unsigned char)*p)) return 0;
    }
    return 1;
}

/*  checks each comma-separated token exists as a numeric index within range [0..n-1]
 *  or as  header name matching one of header's cells
 */
int csv_validate_columns(Row* header, const char* selected_cols) {
    if (header == NULL || selected_cols == NULL) return -1;
    if (selected_cols[0] == '\0') return -1;

    int ncols = row_num_cells(header);
    if (ncols <= 0) return -1;

    // make a copy
    char *copy = malloc(strlen(selected_cols) + 1);
    if (!copy) return -1;
    strcpy(copy, selected_cols);

    char *tok = strtok(copy, ",");
    while (tok != NULL) {
        trim_inplace(tok);
        if (tok[0] == '\0') { free(copy); return -1; }

        if (is_number_token(tok)) {
            int idx = atoi(tok);
            if (idx < 0 || idx >= ncols) { free(copy); return -1; }
        } else {
            int found = 0;
            for (int i = 0; i < ncols; ++i) {
                const char* name = row_get_cell(header, i);
                if (name && strcmp(name, tok) == 0) { found = 1; break; }
            }
            if (!found) { free(copy); return -1; }
        }

        tok = strtok(NULL, ",");
    }

    free(copy);
    return 0;
}

// parse selected_cols into integer index array 
static int parse_selected_indices(Row* header, const char* selected_cols, int **out_indices) {
    if (!header || !selected_cols || out_indices == NULL) return -1;

    int ncols = row_num_cells(header);
    if (ncols <= 0) return -1;

    /* copy string */
    char *copy = malloc(strlen(selected_cols) + 1);
    if (!copy) return -1;
    strcpy(copy, selected_cols);

    int *indices = malloc(sizeof(int) * ncols); /* upper bound */
    if (!indices) { free(copy); return -1; }
    int count = 0;

    char *tok = strtok(copy, ",");
    while (tok != NULL) {
        trim_inplace(tok);
        if (is_number_token(tok)) {
            indices[count++] = atoi(tok);
        } else {
            /* find header name index */
            int found = 0;
            for (int i = 0; i < ncols; ++i) {
                const char *name = row_get_cell(header, i);
                if (name && strcmp(name, tok) == 0) {
                    indices[count++] = i;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                free(indices);
                free(copy);
                return -1;
            }
        }
        tok = strtok(NULL, ",");
    }

    free(copy);
    *out_indices = indices;
    return count;
}

/* Write CSV rows.
 * Returns 0 on success, -1 on error.
 */
int csv_write(FILE* output, Vec* rows, const char* selected_cols) {
    if (output == NULL || rows == NULL) return -1;
    if (vec_length(rows) == 0) return -1;

    Row* header = vec_get(rows, 0);
    if (!header) return -1;

    int num_cols = row_num_cells(header);
    if (num_cols <= 0) return -1;

    if (selected_cols == NULL) {
        /* print all columns */
        for (size_t r = 0; r < vec_length(rows); ++r) {
            Row *row = vec_get(rows, r);
            for (int c = 0; c < num_cols; ++c) {
                if (c > 0) fputc(',', output);
                const char *val = row_get_cell(row, c);
                fputs(val ? val : "", output);
            }
            fputc('\n', output);
        }
        return 0;
    }

    /* parse selection into indices */
    int *indices = NULL;
    int nsel = parse_selected_indices(header, selected_cols, &indices);
    if (nsel <= 0) {
        free(indices);  // free if allocated but no valid indices found
        return -1;
    }

    for (size_t r = 0; r < vec_length(rows); ++r) {
        Row *row = vec_get(rows, r);
        for (int i = 0; i < nsel; ++i) {
            if (i > 0) fputc(',', output);
            int col = indices[i];
            const char *val = row_get_cell(row, col);
            fputs(val ? val : "", output);
        }
        fputc('\n', output);
    }

    free(indices);
    return 0;
}
