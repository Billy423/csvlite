
/*
 * Provides functions for selecting specific columns from a CSV dataset.
 * This module parses a user’s --select argument and returns the list of column indices. 
 * It can also build a new vector of rows that includes only those columns.
 *
 * AUTHOR: Nadeem Mohamed
 * DATE: November 10, 2025
 * VERSION: v0.0.1
*/

#include "../include/select.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * duplicates a string by allocating memory for it. 
 * RETURN: Pointer to a new string or NULL if allocation fails
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
 * removes leading and trailing spaces, tabs, newline characters from a string
 * RETURN: the modified string
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
 * checks if a string contains only numeric characters (0–9).
 * RETURN: 1 if numeric, 0 otherwise.
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
 * parses a comma-separated column list and converts it into
 * an integer array of column indices.
 *
 * parameters:
 *  - columns_spec: user string, e.g., "0,2,4" or "name,age"
 *  - name_to_index: optional map of column names to indices
 *  - total_cols: total number of columns in the dataset
 *  - out_indices: pointer where parsed indices will be stored
 *  - out_count: pointer where the number of indices is stored
 *
 * RETURN: 0 on success, -1 on failure.
 */
int select_parse_indices(const char *columns_spec, const HMap *name_to_index, int total_cols, int **out_indices, int *out_count){
    if(columns_spec == NULL || out_indices == NULL || out_count == NULL){
        return -1;
    }

    *out_indices = NULL;
    *out_count = 0;

    char *copy = dupstr(columns_spec);

    if(copy == NULL){
        return -1;
    }
    // count how many columns based on commas
    int count = 1;
    int i =0;
    while(copy[i] != '\0'){
        if(copy[i] == ','){
            count++;
        }
        i++;
    }

    int *indices = malloc(sizeof(int) * count);
    if(indices == NULL){
        free(copy);
        return -1;
    }

    int n =0;
    char *token = strtok(copy, ",");
    while(token != NULL){
        char *word = trim(token);
        int columnn_index = -1;

        if(is_number(word)){ //numeric index
            columnn_index = atoi(word);
            if(total_cols >= 0 && columnn_index >= total_cols){
                free(indices);
                free(copy);
                return -1;
            }

        }
        else{   //name lookup
            if(name_to_index == NULL){
                free(indices);
                free(copy);
                return -1;
            }

            void *value = hmap_get(name_to_index, word);
            if(value == NULL){
                free(indices);
                free(copy);
                return -1;
            }
            columnn_index = (int)(long)value;
        }
        indices[n] = columnn_index;
        n++;
        token = strtok(NULL, ",");
    }

    *out_indices = indices;
    *out_count = n;
    free(copy);
    return 0;
}


/* 
 * builds a new Vec* of rows containing only the selected columns.
 * similar to the SQL "SELECT column1, column2" operation.
 *
 * parameters:
 *  - rows: vector of all rows in the dataset
 *  - indices: array of column indices to include
 *  - n_indices: number of selected columns
 *
 * RETURN: a new Vec* of projected rows, or NULL on error
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