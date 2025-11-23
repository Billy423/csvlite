/*
 * Provides basic WHERE filtering for CSVlite.
 * Supports single conditions like age>=18 or name==Alice.
 *
 * Increment 1: single simple condition (no && / ||).
 *
 * AUTHOR: Nadeem Mohamed
 * DATE: November 17, 2025
 * VERSION: v0.1.0
 */

#include "../include/where.h"
#include "../include/row.h"
#include "../include/vec.h"
#include <stdlib.h>
#include <string.h>

#define OP_EQ 1
#define OP_NE 2
#define OP_LT 3
#define OP_LE 4
#define OP_GT 5
#define OP_GE 6

/*
 * duplicate a string using malloc.
 * RETURN: new string or NULL on failure.
 */
static char *dupstr(const char *text) {
    if (text == NULL) {
        return NULL;
    }

    int length = (int)strlen(text);
    char *copy = malloc(length + 1);
    if (copy == NULL) {
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        copy[i] = text[i];
    }
    copy[length] = '\0';
    return copy;
}

/*
 * trim leading/trailing spaces and tabs (in place).
 * RETURN: pointer to the trimmed string (inside the same buffer).
 */
static char *trim_spaces(char *s) {
    if (s == NULL) return NULL;

    /* move start to first non-space/tab */
    while (*s == ' ' || *s == '\t') {
        s++;
    }

    int len = (int)strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }

    return s;
}

/*
 * Helper: check if text is a non-empty string of digits (0-9).
 * Used for numeric column indices like "0", "1", etc.
 */
static int is_number(const char *text) {
    if (text == NULL || *text == '\0') return 0;

    int i = 0;
    while (text[i] != '\0') {
        if (text[i] < '0' || text[i] > '9') {
            return 0;
        }
        i++;
    }
    return 1;
}

/*
 * Parse a condition of the form:
 *   <column><op><value>
 * where <op> is one of: ==, !=, >=, <=, >, <
 * RETURN: 0 on success, -1 on failure.
 */
static int parse_condition(
    const char *condition,
    char **out_col_name,
    char **out_value_str,
    int *out_op_type
) {
    if (condition == NULL || out_col_name == NULL || out_value_str == NULL || out_op_type == NULL) {
        return -1;
    }

    char *copy = dupstr(condition);
    if (copy == NULL) return -1;

    const char *op_pos = NULL;
    int op_len = 0;
    int op_type = 0;

    const char *p;

    p = strstr(copy, "==");
    if (p != NULL) {
        op_pos = p;
        op_len = 2;
        op_type = OP_EQ;
    }
    if (op_pos == NULL) {
        p = strstr(copy, "!=");
        if (p != NULL) {
            op_pos = p;
            op_len = 2;
            op_type = OP_NE;
        }
    }
    if (op_pos == NULL) {
        p = strstr(copy, ">=");
        if (p != NULL) {
            op_pos = p;
            op_len = 2;
            op_type = OP_GE;
        }
    }
    if (op_pos == NULL) {
        p = strstr(copy, "<=");
        if (p != NULL) {
            op_pos = p;
            op_len = 2;
            op_type = OP_LE;
        }
    }

    // if still not found, try one-character operators: '>' or '<'
    if (op_pos == NULL) {
        p = strchr(copy, '>');
        if (p != NULL) {
            op_pos = p;
            op_len = 1;
            op_type = OP_GT;
        }
    }
    if (op_pos == NULL) {
        p = strchr(copy, '<');
        if (p != NULL) {
            op_pos = p;
            op_len = 1;
            op_type = OP_LT;
        }
    }

    if (op_pos == NULL || op_len == 0) {
        free(copy);
        return -1;  
    }

    //split into left and right parts
    int left_len = (int)(op_pos - copy);
    int right_len = (int)strlen(copy) - left_len - op_len;

    if (left_len <= 0 || right_len <= 0) {
        free(copy);
        return -1;
    }

    char *left = malloc(left_len + 1);
    char *right = malloc(right_len + 1);
    if (left == NULL || right == NULL) {
        free(left);
        free(right);
        free(copy);
        return -1;
    }

    //copy left side 
    for (int i = 0; i < left_len; i++) {
        left[i] = copy[i];
    }
    left[left_len] = '\0';

    // copy right side 
    const char *right_start = op_pos + op_len;
    for (int i = 0; i < right_len; i++) {
        right[i] = right_start[i];
    }
    right[right_len] = '\0';

    free(copy);

    //trim spaces around both tokens
    char *trim_left = trim_spaces(left);
    char *trim_right = trim_spaces(right);

    char *final_left = dupstr(trim_left);
    char *final_right = dupstr(trim_right);

    free(left);
    free(right);

    if (final_left == NULL || final_right == NULL) {
        free(final_left);
        free(final_right);
        return -1;
    }

    *out_col_name = final_left;
    *out_value_str = final_right;
    *out_op_type = op_type;
    return 0;
}

