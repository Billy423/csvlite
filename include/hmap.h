/*
* Header file for hmap.c
* 
* AUTHOR: Billy
* DATE: November 11, 2025
* VERSION: v2.0.0
*/

#ifndef HMAP_H
#define HMAP_H

#include <stddef.h>

// Hash map for string key -> void* value pairs
// Used for group-by operations and column name resolution
typedef struct HMap HMap;

// Create new hash map with initial capacity
// - returns NULL if failed
HMap *hmap_new(size_t capacity);

// Insert or update key-value pair (key is copied)
// - returns previous value if key existed, NULL if new key
void *hmap_put(HMap *map, const char *key, void *value);

// Get value for key
// - returns NULL if key not found
void *hmap_get(const HMap *map, const char *key);

// Get value for key, or return default if key not found
// - returns value if key exists, default_value if not found
void *hmap_get_or_default(const HMap *map, const char *key, void *default_value);

// Remove key-value pair (does NOT free value)
// - returns removed value if key existed, NULL if key not found
void *hmap_remove(HMap *map, const char *key);

// Get number of key-value pairs
size_t hmap_size(const HMap *map);

// Free hash map
// - does not free value pointers
void hmap_free(HMap *map);

#endif

