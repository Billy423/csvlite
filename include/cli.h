/*
* AUTHOR: Nikhil Ranjith
* DATE: November 30, 2025
* VERSION: v2.0.0
*
* Parses CLI options for CSVlite and exposes globals for the parsed values.
* Supported forms:
*   --file <path> | - (stdin)
*   --select name,age or numeric indices (0,2)
*   --where expressions like age>=18
*   --group-by <name|index>
*   --order-by <name|index[:asc|:desc]> (defaults to asc)
*/

#ifndef CLI_H
#define CLI_H

int cli_parse_args(int argc, char* argv[]);
void cli_init(void);
void cli_cleanup(void);
void cli_print_help(void);

// Global variables
extern char* g_file_path;
extern char* g_select_cols;
extern char* g_where_cond;
extern int g_help_flag;
extern int g_use_stdin;
extern char* g_group_by_col;
extern char* g_order_by_col;

#endif
