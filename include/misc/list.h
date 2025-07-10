// misc/list.h
//
// Simple implementation of a list of pointers. Handle your own lists if you
// want to store primitives.

#pragma once

#include "SDL3/SDL_stdinc.h"

/**
 * Represents a generic list of items.
 */
typedef struct
{
    void **items;
    Uint32 length;
    Uint32 capacity;
} List;

/**
 * Initializes a list.
 */
List *list_init(void);

/**
 * Adds an item to the list, expands if necessary.
 */
void list_add(List *list, void *item);

/**
 * Adds an item at a specified index. Expands if necessary.
 */
void list_add_at(List *list, void *item, Uint32 k);

/**
 * Removes an item from the list at the specified index. Shrinks if necessary.
 */
void list_remove_at(List *list, Uint32 idx);

/**
 * Removes an item. Shrinks if necessary.
 */
void list_remove(List *list, void *item);

/**
 * Retrieves an item at the specified index. NULL if out of bounds.
 */
void *list_get(List *list, int index);

/**
 * Finds an item. Returns -1 if not found.
 */
int list_find(List *list, void *item);

/**
 * Does a linear search on if the list has the specified item.
 */
bool list_has(List *list, void *item);

/**
 * Joins items from the source list to the first list. This does not check for
 * duplicate items.
 */
void list_join(List *list, List *src);

/**
 * Clears all items in a list. Shrinks if necessary.
 */
void list_clear(List *list);

/**
 * Destroys a list and reclaims memory.
 */
void list_destroy(List *list);
