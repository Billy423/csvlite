/*
 * Main orchestration module for CSVLite.
 * Integrates all modules (csv, cli, select, where, group, sort) to process CSV files.
 *
 * AUTHOR: Billy Wu, Nikhil Ranjith
 * DATE: November 21, 2025
 * VERSION: v1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"
#include "row.h"
#include "csv.h"
#include "cli.h"
#include "select.h"   
#include "where.h"    
#include "group.h"
#include "sort.h"
#include "hmap.h"

/*
 * Builds a hash map from column names to indices using the header row
 * used by apply_select() to parse column tokens in SELECT clause
 *
 * PARAMETERS:
 *  header - the header row containing column names
 *
 * RETURNS:
 *  HMap* on success, NULL on failure
 */
static HMap *build_name_to_index_map(Row *header) {
    if (header == NULL) return NULL;
    
    int num_cols = row_num_cells(header);
    if (num_cols <= 0) return NULL;
    
    HMap *map = hmap_new(num_cols);
    if (map == NULL) return NULL;
    
    for (int i = 0; i < num_cols; i++) {
        const char *name = row_get_cell(header, i);
        if (name != NULL) {
            // store (index + 1) to distinguish NULL from index 0 (since (void*)0 is NULL)
            hmap_put(map, name, (void*)(long)(i + 1));
        }
    }
    
    return map;
}

/*
 * Converts a column name to a column index using the header row from csv data
 * Also handles numeric strings
 *
 * PARAMETERS:
 *  header - the header row
 *  col_name - column name or numeric index string
 *
 * RETURNS:
 *  column index on success, -1 on failure
 */
static int get_column_index(Row *header, const char *col_name) {
    if (header == NULL || col_name == NULL) return -1;
    
    // check if it's a numeric index
    int is_numeric = 1;
    for (const char *p = col_name; *p != '\0'; p++) {
        // not numeric
        if (*p < '0' || *p > '9') {
            is_numeric = 0;
            break;
        }
    }
    
    if (is_numeric && *col_name != '\0') {
        int idx = atoi(col_name);
        int num_cols = row_num_cells(header);
        if (idx >= 0 && idx < num_cols) { // validate numeric index
            return idx;
        }
        return -1; // invalid numeric index
    }
    
    // look up by name
    int num_cols = row_num_cells(header);
    for (int i = 0; i < num_cols; i++) {
        const char* name = row_get_cell(header, i);
        if (name != NULL && strcmp(name, col_name) == 0) { // found column name
            return i;
        }
    }
    
    return -1; // column name not found
}

/*
 * Applies SELECT logic to project specific columns
 * Creates new Row objects with only the selected column
 *
 * MEMORY OWNERSHIP:
 * - returns a new Vec* with new Row* objects (caller must free)
 * - frees the input Vec and its Row objects
 */
static Vec* apply_select(Vec* rows, const char* select_cols) {
    if (select_cols == NULL || rows == NULL || vec_length(rows) == 0) {
        return rows;
    }
    
    Row *header = vec_get(rows, 0);
    if (header == NULL) {
        return rows; // no header row
    }
    
    // build name-to-index map for select_parse_indices
    HMap *name_map = build_name_to_index_map(header);
    if (name_map == NULL) {
        fprintf(stderr, "Error: Failed to build column name map\n");
        return rows;
    }
    
    int num_cols = row_num_cells(header);
    int *indices = NULL; // array of column indices
    int num_indices = 0; // number of selected columns
    
    // parse column indices
    if (select_parse_indices(select_cols, name_map, num_cols, &indices, &num_indices) != 0) {
        fprintf(stderr, "Error: Failed to parse column selection\n");
        hmap_free(name_map);
        return rows;
    }
    
    hmap_free(name_map);
    
    if (num_indices == 0 || indices == NULL) {
        fprintf(stderr, "Error: No valid columns selected\n");
        return rows;
    }
    
    // Project rows to selected columns
    Vec *projected = select_project_rows(rows, indices, num_indices);
    free(indices); // free the array of column indices
    
    if (projected == NULL) {
        fprintf(stderr, "Error: Failed to project rows\n");
        return rows;
    }
    
    // free original rows
    for (size_t i = 0; i < vec_length(rows); i++) {
        row_free(vec_get(rows, i));
    }
    vec_free(rows);
    
    return projected;
}

