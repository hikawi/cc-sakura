// misc/hashset.h
//
// Represents a generic type of hash set.

#pragma once

#include "SDL3/SDL_stdinc.h"
#include <stdbool.h>

#define HASH_SET_MAX_BUCKETS 256

/**
 * Represents the node present within a hash set's buckets.
 */
typedef struct HashSetNode
{
    void *item;
    struct HashSetNode *next;
} HashSetNode;

/**
 * Represents a generic hash set.
 */
typedef struct
{
    HashSetNode *buckets[HASH_SET_MAX_BUCKETS];
    int length;
    bool (*compare)(void *a, void *b);
    Uint32 (*hash)(void *item);
} HashSet;

/**
 * Initializes a hash set.
 */
HashSet *hash_set_init(void);

/**
 * Adds an item to a hash set. Returns true if it was added and false if it was
 * not added, maybe it already existed.
 */
bool hash_set_add(HashSet *set, void *item);

/**
 * Removes an item from a hash set.
 */
bool hash_set_remove(HashSet *set, void *item);

/**
 * Checks if a hash set contains an item.
 */
bool hash_set_has(HashSet *set, void *item);

/**
 * Clears all elements from a hash set.
 */
void hash_set_clear(HashSet *set);

/**
 * Frees the hash set pointer.
 */
void hash_set_destroy(HashSet *set);
