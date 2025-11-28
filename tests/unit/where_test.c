/*
* Tests to verify where clauses
*
*
* AUTHOR: Nadeem Mohamed
* DATE: November 23, 2025
* VERSION: v0.0.1
*/
 

#include "../../include/where.h"
#include "../../include/row.h"
#include "../../include/vec.h"

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

/* Helper: build a small students table:
 *
 * header: name,age,gpa
 * row 1:  Alice,20,3.9
 * row 2:  Bob,19,3.5
 * row 3:  Carl,17,2.8
 *
 * RETURN: Vec* of Row* (or NULL on failure)
 */
static Vec *build_sample_rows(void) {
    Vec *rows = vec_new(4);
    if (rows == NULL) return NULL;

    Row *header = row_new(3);
    if (header == NULL) {
        vec_free(rows);
        return NULL;
    }
    row_set_cell(header, 0, "name");
    row_set_cell(header, 1, "age");
    row_set_cell(header, 2, "gpa");
    vec_push(rows, header);

    Row *r1 = row_new(3);
    Row *r2 = row_new(3);
    Row *r3 = row_new(3);
    if (r1 == NULL || r2 == NULL || r3 == NULL) {
        if (r1) row_free(r1);
        if (r2) row_free(r2);
        if (r3) row_free(r3);
        row_free(header);
        vec_free(rows);
        return NULL;
    }

    row_set_cell(r1, 0, "Alice");
    row_set_cell(r1, 1, "20");
    row_set_cell(r1, 2, "3.9");

    row_set_cell(r2, 0, "Bob");
    row_set_cell(r2, 1, "19");
    row_set_cell(r2, 2, "3.5");

    row_set_cell(r3, 0, "Carl");
    row_set_cell(r3, 1, "17");
    row_set_cell(r3, 2, "2.8");

    vec_push(rows, r1);
    vec_push(rows, r2);
    vec_push(rows, r3);

    return rows;
}


static void free_sample(Vec *original, Vec *filtered) {
    if (original != NULL) {
        size_t n = vec_length(original);
        for (size_t i = 0; i < n; i++) {
            Row *r = vec_get(original, i);
            if (r != NULL) {
                row_free(r);
            }
        }
        vec_free(original);
    }
    if (filtered != NULL) {
        vec_free(filtered);
    }
}

/* Test 1: NULL handling */
static void test_where_null_handling(void) {
    Vec *rows = NULL;
    Vec *res1 = where_filter(rows, "age>=18");
    TEST(res1 == NULL,
         "where_filter(NULL, cond) returns NULL",
         "where_filter(NULL, cond) should return NULL");

    Vec *sample = build_sample_rows();
    Vec *res2 = where_filter(sample, NULL);
    TEST(res2 == NULL,
         "where_filter(rows, NULL) returns NULL",
         "where_filter(rows, NULL) should return NULL");

    /* cleanup sample */
    if (sample != NULL) {
        size_t n = vec_length(sample);
        for (size_t i = 0; i < n; i++) {
            Row *r = vec_get(sample, i);
            if (r != NULL) row_free(r);
        }
        vec_free(sample);
    }

    printf("Test 1: NULL handling - Complete\n\n");
}

/* Test 1b: Empty rows Vec (length 0) */
static void test_where_empty_rows_vec(void) {
    Vec *rows = vec_new(0);
    TEST(rows != NULL, "vec_new(0) succeeds", "vec_new(0) failed");

    Vec *filtered = where_filter(rows, "age>=18");
    TEST(filtered == NULL,
         "where_filter() with empty rows returns NULL",
         "where_filter() with empty rows should return NULL");

    if (rows != NULL) {
        vec_free(rows);
    }

    printf("Test 1b: empty rows Vec - Complete\n\n");
}

