/*
 * Parses command line arguments and stores them for the CSVlite tool.
 * Supports file/stdin input, column selection, filtering, grouping, and sorting.
 * --order-by accepts "col", "col:asc", "col:desc", or numeric indices (e.g., 1:desc).
 * --group-by accepts column names or numeric indices. "-" enables stdin.
 *
 * AUTHOR: Nikhil Ranjith
 * DATE: November 30, 2025
 * VERSION: v2.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cli.h"

char* g_file_path = NULL;
char* g_select_cols = NULL;
char* g_where_cond = NULL;
int g_help_flag = 0;
int g_use_stdin = 0;
char* g_group_by_col = NULL;
char* g_order_by_col = NULL;

/*
 * Resets all CLI option globals to their default unset state.
 * Parameters: none
 * Returns: void
 * Side effects: clears previously parsed argument values.
 */
void cli_init(void) {
    g_file_path = NULL;
    g_select_cols = NULL;
    g_where_cond = NULL;
    g_help_flag = 0;
    g_use_stdin = 0;
    g_group_by_col = NULL;
    g_order_by_col = NULL;
}

/*
 * Prints usage information for the CSVlite command line tool.
 * Parameters: none
 * Returns: void
 * Side effects: writes help text to stdout.
 */
void cli_print_help(void) {
    printf("CSVlite - Command-Line CSV Analytics Tool\n");
    printf("\n");
    printf("Usage: csvlite [--file <file> | -] [options]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --file <file>     CSV file to process (or use - for stdin)\n");
    printf("  --select <cols>   Columns to select (e.g. name,age or 0,1)\n");
    printf("  --where <cond>    Filter condition (e.g. age>=18)\n");
    printf("  --group-by <col>  Column name or index to group by (e.g. department or 2)\n");
    printf("  --order-by <col>  Column to order by; supports name or index, optional :asc/:desc (defaults asc)\n");
    printf("  --help            Show this help message\n");
    printf("\n");
    printf("Examples:\n");
    printf("  csvlite --file data.csv --select name,age\n");
    printf("  csvlite --file data.csv --where 'age>=18' --order-by age:desc\n");
    printf("  csvlite - < data.csv              # Read from stdin\n");
    printf("  cat data.csv | csvlite -          # Pipe input\n");
    printf("\n");
}

/*
 * Parses command line arguments and populates module level option variables.
 * Parameters: argc (argument count)
 *             argv (argument vector)
 * Returns: 1 on success
 *          0 on invalid input
 *          -1 when --help is requested
 * Side effects: sets global pointers/flags used by the application. Does not allocate.
 */
int cli_parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "--help") == 0) {
            g_help_flag = 1;
            cli_print_help();
            return -1;
        }

        else if (strcmp(argv[i], "--file") == 0) {
            if (++i < argc) {
                g_file_path = argv[i];
            } else {
                fprintf(stderr, "Error: --file requires a file path\n");
                return 0;
            }
        }
        else if (strcmp(argv[i], "--select") == 0) {
            if (++i < argc) {
                g_select_cols = argv[i];
            } else {
                fprintf(stderr, "Error: --select requires a column list\n");
                return 0;
            }
        }
        else if (strcmp(argv[i], "--where") == 0) {
            if (++i < argc) {
                g_where_cond = argv[i];
            } else {
                fprintf(stderr, "Error: --where requires a condition\n");
                return 0;
            }
        }
        else if (strcmp(argv[i], "--group-by") == 0) {
            if (++i < argc) {
                g_group_by_col = argv[i];
            } else {
                fprintf(stderr, "Error: --group-by requires a column\n");
                return 0;
            }
        }
        else if (strcmp(argv[i], "--order-by") == 0) {
            if (++i < argc) {
                g_order_by_col = argv[i];
            } else {
                fprintf(stderr, "Error: --order-by requires a column\n");
                return 0;
            }
        }
        else if (strcmp(argv[i], "-") == 0) {
            g_use_stdin = 1;
        }
        else {
            fprintf(stderr, "Error: Unknown argument '%s'\n", argv[i]);
            return 0;
        }
    }
    return 1;
}

/*
 * Clears parsed CLI option pointers after use.
 * Parameters: none
 * Returns: void
 * Side effects: resets globals to NULL/defaults without freeing memory.
 */
void cli_cleanup(void) {
    g_file_path = NULL;
    g_select_cols = NULL;
    g_where_cond = NULL;
    g_group_by_col = NULL;
    g_order_by_col = NULL;
}
