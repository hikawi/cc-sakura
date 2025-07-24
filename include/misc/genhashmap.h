/**
 * \file misc/genhashmap.h
 *
 * A truly generic hash map implementation for pointer keys and pointer values. This provides greater customization for
 * hash maps, with provided destructives.
 */

#pragma once

#include "common.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct GenericHashMapNode GenericHashMapNode;

/**
 * Represents a generic hash map with an arbitrary capacity.
 */
typedef struct
{
    GenericHashMapNode **buckets;

    /**
     * Hashing function for the key.
     */
    HashFunction hash;

    /**
     * Comparator function for the key.
     */
    CompareFunction comparator;

    /**
     * The map's number of key-value pairs
     */
    uint32_t size;

    /**
     * The map's bucket capacity
     */
    uint32_t capacity;

    /**
     * Should the map free the key when the pair is destroyed?
     *
     * If `true`, the key will be free-d when the related key-value pair is freed.
     */
    bool destroys_key;

    /**
     * Should the map free the value when the pair is destroyed?
     *
     * If `true`, the value will be free-d when the related key-value pair is freed.
     */
    bool destroys_value;
} GenericHashMap;

/**
 * Initializes a generic hash map.
 *
 * \returns the generic hash map
 */
GenericHashMap *gen_hash_map_init(void);

/**
 * Puts a key-value pair in the generic hash map.
 *
 * \param map the map to modify
 * \param key the key
 * \param value the value
 * \returns the previous value placed at the map, `NULL` otherwise. This also returns `NULL` if
 * GenericHashMap::destroys_value is `true`.
 */
const void *gen_hash_map_put(GenericHashMap *map, const void *key, const void *value);

/**
 * Removes a key-value pair from the generic hash map.
 *
 * \param map the map to modify
 * \param key the key
 * \returns the previous value placed at the map. If GenericHashMap::destroys_value is `true` or no value was removed,
 * this returns `NULL`.
 */
const void *gen_hash_map_remove(GenericHashMap *map, const void *key);

/**
 * Retrieves a value from the key-value pair in a generic hash map.
 *
 * \param map the map to modify
 * \param key the key
 * \returns the value if found, or `NULL` if not
 */
const void *gen_hash_map_get(GenericHashMap *map, const void *key);

/**
 * Checks if the generic hash map contains a key.
 *
 * \param map the map to check
 * \param key the key to check
 * \returns true if the hash map contains such a key
 */
bool gen_hash_map_has(const GenericHashMap *map, const void *key);

/**
 * Iterates through the hash map for a list of key-value pairs.
 *
 * \param map the map to iterate through
 * \param keys the pointer to a keys array
 * \param values the pointer to a values array
 */
void gen_hash_map_iterate(const GenericHashMap *map, const void ***keys, const void ***values);

/**
 * Clears the provided generic hash map.
 *
 * \param map the map to clear
 */
void gen_hash_map_clear(GenericHashMap *map);

/**
 * Destroys a generic hash map.
 *
 * \param map the map to destroy
 */
void gen_hash_map_destroy(GenericHashMap *map);
