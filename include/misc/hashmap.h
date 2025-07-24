/**
 * \file misc/hashmap.h
 *
 * Represents a simple unsigned int to pointer generic hash map.
 */

#pragma once

#define HASH_MAP_MAX_BUCKETS 256

#include <stdbool.h>
#include <stdint.h>

/**
 * \internal
 *
 * Represents a node inside the hash map.
 */
typedef struct HashMapNode
{
    uint32_t key;
    const void *value;
    struct HashMapNode *next;
} HashMapNode;

/**
 * Represents a simple hash map using unsigned ints as keys.
 */
typedef struct
{
    HashMapNode *nodes[HASH_MAP_MAX_BUCKETS];

    /**
     * The size of the hash map.
     */
    uint32_t size;
} HashMap;

/**
 * Initializes a new empty hash map.
 *
 * \returns an empty hash map
 */
HashMap *hash_map_init(void);

/**
 * Checks if a key is present within a hash map.
 *
 * \param map map to check
 * \param key key to check
 * \returns true if `key` is present in `map`
 */
bool hash_map_has_key(const HashMap *map, const uint32_t key);

/**
 * Checks if a value is present within a hash map.
 *
 * \warning This is not recommended as this goes through all of the hash map's content.
 * \param map the hash map to check
 * \param value the pointer value to check
 * \returns true if the value exists in `map`
 */
bool hash_map_has_value(const HashMap *map, const void *value);

/**
 * Retrieves a hash map item from the provided key.
 *
 * \param map the map to query
 * \param key the key to query
 * \returns the value it is mapped to, or `NULL` if it doesn't exist
 */
const void *hash_map_get(const HashMap *map, const uint32_t key);

/**
 * Puts an item with a key-value into the hash map.
 *
 * \param map the map to check
 * \param key the key to map
 * \param value the value to map to
 * \returns the previous value, if exists, `NULL` otherwise
 */
const void *hash_map_put(HashMap *map, const uint32_t key, const void *value);

/**
 * Removes an item from the hash map.
 *
 * \param map the map to modify
 * \param key the key to remove
 * \returns the previous value mapped, `NULL` if it doesn't exist
 */
const void *hash_map_remove(HashMap *map, const uint32_t key);

/**
 * Clears all entries present within the hash map.
 *
 * \param map the map to clear
 */
void hash_map_clear(HashMap *map);

/**
 * Retrieves all key and value pairs of the hash map.
 *
 * The arrays will be allocated by the function for you.
 *
 * \param map the map to iterate
 * \param keys the pointer to a keys array
 * \param values the pointer to a values array
 */
void hash_map_iterate(const HashMap *map, uint32_t **keys, const void ***values);

/**
 * Destroys and reclaims memory of a hash map.
 *
 * \param map map to destroy
 */
void hash_map_destroy(HashMap *map);
