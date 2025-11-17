/*
* AUTHOR: Billy Wu, Nikhil Ranjith
* DATE: November 10, 2025
* VERSION: v0.0.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"
#include "row.h"
#include "csv.h"
#include "cli.h"
#include "select.h"   
// #include "where.h"    


/*
 * Placeholder for applying SELECT logic.
 */
static Vec* apply_select(Vec* rows, const char* select_cols) {
    if (select_cols == NULL) {
        return rows; /* nothing to do */
    }

    /* TODO: call real select_columns() once implemented */
    printf("[INFO] SELECT placeholder: would select columns: %s\n", select_cols);

    return rows;
}

/*
 * Placeholder for applying WHERE filtering.
 */
static Vec* apply_where(Vec* rows, const char* where_cond) {
    if (where_cond == NULL) {
        return rows; /* nothing to do */
    }

    /* TODO: call real where_filter() once implemented */
    printf("[INFO] WHERE placeholder: would filter rows using condition: %s\n", where_cond);

    return rows;
}

// Read csv file, apply WHERE and SELECT, then write output.
static int process_csv(FILE* input, const char* select_cols, const char* where_cond) {
    Vec* rows = csv_read(input);
    if (rows == NULL) {
        fprintf(stderr, "Error: Failed to read CSV\n");
        return 1;
    }

    /* Apply WHERE condition */
    rows = apply_where(rows, where_cond);

    /* Validate SELECT columns (if provided) */
    if (select_cols != NULL) {
        Row* header = vec_get(rows, 0);
        if (csv_validate_columns(header, select_cols) != 0) {
            fprintf(stderr, "Error: Invalid column selection\n");
            for (size_t i = 0; i < vec_length(rows); ++i) {
                row_free(vec_get(rows, i));
            }
            vec_free(rows);
            return 1;
        }
    }

    /* Apply SELECT */
    rows = apply_select(rows, select_cols);

    /* Write output CSV */
    if (csv_write(stdout, rows, select_cols) != 0) {
        fprintf(stderr, "Error: Failed to write output\n");

        for (size_t i = 0; i < vec_length(rows); ++i) {
            row_free(vec_get(rows, i));
        }
        vec_free(rows);
        return 1;
    }

    /* Cleanup */
    for (size_t i = 0; i < vec_length(rows); ++i) {
        row_free(vec_get(rows, i));
    }
    vec_free(rows);

    return 0;
}

int main(int argc, char* argv[]) {
    cli_init();

    /* Parse CLI args */
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

    int result = process_csv(input, g_select_cols, g_where_cond);

    if (!g_use_stdin && input != NULL) {
        fclose(input);
    }

    cli_cleanup();
    return result;
}
