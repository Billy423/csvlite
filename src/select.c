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
 * If the text is NUll, this function returns NUll and does not allocate. 
 * Otherwise, it allcoates bytes, copies the contents and appends a end line character.
 * 
 * Parameters:
 *  text: string to duplicate
 * 
 * RETURN: pointer to a newly allcoated duplicate on success or NULL on failure
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
 * Trims leading spaces, tabs, and newline characters. Trailing whitespace characters is removed 
 * in-place. If the entire string is whitespace, the function returns a poitner to the '\0' terminator
 * 
 * Parameters: 
 *  s: string to modify
 * 
 * Returns: pointer to the first non-whitespace character
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
 * Checks if the input is a number ('0' to '9').
 * 
 * Parameters: 
 *  text: string to test
 * 
 * Returns: 1 if text is non-empty and it is a number or 0 otherwise
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
 * Performs lookup and allocates an int array of size *out_count and fills it with parsed indices
 * in order.  
 * Parameters:
 *  columns_spec: input string for the columns to lookup
 *  name_to_index: hasmap from column names
 *  total_cols: total number of columns
 *  out_indices: output index
 *  out_count: output count
 * 
 * Returns: 0 on success, or -1
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
 * Creates a new set of rows that include only selected columns. It creates a new row 
 * containing only the columns listed in 'indices' in the same order.
 * 
 * Parameters:
 *  rows: Vec* of Row* representing the entire dataset
 *  indices: array of column indices that are selected
 *  n_indices: number of indexes
 * 
 *  Returns: A new Vec* containing newly allocated Row* 
 * 
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