/*
 * Given the header row and a column token, find the column index.
 * column_token can be:
 *   - a number: "0", "1", ...
 *   - a header name: "age", "name", ...
 *
 * RETURN: column index >= 0 on success, -1 on failure.
 */
static int find_column_index(const Row *header, const char *column_token) {
    if (header == NULL || column_token == NULL) return -1;

    int ncols = row_num_cells(header);
    if (ncols <= 0) return -1;

    //numeric index
    if (is_number(column_token)) {
        int idx = atoi(column_token);
        if (idx < 0 || idx >= ncols) return -1;
        return idx;
    }

    //name-based lookup
    for (int i = 0; i < ncols; i++) {
        const char *name = row_get_cell(header, i);
        if (name != NULL && strcmp(name, column_token) == 0) {
            return i;
        }
    }

    return -1;  
}

/*
 * Compare one cell value against the right-hand side of the condition.
 *
 * If the operator is == or !=, do a string comparison.
 * If the operator is <, <=, >, >=
 *
 * RETURN: 1 if the condition is satisfied, 0 otherwise.
 */
static int matches_condition(
    const char *cell_value,
    const char *rhs_value,
    int op_type
) {
    if (cell_value == NULL) cell_value = "";
    if (rhs_value == NULL) rhs_value = "";

    if (op_type == OP_EQ) {
        return strcmp(cell_value, rhs_value) == 0;
    }
    if (op_type == OP_NE) {
        return strcmp(cell_value, rhs_value) != 0;
    }


    double left_num = atof(cell_value);
    double right_num = atof(rhs_value);

    if (op_type == OP_LT) {
        return left_num < right_num;
    }
    if (op_type == OP_LE) {
        return left_num <= right_num;
    }
    if (op_type == OP_GT) {
        return left_num > right_num;
    }
    if (op_type == OP_GE) {
        return left_num >= right_num;
    }

    return 0;
}

/*
 * Filters rows using a single WHERE condition, like "age>=18" or "name==Alice".
 *
 * MEMORY OWNERSHIP:
 * - Returns a new Vec* that the caller must free with vec_free()
 * - Reuses Row* pointers from input (does NOT copy Row objects)
 * - Caller must free Row objects separately (they are shared with input Vec)
 * - Does NOT free the input Vec or Row objects
 *
 * PARAMETERS:
 *  - rows: vector of Row*, row 0 is assumed to be the header row
 *  - condition: where expression from CLI (format: "<column><op><value>")
 *               used operators: ==, !=, >=, <=, >, <
 *
 * RETURNS:
 *  - new Vec* containing header + only matching data rows
 *  - NULL on failure (bad condition, memory issues, etc.)
 */
Vec *where_filter(const Vec *rows, const char *condition) {
    if (rows == NULL || condition == NULL || *condition == '\0') {
        return NULL;
    }

    size_t total_rows = vec_length(rows);
    if (total_rows == 0) {
        return NULL;
    }

    //Parse condition into (column token, op type, rhs value)
    char *col_token = NULL;
    char *rhs_value = NULL;
    int op_type = 0;

    if (parse_condition(condition, &col_token, &rhs_value, &op_type) != 0) {
        return NULL;
    }

    //Get header row
    const Row *header = vec_get(rows, 0);
    if (header == NULL) {
        free(col_token);
        free(rhs_value);
        return NULL;
    }

    int col_index = find_column_index(header, col_token);
    if (col_index < 0) {
        free(col_token);
        free(rhs_value);
        return NULL;
    }

    //Prepare result vector: at most total_rows rows
    Vec *result = vec_new(total_rows);
    if (result == NULL) {
        free(col_token);
        free(rhs_value);
        return NULL;
    }

    //Always keep the header as the first row 
    Row *header_row = vec_get(rows, 0);
    vec_push(result, header_row);

    //Filter the remaining rows 
    for (size_t i = 1; i < total_rows; i++) {
        Row *r = vec_get(rows, i);
        const char *cell = row_get_cell(r, col_index);

        if (matches_condition(cell, rhs_value, op_type)) {
            vec_push(result, r);   
        }
    }

    free(col_token);
    free(rhs_value);
    return result;
}
