#include "../include/select.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        if(is_number(word)){
            columnn_index = atoi(word);
            if(total_cols >= 0 && columnn_index >= total_cols){
                free(indices);
                free(copy);
                return -1;
            }

        }
        else{
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