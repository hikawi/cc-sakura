/**
 * \file misc/list.h
 *
 * Simple implementation of a generic list of pointers. Use C-style arrays if you want to store primitives instead.
 */

#pragma once

#include "common.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Represents a generic list of items.
 */
typedef struct
{
    /**
     * The items present within the list.
     */
    const void **items;

    /**
     * The length of the list.
     */
    uint32_t length;

    /**
     * \internal
     *
     * The capacity of the list.
     */
    uint32_t capacity;
} List;

/**
 * Initializes an empty list.
 *
 * \returns an empty list
 */
List *list_init(void);

/**
 * Adds an item to the list, expands if necessary.
 *
 * \param list the list to add to
 * \param item the item to add
 */
void list_add(List *list, const void *item);

/**
 * Adds an item at a specified index, expands if necessary.
 *
 * \param list the list to add to
 * \param item the item to add
 * \param k the index to add at
 */
void list_add_at(List *list, const void *item, const uint32_t k);

/**
 * Removes an item from the list at the specified index, shrinks if necessary.
 *
 * \param list the list to remove from
 * \param idx the index to remove at
 */
void list_remove_at(List *list, const uint32_t idx);

/**
 * Removes an item, shrinks if necessary.
 *
 * This essentially finds the item, and defers to list_remove_at. This only removes the first occurrence of the item.
 *
 * \param list the list to remove from
 * \param item the item to remove
 */
void list_remove(List *list, const void *item);

/**
 * Retrieves an item at the specified index.
 *
 * \param list the list to query
 * \param index the index to query at
 * \returns the pointer if queried, `NULL` if out of bounds
 */
const void *list_get(const List *list, const uint32_t index);

/**
 * Finds an item in a list.
 *
 * \param list the list to query
 * \param item the item to look for
 * \returns the index if found, -1 if not found
 */
int list_find(const List *list, const void *item);

/**
 * Does a linear search on if the list has the specified item.
 *
 * \param list the list to query
 * \param item the item to check
 * \returns true if the list contains the item
 */
bool list_has(const List *list, const void *item);

/**
 * Joins items from the source list to the first list.
 *
 * This does not check for duplicate items. After the operation, the `list` will have all `src` items appended to it.
 *
 * \param list the list destination to join to
 * \param src the source items to join with
 */
void list_join(List *list, const List *src);

/**
 * Clears all items in a list, shrinks if necessary.
 *
 * \param list the list to clear
 */
void list_clear(List *list);

/**
 * Sorts a list of items based on the comparator function.
 *
 * Uses a comparator that returns a negative value if l < p, 0 if l = p and
 * positive value if l > p. This uses a simple quick-sort algorithm.
 *
 * \param list the list to sort
 * \param comparator the comparator to sort with
 */
void list_sort(List *list, CompareFunction comparator);

/**
 * Destroys a list and reclaims memory.
 *
 * \param list the list to destroy
 */
void list_destroy(List *list);