/* Test 2: Numeric comparison age>=18 (should keep Alice and Bob) */
static void test_where_age_greater_equal_18(void) {
    Vec *rows = build_sample_rows();
    TEST(rows != NULL,
         "build_sample_rows() succeeded",
         "build_sample_rows() failed");

    Vec *filtered = where_filter(rows, "age>=18");
    TEST(filtered != NULL,
         "where_filter(age>=18) returned non-NULL",
         "where_filter(age>=18) returned NULL");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 3,
             "age>=18 keeps header + 2 rows",
             "age>=18 incorrect number of rows");

        if (n >= 3) {
            Row *r1 = vec_get(filtered, 1);
            Row *r2 = vec_get(filtered, 2);

            const char *name1 = row_get_cell(r1, 0);
            const char *name2 = row_get_cell(r2, 0);

            TEST(name1 != NULL && strcmp(name1, "Alice") == 0,
                 "First data row is Alice",
                 "First data row should be Alice");

            TEST(name2 != NULL && strcmp(name2, "Bob") == 0,
                 "Second data row is Bob",
                 "Second data row should be Bob");
        }
    }

    free_sample(rows, filtered);
    printf("Test 2: age>=18 - Complete\n\n");
}

/* Test 2b: age<=19 (keeps Bob and Carl) */
static void test_where_age_less_equal_19(void) {
    Vec *rows = build_sample_rows();
    Vec *filtered = where_filter(rows, "age<=19");
    TEST(filtered != NULL,
         "where_filter(age<=19) returned non-NULL",
         "where_filter(age<=19) returned NULL");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 3,
             "age<=19 keeps header + 2 rows",
             "age<=19 incorrect number of rows");

        if (n >= 3) {
            Row *r1 = vec_get(filtered, 1);
            Row *r2 = vec_get(filtered, 2);
            const char *name1 = row_get_cell(r1, 0);
            const char *name2 = row_get_cell(r2, 0);

            TEST(name1 != NULL && strcmp(name1, "Bob") == 0,
                 "First data row is Bob",
                 "First data row should be Bob");

            TEST(name2 != NULL && strcmp(name2, "Carl") == 0,
                 "Second data row is Carl",
                 "Second data row should be Carl");
        }
    }

    free_sample(rows, filtered);
    printf("Test 2b: age<=19 - Complete\n\n");
}

/* Test 2c: age>18 (keeps Alice and Bob) */
static void test_where_age_greater_than_18(void) {
    Vec *rows = build_sample_rows();
    Vec *filtered = where_filter(rows, "age>18");
    TEST(filtered != NULL,
         "where_filter(age>18) returned non-NULL",
         "where_filter(age>18) returned NULL");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 3,
             "age>18 keeps header + 2 rows",
             "age>18 incorrect number of rows");
    }

    free_sample(rows, filtered);
    printf("Test 2c: age>18 - Complete\n\n");
}

/* Test 2d: age<19 (keeps only Carl) */
static void test_where_age_less_than_19(void) {
    Vec *rows = build_sample_rows();
    Vec *filtered = where_filter(rows, "age<19");
    TEST(filtered != NULL,
         "where_filter(age<19) returned non-NULL",
         "where_filter(age<19) returned NULL");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 2,
             "age<19 keeps header + 1 row",
             "age<19 incorrect number of rows");

        if (n >= 2) {
            Row *r = vec_get(filtered, 1);
            const char *name = row_get_cell(r, 0);
            TEST(name != NULL && strcmp(name, "Carl") == 0,
                 "Data row is Carl",
                 "Data row should be Carl");
        }
    }

    free_sample(rows, filtered);
    printf("Test 2d: age<19 - Complete\n\n");
}

/* Test 3: String equality name==Bob */
static void test_where_name_equals_bob(void) {
    Vec *rows = build_sample_rows();
    Vec *filtered = where_filter(rows, "name==Bob");
    TEST(filtered != NULL,
         "where_filter(name==Bob) returned non-NULL",
         "where_filter(name==Bob) returned NULL");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 2,
             "name==Bob keeps header + 1 row",
             "name==Bob incorrect number of rows");

        if (n >= 2) {
            Row *r = vec_get(filtered, 1);
            const char *name = row_get_cell(r, 0);
            TEST(name != NULL && strcmp(name, "Bob") == 0,
                 "Data row is Bob",
                 "Data row should be Bob");
        }
    }

    free_sample(rows, filtered);
    printf("Test 3: name==Bob - Complete\n\n");
}

