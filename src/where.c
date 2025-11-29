/*
 * Provides WHERE filtering
 * Supports single conditions like age>=18 or name==Alice.
 * where <column> can be a numeric column index ("0", "1", …) or a header
 * name ("age", "name", …), and <op> is one of: ==, !=, >=, <=, >, <.
 * The module parses the condition, locates the target column, and returns
 * a new Vec* containing only the filetered rows
 * 
 * AUTHOR: Nadeem Mohamed
 * DATE: November 17, 2025
 * VERSION: v2.0.0
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
 *  Function to duplicate the string
 *  If text is NULL, the function returns NULL and does not allocate.
 *  Otherwise, it allocates (strlen(text) + 1) bytes, copies the characters,
 *   and appends a '\0' terminator.
 *
 * parameters:
 *  text: source string to copy (may be NULL).
 *
 * RETURN: a pointer to a newly allocated copy of text on success,
 *         or NULL if text is NULL or allocation fails.
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
 *  Advances past leading ' ' and '\t' characters by moving the returned
 *   pointer into the original buffer.
 *  Then walks backwards from the end of the string, replacing trailing
 *   spaces/tabs with '\0'.
 *  If the string is all whitespace, the function returns a pointer to
 *   the '\0' terminator inside the original buffer.
 *
 * parameters:
 *  s: modifiable C string buffer to trim (may be NULL).
 *
 * RETURN: pointer to the first non-space/tab character inside s,
 *         or NULL if s is NULL.
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
 *  NULL or empty strings are treated as "not a number".
 *  Every character must be between '0' and '9'; any other character
 *   causes failure.
 *
 * Used for numeric column indices like "0", "1", etc.
 *
 * parameters:
 *  text: string to test (may be NULL).
 *
 * RETURN: 1 if text is non-empty and all characters are digits,
 *         0 otherwise.
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
 *  Parse a condition string of the form:
 *     <column><op><value>
 *  where <op> is one of: ==, !=, >=, <=, >, <.
 *  Makes a temporary copy of the condition string so it can search and
 *   split it safely.
 *  Searches in order for two-character operators (==, !=, >=, <=) and
 *   then one-character operators (>, <).
 *  Trims spaces/tabs around each part.
 *  Allocates fresh strings for out_col_name and out_value_str; these are
 *   independent of the original condition buffer.
 *
 * parameters:
 *  condition: original condition string from the CLI.
 *  out_col_name: output; set to a newly allocated column token string.
 *  out_value_str: output; set to a newly allocated right-hand-side string.
 *  out_op_type: output; set to one of OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE.
 *
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
 * column_token can be either:
 *     a number: "0", "1", ... (treated as zero-based index), or
 *     a header name: "age", "name", ...
 * For numeric tokens, the function checks that the index is within bounds.
 * For name-based tokens, it scans the header row cells and compares each
 *   cell string to column_token using strcmp().
 *
 * parameters:
 * - header: pointer to the header Row (row 0 of the dataset).
 * - column_token: column designator from the condition string.
 *
 * RETURN: column index (>= 0) on success, or -1 if the token is invalid
 *         or the name cannot be found.
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
 * cell_value and rhs_value are treated as empty strings if NULL.
 * For == and !=, a plain string comparison is used.
 * For <, <=, >, >=, both strings are converted to double 
 *
 * parameters:
 *  cell_value: value from the CSV row for the target column.
 *  rhs_value:  right-hand-side constant from the condition.
 *  op_type:    operator type (one of OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE).
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
 *  Filters rows using a single WHERE condition, like "age>=18" or "name==Alice".
 *  The input rows Vec is treated as a table where:
 *  First, the function parses the condition string into:
 *      a column token (index or header name),
 *      an operator type, and
 *      a right-hand-side value.
 *  It then resolves the column token to a column index using the header row.
 *  A new Vec is allocated to store the result. The header row is always
 *   included as the first element in the result.
 *  For each data row, the function pulls the cell in the target column
 *   and checks whether it satisfies the condition using matches_condition().
 *  Matching rows are appended to the result Vec.
 *  The result Vec shares Row* pointers with the original rows; it does not
 *   allocate or copy Row objects.
 *
 * parameters:
 *  rows:      Vec* of Row* representing the full dataset; row 0 is header.
 *  condition: condition string of the form "<column><op><value>" with
 *              operators ==, !=, >=, <=, >, <.
 *
 * RETURN:
 *  A new Vec* containing the header row plus only those data rows that
 *   satisfy the condition, on success.
 *  NULL on failure (invalid condition, column not found, or allocation error).
 *
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