/*
 * Apply WHERE filtering using where_filter().
 *
 * MEMORY OWNERSHIP:
 * - where_filter() returns a new Vec but reuses Row* pointers from input
 * - only free the original Vec structure
 * - caller must free Row objects from the returned Vec later
 */
static Vec* apply_where(Vec* rows, const char* where_cond) {
    if (where_cond == NULL) {
        return rows; 
    }

    Vec *filtered = where_filter(rows, where_cond);
    if (filtered == NULL) {
        fprintf(stderr, "Error: WHERE filtering failed\n");
        return rows;  
    }

    // free original Vec structure (Row objects are shared with filtered Vec)
    vec_free(rows);
    return filtered;
}

/*
 * Applies GROUP BY logic to group rows by a column
 *
 * MEMORY OWNERSHIP:
 * - returns a new Vec* but reuses Row* pointers from input (shared)
 * - does NOT free the input Vec or Row objects
 */
static Vec *apply_group(Vec *rows, const char *group_col) {
    if (group_col == NULL || rows == NULL || vec_length(rows) == 0) {
        return rows;
    }
    
    // get the header row
    Row *header = vec_get(rows, 0);
    if (header == NULL) {
        return rows;
    }
    
    // convert column name to column index
    int col_index = get_column_index(header, group_col);
    if (col_index < 0) {
        fprintf(stderr, "Error: Column '%s' not found for GROUP BY\n", group_col);
        return rows;
    }
    
    Vec *grouped = group_by_column(rows, col_index);
    if (grouped == NULL) {
        fprintf(stderr, "Error: GROUP BY failed\n");
        return rows;
    }
    
    // group_by_column reuses internal Row* pointers, so only free the Vec structure
    // Row objects are shared between input and output
    vec_free(rows);
    
    return grouped;
}

/*
 * Applies ORDER-BY logic to sort rows by a column
 * Supports format: "col_name:asc" or "col_name:desc" (defaults to asc)
 *
 * MEMORY OWNERSHIP:
 * - returns a new Vec* but reuses Row* pointers from input (shared)
 * - does NOT free the input Vec or Row objects
 */
static Vec *apply_sort(Vec *rows, const char *order_col) {
    if (order_col == NULL || rows == NULL || vec_length(rows) == 0) {
        return rows;
    }
    
    Row *header = vec_get(rows, 0);
    if (header == NULL) {
        return rows;
    }
    
    char col_name[256]; // column name buffer
    int is_ascending = 1; // defaults to ascending
    
    // find first occurrence of ':'
    const char *colon_pos = strchr(order_col, ':');
    
    if (colon_pos != NULL) {
        // split at colon: extract column name (before colon)
        size_t name_len = colon_pos - order_col;
        if (name_len >= sizeof(col_name)) { // truncate if name is too long
            name_len = sizeof(col_name) - 1;
        }
        strncpy(col_name, order_col, name_len);
        col_name[name_len] = '\0';
        
        // check content after colon
        const char *direction = colon_pos + 1;
        size_t dir_len = strlen(direction);
        
        // check for "desc" (case-sensitive)
        if (dir_len >= 4 && strncmp(direction, "desc", 4) == 0) {
            is_ascending = 0;
        } else if (dir_len >= 3 && strncmp(direction, "asc", 3) == 0) { // check for "asc"
            is_ascending = 1;
        }
        // defaults to ascending
    } else {
        // no colon found, use entire string as column name
        strncpy(col_name, order_col, sizeof(col_name) - 1);
        col_name[sizeof(col_name) - 1] = '\0';
    }
    
    // convert column name to index
    int col_index = get_column_index(header, col_name);
    if (col_index < 0) {
        fprintf(stderr, "Error: Column '%s' not found for ORDER BY\n", col_name);
        return rows;
    }
    
    size_t len = vec_length(rows); // total number of rows
    if (len <= 1) {
        return rows;  // only the header or empty, return early
    }
    
    // create a vector with only data rows (skip header)
    Vec *data_rows = vec_new(len - 1);
    if (data_rows == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for sorting\n");
        return rows;
    }
    
    for (size_t i = 1; i < len; i++) {
        Row *row = vec_get(rows, i);
        if (row == NULL || vec_push(data_rows, row) != 0) {
            vec_free(data_rows);
            return rows;
        }
    }
    
    // sort only the data rows
    Vec *sorted_data = sort_by_column(data_rows, col_index, is_ascending);
    vec_free(data_rows);  // free the temporary data_rows vector
    
    if (sorted_data == NULL) {
        fprintf(stderr, "Error: ORDER BY failed\n");
        return rows;
    }
    
    // create final result: header + sorted data
    Vec *result = vec_new(len);
    if (result == NULL) {
        vec_free(sorted_data); // free only the sorted_data vector 
        return rows;
    }
    
    // add header first
    if (vec_push(result, header) != 0) {
        vec_free(result);
        vec_free(sorted_data);
        return rows;
    }
    
    // add sorted data rows
    for (size_t i = 0; i < vec_length(sorted_data); i++) {
        Row *row = vec_get(sorted_data, i);
        if (row == NULL || vec_push(result, row) != 0) {
            vec_free(result);
            vec_free(sorted_data);
            return rows;
        }
    }
    
    // free only temporary structures
    vec_free(sorted_data);
    vec_free(rows);
    
    return result;
}

