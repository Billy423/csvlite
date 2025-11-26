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

static Vec* build_sample_rows(void) {
    Vec* rows = vec_new(4);
    if (!rows) return NULL;

    Row* header = row_new(3);
    Row* r1 = row_new(3);
    Row* r2 = row_new(3);
    if (!header || !r1 || !r2) {
        if (header) row_free(header);
        if (r1) row_free(r1);
        if (r2) row_free(r2);
        vec_free(rows);
        return NULL;
    }

    row_set_cell(header, 0, "name");
    row_set_cell(header, 1, "age");
    row_set_cell(header, 2, "city");

    row_set_cell(r1, 0, "Alice");
    row_set_cell(r1, 1, "30");
    row_set_cell(r1, 2, "Seattle");

    row_set_cell(r2, 0, "Bob");
    row_set_cell(r2, 1, "25");
    row_set_cell(r2, 2, "Denver");

    vec_push(rows, header);
    vec_push(rows, r1);
    vec_push(rows, r2);
    return rows;
}

static void free_rows(Vec* rows) {
    if (!rows) return;
    for (size_t i = 0; i < vec_length(rows); i++) {
        row_free(vec_get(rows, i));
    }
    vec_free(rows);
}

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

// Test: csv_read handles whitespace and missing columns
static void test_csv_read_whitespace_and_missing(void) {
    FILE* tmp = tmpfile();
    TEST(tmp != NULL, "tmpfile created for csv_read test", "failed to create tmpfile");
    if (!tmp) return;

    fputs(" name , age , city \nAlice,30,Seattle\nBob,25,\n\n", tmp);
    rewind(tmp);

    Vec* rows = csv_read(tmp);
    fclose(tmp);

    TEST(rows != NULL, "csv_read returns rows for tmpfile", "csv_read returned NULL for tmpfile");
    if (!rows) return;

    TEST(vec_length(rows) == 3, "csv_read keeps header and two data rows", "csv_read wrong row count");
    Row* header = vec_get(rows, 0);
    TEST(strcmp(row_get_cell(header, 0), "name") == 0 &&
         strcmp(row_get_cell(header, 1), "age") == 0 &&
         strcmp(row_get_cell(header, 2), "city") == 0,
         "header cells trimmed correctly", "header cells not trimmed correctly");

    Row* bob = vec_get(rows, 2);
    TEST(strcmp(row_get_cell(bob, 2), "") == 0, "missing trailing column set to empty string", "missing trailing column not empty");

    free_rows(rows);
}

// Test: csv_read handles NULL input
static void test_csv_read_null_input(void) {
    Vec* rows = csv_read(NULL);
    TEST(rows == NULL, "csv_read returns NULL for NULL input", "csv_read did not return NULL for NULL input");
}

// Test: csv_validate_columns success and failures
static void test_csv_validate_columns_cases(void) {
    Row* header = row_new(3);
    TEST(header != NULL, "header allocated for validate tests", "header allocation failed");
    if (!header) return;

    row_set_cell(header, 0, "name");
    row_set_cell(header, 1, "age");
    row_set_cell(header, 2, "city");

    TEST(csv_validate_columns(header, "0,2") == 0, "validate numeric indices succeeds", "validate numeric indices failed");
    TEST(csv_validate_columns(header, " name , age ") == 0, "validate trims tokens", "validate failed to trim tokens");
    TEST(csv_validate_columns(header, NULL) == -1, "validate NULL input fails", "validate NULL input did not fail");
    TEST(csv_validate_columns(header, "") == -1, "validate empty string fails", "validate empty string did not fail");
    TEST(csv_validate_columns(header, "5") == -1, "validate out-of-range index fails", "validate out-of-range index did not fail");
    TEST(csv_validate_columns(header, "unknown") == -1, "validate unknown header fails", "validate unknown header did not fail");
    TEST(csv_validate_columns(header, "name,   ,age") == -1, "validate detects empty trimmed token", "validate did not detect empty trimmed token");

    row_free(header);
}

// Test: csv_write selected columns and invalid selections
static void test_csv_write_selected_columns(void) {
    Vec* rows = build_sample_rows();
    TEST(rows != NULL, "sample rows built for csv_write", "failed to build sample rows");
    if (!rows) return;

    FILE* tmp = tmpfile();
    TEST(tmp != NULL, "tmpfile created for csv_write", "failed to create tmpfile for csv_write");
    if (!tmp) {
        free_rows(rows);
        return;
    }

    int rc = csv_write(tmp, rows, "age,name");
    TEST(rc == 0, "csv_write succeeds with selected columns", "csv_write failed with selected columns");

    fflush(tmp);
    rewind(tmp);
    char buffer[128] = {0};
    fread(buffer, 1, sizeof(buffer) - 1, tmp);
    TEST(strstr(buffer, "age,name\n30,Alice\n25,Bob\n") == buffer,
         "csv_write output matches expected order", "csv_write output incorrect");

    fclose(tmp);
    free_rows(rows);
}

// Test: csv_write error paths
static void test_csv_write_invalid_inputs(void) {
    Vec* rows = build_sample_rows();
    TEST(rows != NULL, "sample rows built for invalid csv_write tests", "failed to build sample rows for invalid tests");
    if (!rows) return;

    FILE* tmp = tmpfile();
    TEST(tmp != NULL, "tmpfile created for invalid csv_write tests", "failed to create tmpfile for invalid tests");
    if (!tmp) {
        free_rows(rows);
        return;
    }

    TEST(csv_write(NULL, rows, NULL) == -1, "csv_write fails when output is NULL", "csv_write did not fail with NULL output");
    TEST(csv_write(tmp, NULL, NULL) == -1, "csv_write fails when rows is NULL", "csv_write did not fail with NULL rows");

    Vec* empty = vec_new(1);
    TEST(csv_write(tmp, empty, NULL) == -1, "csv_write fails when rows vector empty", "csv_write did not fail with empty rows");
    vec_free(empty);

    int rc = csv_write(tmp, rows, "unknown");
    TEST(rc == -1, "csv_write fails with invalid selection", "csv_write did not fail with invalid selection");

    fclose(tmp);
    free_rows(rows);
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

    test_csv_read_whitespace_and_missing();
    test_csv_read_null_input();
    test_csv_validate_columns_cases();
    test_csv_write_selected_columns();
    test_csv_write_invalid_inputs();

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
