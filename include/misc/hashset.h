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
    const void *item;
    struct HashSetNode *next;
} HashSetNode;

/**
 * Represents a generic hash set.
 */
typedef struct
{
    HashSetNode *buckets[HASH_SET_MAX_BUCKETS];
    int length;
    bool (*compare)(const void *a, const void *b);
    Uint64 (*hash)(const void *item);
} HashSet;

/**
 * Initializes a hash set.
 */
HashSet *hash_set_init(void);

/**
 * Adds an item to a hash set. Returns true if it was added and false if it was
 * not added, maybe it already existed.
 */
bool hash_set_add(HashSet *set, const void *item);

/**
 * Removes an item from a hash set.
 */
bool hash_set_remove(HashSet *set, const void *item);

/**
 * Checks if a hash set contains an item.
 */
bool hash_set_has(const HashSet *set, const void *item);

/**
 * Iterates through the hash set and collects all elements into the provided
 * pointer. If the pointer is null, it will be created for you.
 */
void hash_set_iterate(const HashSet *set, int *length, const void ***items);

/**
 * Clears all elements from a hash set.
 */
void hash_set_clear(HashSet *set);

/**
 * Frees the hash set pointer.
 */
void hash_set_destroy(HashSet *set);
