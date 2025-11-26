/*
* Tests to verify command-line interface parses arguments as expected.
*
*
* AUTHOR: Nikhil Ranjith
* DATE: November 10, 2025
* VERSION: v0.0.1
*/
 
#include "../../include/cli.h"
#include <stdio.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(cond, success_msg, fail_msg) \
    do { \
        tests_run++; \
        if (cond) { \
            tests_passed++; \
            printf("PASS: %s\n", success_msg); \
        } else { \
            printf("FAIL: %s\n", fail_msg); \
        } \
    } while(0)

// Test 1: No arguments, should keep defaults
void test_cli_defaults(void) {
    cli_init();
    char* argv[] = { "csvlite" };
    int argc = 1;

    int result = cli_parse_args(argc, argv);
    TEST(result == 1, "cli_parse_args returns 1 for default args", "cli_parse_args failed default args");

    TEST(g_file_path == NULL, "g_file_path is NULL by default", "g_file_path not NULL");
    TEST(g_select_cols == NULL, "g_select_cols is NULL by default", "g_select_cols not NULL");
    TEST(g_where_cond == NULL, "g_where_cond is NULL by default", "g_where_cond not NULL");
    TEST(g_use_stdin == 0, "g_use_stdin is 0 by default", "g_use_stdin not 0");
    TEST(g_help_flag == 0, "g_help_flag is 0 by default", "g_help_flag not 0");
}

// Test 2: File argument
void test_cli_file(void) {
    cli_init();
    char* argv[] = { "csvlite", "--file", "data.csv" };
    int argc = 3;

    int result = cli_parse_args(argc, argv);
    TEST(result == 1, "--file parsed successfully", "Failed to parse --file");
    TEST(g_file_path != NULL && strcmp(g_file_path, "data.csv") == 0,
         "g_file_path set correctly", "g_file_path incorrect");
}

// Test 3: Select argument
void test_cli_select(void) {
    cli_init();
    char* argv[] = { "csvlite", "--select", "name,age" };
    int argc = 3;

    cli_parse_args(argc, argv);
    TEST(g_select_cols != NULL && strcmp(g_select_cols, "name,age") == 0,
         "g_select_cols set correctly", "g_select_cols incorrect");
}

// Test 4: Where argument
// void test_cli_where(void) {
//     cli_init();
//     char* argv[] = { "csvlite", "--where", "age>=18" };
//     int argc = 3;

//     cli_parse_args(argc, argv);
//     TEST(g_where_cond != NULL && strcmp(g_where_cond, "age>=18") == 0,
//          "g_where_cond set correctly", "g_where_cond incorrect");
// }

// Test 5: Stdin flag
void test_cli_stdin(void) {
    cli_init();
    char* argv[] = { "csvlite", "-" };
    int argc = 2;

    cli_parse_args(argc, argv);
    TEST(g_use_stdin == 1, "g_use_stdin set to 1 with -", "g_use_stdin not set");
}

// Test 6: Help flag
void test_cli_help(void) {
    cli_init();
    char* argv[] = { "csvlite", "--help" };
    int argc = 2;

    int result = cli_parse_args(argc, argv);
    TEST(result == -1, "cli_parse_args returns -1 with --help", "cli_parse_args did not return -1 with --help");
    TEST(g_help_flag == 1, "g_help_flag set with --help", "g_help_flag not set with --help");
}

// Test 7: Group-by and order-by arguments
void test_cli_group_and_order(void) {
    cli_init();
    char* argv[] = { "csvlite", "--group-by", "city", "--order-by", "age:desc" };
    int argc = 5;

    int result = cli_parse_args(argc, argv);
    TEST(result == 1, "cli_parse_args handles --group-by and --order-by", "cli_parse_args failed for group/order");
    TEST(g_group_by_col != NULL && strcmp(g_group_by_col, "city") == 0,
         "g_group_by_col set correctly", "g_group_by_col incorrect");
    TEST(g_order_by_col != NULL && strcmp(g_order_by_col, "age:desc") == 0,
         "g_order_by_col set correctly", "g_order_by_col incorrect");
}

// Test 8: Missing required value for option
void test_cli_missing_file_value(void) {
    cli_init();
    char* argv[] = { "csvlite", "--file" };
    int argc = 2;

    int result = cli_parse_args(argc, argv);
    TEST(result == 0, "cli_parse_args returns 0 when --file value missing", "cli_parse_args did not fail without file value");
    TEST(g_file_path == NULL, "g_file_path remains NULL on parse error", "g_file_path unexpectedly set on error");
}

// Test 9: Unknown argument handling
void test_cli_unknown_argument(void) {
    cli_init();
    char* argv[] = { "csvlite", "--unknown" };
    int argc = 2;

    int result = cli_parse_args(argc, argv);
    TEST(result == 0, "cli_parse_args returns 0 for unknown argument", "cli_parse_args did not fail for unknown argument");
}

// Test 10: Cleanup resets pointers
void test_cli_cleanup(void) {
    cli_init();
    char* argv[] = { "csvlite", "--file", "data.csv", "--select", "name" };
    int argc = 5;

    int result = cli_parse_args(argc, argv);
    TEST(result == 1, "cli_parse_args succeeded before cleanup", "cli_parse_args failed before cleanup");

    cli_cleanup();
    TEST(g_file_path == NULL && g_select_cols == NULL && g_where_cond == NULL &&
         g_group_by_col == NULL && g_order_by_col == NULL,
         "cli_cleanup resets option pointers", "cli_cleanup did not reset option pointers");
}

int main(void) {
    printf("=== CLI Unit Tests ===\n\n");

    test_cli_defaults();
    test_cli_file();
    test_cli_select();
    // test_cli_where();
    test_cli_stdin();
    test_cli_help();
    test_cli_group_and_order();
    test_cli_missing_file_value();
    test_cli_unknown_argument();
    test_cli_cleanup();

    printf("\n=== Test Summary ===\n");
    printf("CLI Tests run: %d\n", tests_run);
    printf("CLI Tests passed: %d\n", tests_passed);
    printf("CLI Tests failed: %d\n", tests_run - tests_passed);

    return (tests_run == tests_passed) ? 0 : 1;
}
