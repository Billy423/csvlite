/**
 * hmap.c - Hash map implementation for string key -> void* value pairs
 * 
 * Provides a hash table with collision handling. 
 * 
 * Note:
 * - The capacity is fixed and does not change after initialization
 * - The resize feature is not implemented (given the scope of the project)

 */

#include "../include/hmap.h"
#include <stdlib.h>
#include <string.h>

// Stores key-value pair (implemented using hashtable)
typedef struct HMapEntry {
    char *key;  // string key
    void *value;  // value pointer
    struct HMapEntry *next;  // next entry in LinkedList (for collision handling)
} HMapEntry;

struct HMap {
    HMapEntry **buckets;  // array of pointer to HMapEntry
    size_t capacity;  // number of buckets
    size_t size;
};

// Simple hash function using djb2 algorithm
// Returns hash value in range [0, capacity)
// Note: not a good hash function, but it's simple and works for our use case
static size_t hash_code(const char *key, size_t capacity) {
    if (key == NULL || capacity == 0) return 0;
    
    unsigned long hash = 5381;  // prime
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash % capacity;
}

// Initialize hash map
HMap *hmap_new(size_t capacity) {
    if (capacity == 0) {
        capacity = 16;  // default minimum capacity
    }
    
    HMap *map = malloc(sizeof(HMap));
    if (map == NULL) { // allocation faileds
        return NULL;
    }
    
    // initialize buckets to NULL
    map->buckets = calloc(capacity, sizeof(HMapEntry *));
    if (map->buckets == NULL) { // allocation failed
        free(map);
        return NULL;
    }
    
    // initialize fields
    map->capacity = capacity;
    map->size = 0;
    
    return map;
}

// Insert or update key-value pair (key is copied)
// - returns previous value if key existed, NULL if new key
void *hmap_put(HMap *map, const char *key, void *value) {

    if (map == NULL || key == NULL) return NULL;
    
    size_t index = hash_code(key, map->capacity);
    HMapEntry *entry = map->buckets[index];
    
    // go to the end of the linked list
    while (entry != NULL) {
        // key already exists: update existing value, return previous
        if (strcmp(entry->key, key) == 0) {
            void *pre_value = entry->value;
            entry->value = value;
            return pre_value;
        }
        entry = entry->next;
    }
    
    // not in the linked list: create new entry
    entry = malloc(sizeof(HMapEntry));
    if (entry == NULL) { // allocation failed
        return NULL;
    }
    
    entry->key = malloc(strlen(key) + 1); // allow '\0' terminator
    if (entry->key == NULL) { // allocation failed
        free(entry);
        return NULL;
    }
    strcpy(entry->key, key);
    
    entry->value = value;
    entry->next = map->buckets[index];  // insert at head of linked list
    map->buckets[index] = entry;
    map->size++;
    
    return NULL;  // new key, no previous value
}

// Get value for key
// - returns NULL if key not found
void *hmap_get(const HMap *map, const char *key) {

    if (map == NULL || key == NULL) return NULL;
    
    size_t index = hash_code(key, map->capacity);
    HMapEntry *entry = map->buckets[index];
    
    
/*
    Note: strcmp() takes O(l) time, collision handling gives O(k × l) total
    Possible optimizations:
        - store additional have_code in HMapEntry
        - but additional hash code leads to more memory usage per entry
    Conclusion: with csv-handling, time tradeoff is acceptable
*/
    // traverse the linked list
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;  // key not found
}

// Get value for key, or return default if key not found
// - returns value if key exists, default_value if not found
void *hmap_get_or_default(const HMap *map, const char *key, void *default_value) {
    void *value = hmap_get(map, key);
    return value != NULL ? value : default_value;
}

// Remove key-value pair
// - returns removed value if key existed, NULL if key not found
void *hmap_remove(HMap *map, const char *key) {

    if (map == NULL || key == NULL) return NULL;
    
    size_t index = hash_code(key, map->capacity);
    HMapEntry *entry = map->buckets[index];
    HMapEntry *prev = NULL;
    
    // remove element from linked list
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) { // key found
            
            void *pre_value = entry->value;
            
            if (prev == NULL) { // first entry
                map->buckets[index] = entry->next;
            } else {
                prev->next = entry->next;
            }
            
            // caller is responsible for freeing the value
            free(entry->key); 
            free(entry);
            map->size--;
            return pre_value;
        }
        prev = entry;
        entry = entry->next;
    }
    
    return NULL;  // key not found
}

size_t hmap_size(const HMap *map) {
    return map == NULL ? 0 : map->size;
}

// Free hash map
// - does not free value pointers
void hmap_free(HMap *map) {

    if (map == NULL) return;
    
    // free all entries in all buckets
    for (size_t i = 0; i < map->capacity; i++) {
        HMapEntry *entry = map->buckets[i];

        // free each entry in the linked list
        while (entry != NULL) {
            HMapEntry *next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    
    free(map->buckets);
    free(map);
}