/* Test 3b: String inequality name!=Alice (keeps Bob and Carl) */
static void test_where_name_not_alice(void) {
    Vec *rows = build_sample_rows();
    Vec *filtered = where_filter(rows, "name!=Alice");
    TEST(filtered != NULL,
         "where_filter(name!=Alice) returned non-NULL",
         "where_filter(name!=Alice) returned NULL");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 3,
             "name!=Alice keeps header + 2 rows",
             "name!=Alice incorrect number of rows");
    }

    free_sample(rows, filtered);
    printf("Test 3b: name!=Alice - Complete\n\n");
}

/* Test 3c: Condition with extra spaces "  age   >=   19  " */
static void test_where_spaces_in_condition(void) {
    Vec *rows = build_sample_rows();
    Vec *filtered = where_filter(rows, "  age   >=   19  ");
    TEST(filtered != NULL,
         "where_filter() handles spaces in condition",
         "where_filter() failed with spaces in condition");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 3,
             "age>=19 with spaces keeps header + 2 rows",
             "age>=19 with spaces incorrect row count");
    }

    free_sample(rows, filtered);
    printf("Test 3c: spaces in condition - Complete\n\n");
}

/* Test 3d: Numeric column index: "1>=18" (col 1 is age) */
static void test_where_numeric_column_index(void) {
    Vec *rows = build_sample_rows();
    Vec *filtered = where_filter(rows, "1>=18");  // column 1 == "age"
    TEST(filtered != NULL,
         "where_filter(1>=18) returned non-NULL",
         "where_filter(1>=18) returned NULL");

    if (filtered != NULL) {
        size_t n = vec_length(filtered);
        TEST(n == 3,
             "1>=18 keeps header + 2 rows (same as age>=18)",
             "1>=18 incorrect number of rows");
    }

    free_sample(rows, filtered);
    printf("Test 3d: numeric column index - Complete\n\n");
}

/* Test 4: Invalid column name or operator should return NULL */
static void test_where_invalid_condition(void) {
    Vec *rows = build_sample_rows();

    Vec *res1 = where_filter(rows, "nosuchcol==1");
    TEST(res1 == NULL,
         "Invalid column returns NULL Vec",
         "Invalid column should return NULL Vec");

    Vec *res2 = where_filter(rows, "age??18");
    TEST(res2 == NULL,
         "Invalid operator returns NULL Vec",
         "Invalid operator should return NULL Vec");

    /* free only original rows */
    if (rows != NULL) {
        size_t n = vec_length(rows);
        for (size_t i = 0; i < n; i++) {
            Row *r = vec_get(rows, i);
            if (r != NULL) row_free(r);
        }
        vec_free(rows);
    }

    printf("Test 4: invalid conditions - Complete\n\n");
}

/* Test 4b: Missing RHS "age>" should be treated as invalid */
static void test_where_missing_rhs(void) {
    Vec *rows = build_sample_rows();
    Vec *res = where_filter(rows, "age>");
    TEST(res == NULL,
         "Condition with missing RHS returns NULL",
         "Condition with missing RHS should return NULL");

    if (rows != NULL) {
        size_t n = vec_length(rows);
        for (size_t i = 0; i < n; i++) {
            Row *r = vec_get(rows, i);
            if (r != NULL) row_free(r);
        }
        vec_free(rows);
    }

    printf("Test 4b: missing RHS - Complete\n\n");
}

int main(void) {
    printf("=== WHERE Unit Tests ===\n\n");

    test_where_null_handling();
    test_where_empty_rows_vec();

    test_where_age_greater_equal_18();
    test_where_age_less_equal_19();
    test_where_age_greater_than_18();
    test_where_age_less_than_19();

    test_where_name_equals_bob();
    test_where_name_not_alice();
    test_where_spaces_in_condition();
    test_where_numeric_column_index();

    test_where_invalid_condition();
    test_where_missing_rhs();

    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);

    return (tests_run == tests_passed) ? 0 : 1;
}
