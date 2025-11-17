/*
* CSV unit tests: read existing files from "data" folder
*
* AUTHOR: Nikhil Ranjith
* DATE: November 10, 2025
* VERSION: v0.0.1
*/

#include "../../include/csv.h"
#include "../../include/vec.h"
#include "../../include/row.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(condition, success_message, failure_message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("PASS: %s\n", success_message); \
        } else { \
            printf("FAIL: %s\n", failure_message); \
        } \
    } while (0)

// Helper: Process a CSV file for general tests
static void process_csv_file(const char* filepath) {
    FILE* f = fopen(filepath, "r");
    TEST(f != NULL, "CSV file opens", "CSV file does NOT open");
    if (!f) return;

    Vec* rows = csv_read(f);
    fclose(f);

    TEST(rows != NULL, "csv_read() returns non-NULL", "csv_read() returned NULL");
    if (!rows) return;

    size_t nrows = vec_length(rows);
    printf("[INFO] File '%s' contains %zu rows\n", filepath, nrows);

    if (nrows > 0) {
        Row* header = vec_get(rows, 0);
        size_t ncols = row_num_cells(header);

        printf("[INFO] Header columns (%zu): ", ncols);
        for (size_t i = 0; i < ncols; i++) {
            const char* col = row_get_cell(header, i);
            printf("%s%s", col, (i < ncols - 1) ? ", " : "\n");
        }

        // Test column validation
        TEST(csv_validate_columns(header, row_get_cell(header, 0)) == 0,
             "First column validation OK",
             "First column validation FAILED");
    }

    /* Clean up rows */
    for (size_t i = 0; i < nrows; i++) {
        row_free(vec_get(rows, i));
    }
    vec_free(rows);
}

int main(void) {
    const char* files[] = {
        "data/empty.csv",
        "data/students.csv",
        "data/simple.csv"
    };
    const size_t nfiles = sizeof(files) / sizeof(files[0]);

    printf("=== CSV Unit Tests (existing files) ===\n\n");

    for (size_t i = 0; i < nfiles; i++) {
        printf("Processing '%s'...\n", files[i]);
        process_csv_file(files[i]);
        printf("\n");
    }

    printf("=== Test Summary ===\n");
    printf("CSV Tests run: %d\n", tests_run);
    printf("CSV Tests passed: %d\n", tests_passed);
    printf("CSV Tests failed: %d\n", tests_run - tests_passed);

    if (tests_run == tests_passed) {
        return 0;  
    } else {
        return 1;  
    }
}
