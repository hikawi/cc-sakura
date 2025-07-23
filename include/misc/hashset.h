/**
 * \file misc/hashset.h
 *
 * A generic pointer-based hash set.
 */

#pragma once

#include "common.h"

#include <stdbool.h>
#include <stdint.h>

#define HASH_SET_MAX_BUCKETS 256

/**
 * \internal
 *
 * Represents the node present within a hash set's buckets.
 */
typedef struct HashSetNode
{
    const void *item;
    struct HashSetNode *next;
} HashSetNode;

/**
 * Represents a generic hash set.
 *
 * Each hashset holds only pointers to objects, it's not meant to hold simple values as that can not be "generic"-fied.
 */
typedef struct
{
    HashSetNode *buckets[HASH_SET_MAX_BUCKETS];

    /**
     * The length of the hash set.
     */
    uint32_t length;

    /**
     * The compare function of the hash set.
     *
     * It must be a function that takes two pointers, and returns an integer to denote which one is before which one.
     * `< 0` means first is before right, `= 0` if both are equal, and `> 0` if first is behind right.
     */
    CompareFunction compare;

    /**
     * The hash function of the hash set.
     *
     * It must be a function that takes one pointer and hashes it down to a `uint64_t`.
     */
    HashFunction hash;
} HashSet;

/**
 * Initializes a hash set.
 *
 * \returns a generic hash set
 */
HashSet *hash_set_init(void);

/**
 * Adds an item to a hash set.
 *
 * \param set the hash set to add to
 * \param item the item to add to
 * \returns true if it was added and false if it was not added, maybe it already existed.
 */
bool hash_set_add(HashSet *set, const void *item);

/**
 * Removes an item from a hash set.
 *
 * \param set the set to remove from
 * \param item the item to remove
 * \returns true if it was removed, false if it did not exist
 */
bool hash_set_remove(HashSet *set, const void *item);

/**
 * Checks if a hash set contains an item.
 *
 * \param set the set to check
 * \param item the item to check with
 * \returns true if the hash set contains the item
 */
bool hash_set_has(const HashSet *set, const void *item);

/**
 * Iterates through the hash set and collects all elements into the provided
 * pointer.
 *
 * \warning If you pass in a non-null pointer, make sure they can handle at least `length` items from hash-set,
 * or it is an undefined behavior.
 * \param set the set to iterate from
 * \param length the pointer that the hash set's size will be put into.
 * \param items the pointer to an array of items in the hash set. If this is NULL, it will be allocated for you. If it
 * is not NULL, make sure it can contain at least `length` items.
 */
void hash_set_iterate(const HashSet *set, uint32_t *length, const void ***items);

/**
 * Clears all elements from a hash set.
 *
 * \param set the set to clear
 */
void hash_set_clear(HashSet *set);

/**
 * Frees the hash set pointer.
 *
 * \warning This invalidates the provided hash set pointer
 * \param set the set to destroy
 */
void hash_set_destroy(HashSet *set);
