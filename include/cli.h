/*
* AUTHOR: Nikhil Ranjith
* DATE: November 10, 2025
* VERSION: v0.0.1
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

#endif
