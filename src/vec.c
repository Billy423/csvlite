/**
 * vec.c - Dynamic array implementation for storing Row pointers
 * 
 * Provides a resizable array that automatically grows when full.
 * Used by Query team to store and access CSV row data.
 * 
 * AUTHOR: Billy
 * DATE: 2025-11-11
 * VERSION: v0.0.2
 */

#include "../include/vec.h"
#include <stdlib.h>
#include <string.h>

// Vector structure: stores auto-resizable array of Row pointers
struct Vec {
    Row **items;  // array of Row pointers
    size_t length;  // number of items currently stored
    size_t capacity;  // maximum capacity before auto-resize
};

/* 
 * Creates a new vector with the specified initial capacity.
 * The vector will automatically resize when full.
 *
 * parameters:
 * - capacity: initial capacity (minimum 1, will be set to 1 if 0)
 *
 * RETURN: pointer to new Vec on success, NULL on allocation failure
 */
Vec *vec_new(size_t capacity) {

    if (capacity == 0) {
        capacity = 1;  // default minimum capacity
    }
    
    Vec *vec = malloc(sizeof(Vec));
    if (vec == NULL) { // allocation failed
        return NULL;
    }
    
    vec->items = malloc(sizeof(Row *) * capacity);
    if (vec->items == NULL) { // allocation failed
        free(vec);
        return NULL;
    }
    
    // initializes fields
    vec->length = 0;
    vec->capacity = capacity;
    
    return vec;
}

/* 
 * Gets the number of items currently stored in the vector.
 *
 * parameters:
 * - vec: vector to query
 *
 * RETURN: number of items in vector, 0 if vec is NULL
 */
size_t vec_length(const Vec *vec) {
    return vec == NULL ? 0 : vec->length;
}

/* 
 * Gets the current capacity of the vector (maximum items before resize).
 *
 * parameters:
 * - vec: vector to query
 *
 * RETURN: current capacity, 0 if vec is NULL
 */
size_t vec_capacity(const Vec *vec) {
    return vec == NULL ? 0 : vec->capacity;
}

/* 
 * Gets a pointer to the internal array of Row pointers.
 * This is provided for advanced operations like sorting that require
 * direct array access for performance.
 * 
 * WARNING: The caller must not modify the array size directly.
 * Only use vec_push() to add items and vec_free() to deallocate.
 * Modifying the array contents (swapping pointers) is allowed.
 *
 * parameters:
 * - vec: vector to get array pointer from
 *
 * RETURN: pointer to internal Row* array on success, NULL if vec is NULL
 */
Row **vec_get_data(Vec *vec) {
    return vec == NULL ? NULL : vec->items;
}

/* 
 * Gets the Row pointer at the specified index.
 *
 * parameters:
 * - vec: vector to get item from
 * - index: 0-based index of the item to retrieve
 *
 * RETURN: pointer to Row at index on success, NULL if vec is NULL or index is out of bounds
 */
Row *vec_get(const Vec *vec, size_t index) {
    if (vec == NULL || index >= vec->length) {
        return NULL;
    }
    return vec->items[index];
}

/* 
 * Internal helper: doubles the vector's capacity when it is full.
 * This is called automatically by vec_push() when needed.
 *
 * parameters:
 * - vec: vector to resize
 *
 * RETURN: 0 on success, -1 on failure (vec is NULL or realloc failed)
 */
static int vec_resize(Vec *vec) {
    if (vec == NULL) {
        return -1;
    }
    
    size_t new_capacity = vec->capacity * 2;
    if (new_capacity == 0) {
        new_capacity = 1;
    }
    
    Row **new_items = realloc(vec->items, sizeof(Row *) * new_capacity);
    if (new_items == NULL) { // failed to reallocate memory
        return -1;
    }
    
    vec->items = new_items;
    vec->capacity = new_capacity;
    return 0;
}

/* 
 * Adds a Row pointer to the end of the vector.
 * The vector will automatically resize if it is full.
 *
 * parameters:
 * - vec: vector to add item to
 * - item: Row pointer to add (must not be NULL)
 *
 * RETURN: 0 on success, -1 on failure (vec/item is NULL or resize failed)
 */
int vec_push(Vec *vec, Row *item) {
    if (vec == NULL || item == NULL) {
        return -1;
    }
    
    if (vec->length >= vec->capacity) {
        if (vec_resize(vec) != 0) { // failed to resize
            return -1;
        }
    }
    
    vec->items[vec->length++] = item;
    return 0;
}

/* 
 * Frees the vector structure and its internal array.
 * Does NOT free the Row objects stored in the vector.
 * The caller is responsible for freeing Row objects separately.
 *
 * parameters:
 * - vec: vector to free (safe to call with NULL)
 *
 * RETURN: none
 */
void vec_free(Vec *vec) {
    if (vec == NULL) return;
    
    if (vec->items != NULL) {
        free(vec->items);
    }
    free(vec);
}

