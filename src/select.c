/*
 * Provides functions for selecting specific columns from a CSV dataset.
 * This module parses a user’s --select argument and returns the list of column indices. 
 * These indices can be specified as numeric positions or as column names that are resolved to a hashmap
 * It can also build a new vector of rows that includes only those columns.
 *
 * AUTHOR: Nadeem Mohamed
 * DATE: November 10, 2025
 * VERSION: v2.0.0
*/

#include "../include/select.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Behavior details:
 *  If text is NULL, this function returns NULL and does not allocate.
 *  Otherwise, it allocates (strlen(text) + 1) bytes, copies the contents,
 *   and appends a '\0' terminator.
 *  The caller owns the returned buffer and must free() it when done.
 *
 * parameters:
 *  text: source string to duplicate (may be NULL).
 *
 * RETURN: pointer to a newly allocated copy of text on success,
 *         or NULL if text is NULL or memory allocation fails.
 */
static char *dupstr(const char *text){
    if(text == NULL){
        return NULL;
    }

    int length = strlen(text);
    char *copy = malloc(length+1);

    if(copy == NULL){
        return NULL;
    }

    for(int i =0;i<length;i++){
        copy[i] = text[i];
    }

    copy[length] = '\0';
    return copy;
}

/*
 * Behavior details:
 *  Leading spaces, tabs, and newline-style characters (' ', '\t', '\n', '\r')
 *   are skipped by advancing the returned pointer into the original buffer.
 *  Trailing whitespace is removed in-place by overwriting the last
 *   non-whitespace character with '\0'.
 *  If the string is entirely whitespace, the function returns a pointer
 *   to the '\0' terminator
 *
 * parameters:
 *  s: modifiable C string buffer to trim (may be NULL).
 *
 * RETURN: pointer to the first non-whitespace character inside s.
 */
static char* trim(char *s){
    if(s == NULL){
        return s;
    }

    while(*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r'){
        s++;
    }

    int len = strlen(s);
    while(len>0 && (s[len-1] == ' ' || s[len-1] == '\t' || s[len-1] == '\n' || s[len-1] == '\r')){
        len--;
    }

    s[len] = '\0';
    return s;
}


/* 
* Behavior details:
 *  An empty string or NULL pointer is considered "not a number".
 *  For a non-empty string, the function scans each character and verifies
 *   that it is between '0' and '9'. Any other character causes failure.
 *
 * parameters:
 *  text: string to test (may be NULL).
 *
 * RETURN: 1 if text is non-empty and every character is a digit '0'–'9',
 *         0 otherwise.
 */
static int is_number(const char *text){
    if(text == NULL || *text == '\0'){
        return 0;
    }
    int i =0;
    while(text[i] != '\0'){
        if(text[i]<'0' || text[i]>'9'){
            return 0;
        }
        i++;

    }
    return 1;
}

/* 
 * Behavior details:
 *  Each token is trimmed of leading/trailing whitespace.
 *  If a token is numeric, it is treated as a zero-based index.
 *  If a token is not numeric, it is treated as a column name. 
 *   If lookup fails, parsing fails.
 *  On any error (bad parameters, out-of-range index, missing name, or
 *   allocation failure), this function cleans up and returns -1.
 *  On success, it allocates an int array of size *out_count and fills it with
 *   the parsed indices in order.
 *
 * parameters:
 *  columns_spec: user-supplied string describing columns; must be non-NULL
 *  name_to_index: optional hash map from column names to 
 *  total_cols: total number of columns in the dataset
 *  out_indices: output index
 *  out_count:   output count
 * RETURN: 0 on success, or -1
 */
int select_parse_indices(const char *columns_spec, const HMap *name_to_index, int total_cols, int **out_indices, int *out_count){
    if (columns_spec == NULL || out_indices == NULL || out_count == NULL) {
        return -1;
    }

    *out_indices = NULL;
    *out_count = 0;

    char *copy = dupstr(columns_spec);
    if (copy == NULL) {
        return -1;
    }

    // count how many columns based on commas
    int count = 1;
    for (int i = 0; copy[i] != '\0'; i++) {
        if (copy[i] == ',') count++;
    }

    int *indices = malloc(sizeof(int) * count);
    if (indices == NULL) {
        free(copy);
        return -1;
    }

    int n = 0;
    char *token = strtok(copy, ",");
    while (token != NULL) {
        char *word = trim(token);
        int columnn_index = -1;

        if (is_number(word)) { // numeric index
            columnn_index = atoi(word);
            if (total_cols >= 0 && columnn_index >= total_cols) {
                free(indices);
                free(copy);
                return -1;
            }
        } else { // name lookup
            if (name_to_index == NULL) {
                free(indices);
                free(copy);
                return -1;
            }

            void *value = hmap_get(name_to_index, word);
            if (value == NULL) {
                free(indices);
                free(copy);
                return -1;
            }

            columnn_index = (int)((long)value - 1);
            if (total_cols >= 0 && columnn_index < 0) {
                free(indices);
                free(copy);
                return -1;
            }
        }

        indices[n++] = columnn_index;
        token = strtok(NULL, ",");
    }

    *out_indices = indices;
    *out_count = n;
    free(copy);
    return 0;
}


/* 
 * Behavior details:
 *  The input rows Vec is treated as a sequence of Row* values.
 *  For each source row, this function creates a new Row with n_indices
 *   columns and copies the selected cells into it in the order specified
 *   by indices[].
 *  If any memory allocation fails or row_set_cell() returns an error, the
 *   function frees all partially created rows, and returns NULL to signal failure.
 *
 * parameters:
 *  rows:      Vec* of Row* representing the full dataset (must be non-NULL).
 *  indices:   array of column indices specifying which columns to keep.
 *  n_indices: number of entries in indices; must be > 0.
 *
 * RETURN: a new Vec* containing newly allocated Row* objects representing
 *         the projected table on success, or NULL on error.
 */
Vec *select_project_rows(const Vec *rows, const int *indices, int n_indices){
    if (rows == NULL || indices == NULL || n_indices <= 0){
        return NULL;
    }
    int total_rows = vec_length(rows);

    Vec *result = vec_new(total_rows);

    if(result == NULL){
        return NULL;
    }

    for(int i= 0; i<total_rows; i++){
        const Row *src = vec_get(rows,i);
        Row *dst = row_new(n_indices);
        if(dst == NULL){
            vec_free(result);
            return NULL;
        }

        for(int j = 0; j <n_indices; j++){
            const char *cell = row_get_cell(src, indices[j]);
            if (row_set_cell(dst, j, cell) != 0) {
                row_free(dst);
                vec_free(result);
                return NULL;
            }
        }
        vec_push(result, dst);
    }
    return result;

}