/*
 * Processes CSV file
 * 
 * Operation order: reads, WHERE, GROUP BY, ORDER BY, and SELECT, then writes output.
 */
static int process_csv(FILE* input, const char* select_cols, const char* where_cond,
                       const char *group_by_col, const char *order_by_col) {
    Vec* rows = csv_read(input);
    if (rows == NULL) {
        fprintf(stderr, "Error: Failed to read CSV\n");
        return 1;
    }
    
    if (vec_length(rows) == 0) {
        fprintf(stderr, "Error: CSV file is empty\n");
        vec_free(rows);
        return 1;
    }

    // apply WHERE condition
    rows = apply_where(rows, where_cond);
    if (rows == NULL) {
        fprintf(stderr, "Error: WHERE filtering failed\n");
        return 1;
    }

    // apply GROUP BY
    rows = apply_group(rows, group_by_col);
    if (rows == NULL) {
        fprintf(stderr, "Error: GROUP BY failed\n");
        return 1;
    }

    // apply ORDER BY
    rows = apply_sort(rows, order_by_col);
    if (rows == NULL) {
        fprintf(stderr, "Error: ORDER BY failed\n");
        return 1;
    }

    // validate SELECT columns (if provided)
    if (select_cols != NULL) {
        Row* header = vec_get(rows, 0);

        // free rows if header is NULL
        if (header == NULL) {
            fprintf(stderr, "Error: No header row found\n");
            for (size_t i = 0; i < vec_length(rows); i++) {
                row_free(vec_get(rows, i));
            }
            vec_free(rows);
            return 1;
        }

        if (csv_validate_columns(header, select_cols) != 0) {
            fprintf(stderr, "Error: Invalid column selection\n");
            for (size_t i = 0; i < vec_length(rows); ++i) {
                row_free(vec_get(rows, i));
            }
            vec_free(rows);
            return 1;
        }
    }

    // apply SELECT
    rows = apply_select(rows, select_cols);
    if (rows == NULL) {
        fprintf(stderr, "Error: SELECT failed\n");
        return 1;
    }

    // write output CSV
    // after SELECT projection, rows already contain only selected columns,
    // so pass NULL to csv_write to write all columns from projected rows
    if (csv_write(stdout, rows, NULL) != 0) {

        // write failed
        fprintf(stderr, "Error: Failed to write output\n");

        for (size_t i = 0; i < vec_length(rows); ++i) {
            row_free(vec_get(rows, i));
        }
        vec_free(rows);
        return 1;
    }

    // cleanup
    for (size_t i = 0; i < vec_length(rows); ++i) {
        row_free(vec_get(rows, i));
    }
    vec_free(rows);

    return 0;
}

int main(int argc, char* argv[]) {
    cli_init();

    // parse CLI args
    int parse_result = cli_parse_args(argc, argv);
    if (parse_result <= 0) {
        return (parse_result == -1) ? 0 : 1;
    }

    FILE* input = stdin;

    if (!g_use_stdin) {
        if (g_file_path == NULL) {
            fprintf(stderr, "Error: No input file specified\n");
            return 1;
        }
        input = fopen(g_file_path, "r");
        if (input == NULL) {
            fprintf(stderr, "Error: Cannot open file %s\n", g_file_path);
            return 1;
        }
    }

    int result = process_csv(input, g_select_cols, g_where_cond, g_group_by_col, g_order_by_col);

    if (!g_use_stdin && input != NULL) {
        fclose(input);
    }

    cli_cleanup();
    return result;
}
