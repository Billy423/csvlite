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
// - returns 0 on success, -1 if failed
int hmap_put(HMap *map, const char *key, void *value);

// Get value for key
// - returns NULL if key not found
void *hmap_get(const HMap *map, const char *key);

// Remove key-value pair (does NOT free value)
// - returns 0 on success, -1 if key not found
int hmap_remove(HMap *map, const char *key);

// Get number of key-value pairs
size_t hmap_size(const HMap *map);

// Free hash map
// - does not free value pointers
void hmap_free(HMap *map);

#endif

