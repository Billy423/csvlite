/**
 * vec.c - Dynamic array implementation for storing Row pointers
 * 
 * Provides a resizable array that automatically grows when full.
 * Used by Query team to store and access CSV row data.
 */

#include "vec.h"
#include <stdlib.h>
#include <string.h>

// Vector structure: stores auto-resizable array of Row pointers
struct Vec {
    Row **items;  // array of Row pointers
    size_t length;  // number of items currently stored
    size_t capacity;  // maximum capacity before auto-resize
};

// Initialize a new vector with the given capacity
// Returns a pointer to the new vector, or NULL if allocation fails
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

// Get number of items stored (returns 0 if vec is NULL)
size_t vec_length(const Vec *vec) {
    return vec == NULL ? 0 : vec->length;
}

// Get current capacity (returns 0 if vec is NULL)
size_t vec_capacity(const Vec *vec) {
    return vec == NULL ? 0 : vec->capacity;
}

// Get Row at index 
// Returns NULL if invalid index or vec is NULL
Row *vec_get(const Vec *vec, size_t index) {
    if (vec == NULL || index >= vec->length) {
        return NULL;
    }
    return vec->items[index];
}

// Internal helper: doubles capacity when vector is full
// Returns 0 on success, -1 on failure
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

// Add Row to end of vector (auto-resizes if full)
// Returns 0 on success, -1 on failure
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

// Free vector structure
void vec_free(Vec *vec) {
    if (vec == NULL) return;
    
    if (vec->items != NULL) {
        free(vec->items);
    }
    free(vec);
}

