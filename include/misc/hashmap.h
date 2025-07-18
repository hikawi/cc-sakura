// misc/hashmap.h
//
// Represents a simple string as a key hash map.

#pragma once

#define HASH_MAP_MAX_BUCKETS 256

#include "SDL3/SDL_stdinc.h"

/**
 * Represents a node inside the hash map.
 */
typedef struct HashMapNode
{
    Uint32 key;
    const void *value;
    struct HashMapNode *next;
} HashMapNode;

/**
 * Represents a simple hash map using unsigned ints as values.
 */
typedef struct
{
    HashMapNode *nodes[HASH_MAP_MAX_BUCKETS];
    Uint32 size;
} HashMap;

/**
 * Initializes a new empty hash map.
 */
HashMap *hash_map_init(void);

/**
 * Checks if a key is present within a hash map.
 */
bool hash_map_has_key(const HashMap *map, const Uint32 key);

/**
 * Checks if a value is present within a hash map. This is not recommended as
 * this goes through all of the hash map's content.
 */
bool hash_map_has_value(const HashMap *map, const void *value);

/**
 * Retrieves a hash map item from the provided key. If it doesn't exist, NULL is
 * provided.
 */
const void *hash_map_get(const HashMap *map, const Uint32 key);

/**
 * Puts an item with a key-value into the hash map. If it replaced something,
 * return that something. Otherwise, return NULL if it's a new item.
 */
const void *hash_map_put(HashMap *map, const Uint32 key, const void *value);

/**
 * Removes an item from the hash map. Returns that removed item, or NULL if
 * there was nothing to remove.
 */
const void *hash_map_remove(HashMap *map, const Uint32 key);

/**
 * Clears all entries present within the hash map.
 */
void hash_map_clear(HashMap *map);

/**
 * Retrieves all key and value pairs of the hash map.
 */
void hash_map_iterate(const HashMap *map, Uint32 **keys, const void ***values);

/**
 * Destroys and reclaims memory of a hash map.
 */
void hash_map_destroy(HashMap *map);
