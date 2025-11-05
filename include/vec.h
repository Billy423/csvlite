#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include "row.h"

// Dynamic array for storing Row pointers
typedef struct Vec Vec;

// Create new vector with initial capacity
// - returns NULL if failed
Vec *vec_new(size_t capacity);

// Add row to end of vector (auto-resizes if needed)
// - returns 0 on success, -1 if failed
int vec_push(Vec *vec, Row *item);

// Get row at index (0-based)
// - returns NULL if invalid index
Row *vec_get(const Vec *vec, size_t index);

// Get number of items in vector
size_t vec_length(const Vec *vec);

// Get current capacity of vector
size_t vec_capacity(const Vec *vec);

// Free vector (does NOT free Row objects inside)
// - does not free Row objects inside
void vec_free(Vec *vec);

#endif

