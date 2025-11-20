/* Unit tests for sort.c
 * Tests basic single-column sorting using the sort_rows() function.
 * Ensures that rows are sorted correctly in ascending order based
 * on the specified column index.
 *
 * Vivek Patel, November 17, 2025, v0.0.2
 */

#include "../../include/sort.h"
#include "../../include/vec.h"
#include "../../include/row.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

// Unified test macro
#define TEST(cond, ok_msg, fail_msg) \
    do { \
        tests_run++; \
        if (cond) { \
            tests_passed++; \
            printf("PASS: %s\n", ok_msg); \
        } else { \
            printf("FAIL: %s\n", fail_msg); \
        } \
    } while (0)

// Utility: Make Row from comma-separated text 
static Row *make_row(const char *a, const char *b) {
    Row *r = row_new(2);
    row_set_cell(r, 0, a);
    row_set_cell(r, 1, b);
    return r;
}

//  Test 1: Numeric sorting
void test_sort_numeric_column(void) {
    Vec *rows = vec_new(4);
    vec_push(rows, make_row("Alice", "92"));
    vec_push(rows, make_row("Bob", "80"));
    vec_push(rows, make_row("Carol", "88"));
    vec_push(rows, make_row("Dave", "75"));

    sort_by_column(rows, 1);

    TEST(strcmp(row_get_cell(vec_get(rows, 0), 0), "Dave") == 0,
         "Numeric sort: row 0 correct",
         "Numeric sort: row 0 wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 1), 0), "Bob") == 0,
         "Numeric sort: row 1 correct",
         "Numeric sort: row 1 wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 2), 0), "Carol") == 0,
         "Numeric sort: row 2 correct",
         "Numeric sort: row 2 wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 3), 0), "Alice") == 0,
         "Numeric sort: row 3 correct",
         "Numeric sort: row 3 wrong");

    vec_free(rows);
    printf("Test 1 complete\n\n");
}

//  Test 2: Text sorting
void test_sort_text_column(void) {
    Vec *rows = vec_new(4);
    vec_push(rows, make_row("Charlie", "3"));
    vec_push(rows, make_row("Alice", "1"));
    vec_push(rows, make_row("Bob", "2"));
    vec_push(rows, make_row("David", "4"));

    sort_by_column(rows, 0);

    TEST(strcmp(row_get_cell(vec_get(rows, 0), 0), "Alice") == 0,
         "Text sort: row 0 correct",
         "Text sort: row 0 wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 1), 0), "Bob") == 0,
         "Text sort: row 1 correct",
         "Text sort: row 1 wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 2), 0), "Charlie") == 0,
         "Text sort: row 2 correct",
         "Text sort: row 2 wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 3), 0), "David") == 0,
         "Text sort: row 3 correct",
         "Text sort: row 3 wrong");

    vec_free(rows);
    printf("Test 2 complete\n\n");
}

//  Test 3: Empty vector should return empty vector
void test_sort_empty_vector(void) {
Vec *rows = vec_new(0);

    int result = sort_by_column(rows, 0);

    TEST(result == 0, "Empty vector: OK", "Empty vector: returned error");

    vec_free(rows);
    printf("Test 3 complete\n\n");
}

//  Test 4: NULL input should return NULL
void test_sort_null_input(void) {
    int result = sort_by_column(NULL, 0);

    TEST(result == -1, "NULL input handled", "NULL input not handled");
    printf("Test 4 complete\n\n");
}

//  Test 5: Out-of-bounds column index
void test_sort_out_of_bounds(void) {
    Vec *rows = vec_new(2);
    vec_push(rows, make_row("A", "B"));
    vec_push(rows, make_row("C", "D"));

    int result = sort_by_column(rows, 5);

    TEST(result == -1, "Out-of-bounds column rejected", "Out-of-bounds not detected");

    vec_free(rows);
    printf("Test 5 complete\n\n");
}

//  Test 6: NULL row inside vector
void test_sort_null_row_inside(void) {
    Vec *rows = vec_new(3);
    vec_push(rows, make_row("Z", "9"));
    vec_push(rows, NULL);
    vec_push(rows, make_row("A", "1"));

    int result = sort_by_column(rows, 0);

    TEST(result == -1, "NULL row inside rejected", "NULL row inside not handled");

    vec_free(rows);
    printf("Test 6 complete\n\n");
}

//  Test 7: Repeated values
void test_sort_repeated_values(void) {
    Vec *rows = vec_new(4);
    vec_push(rows, make_row("A", "10"));
    vec_push(rows, make_row("B", "10"));
    vec_push(rows, make_row("C", "10"));
    vec_push(rows, make_row("D", "10"));

    sort_by_column(rows, 1);

    TEST(strcmp(row_get_cell(vec_get(rows, 0), 0), "A") == 0,
         "Repeat values: A correct",
         "Repeat values: A wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 1), 0), "B") == 0,
         "Repeat values: B correct",
         "Repeat values: B wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 2), 0), "C") == 0,
         "Repeat values: C correct",
         "Repeat values: C wrong");

    TEST(strcmp(row_get_cell(vec_get(rows, 3), 0), "D") == 0,
         "Repeat values: D correct",
         "Repeat values: D wrong");

    vec_free(rows);
    printf("Test 7 complete\n\n");
}

//  Test 8: Single row
void test_sort_single_row(void) {
    Vec *rows = vec_new(1);
    vec_push(rows, make_row("Only", "999"));

    sort_by_column(rows, 1);

    TEST(strcmp(row_get_cell(vec_get(rows, 0), 0), "Only") == 0,
         "Single row unchanged",
         "Single row changed");

    vec_free(rows);
    printf("Test 8 complete\n\n");
}

// Main test driver
int main(void) {
    printf("=== Sort Unit Tests ===\n\n");

    test_sort_numeric_column();
    test_sort_text_column();
    test_sort_empty_vector();
    test_sort_null_input();
    test_sort_out_of_bounds();
    test_sort_null_row_inside();
    test_sort_repeated_values();
    test_sort_single_row();

    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);

    return (tests_run == tests_passed) ? 0 : 1;
}