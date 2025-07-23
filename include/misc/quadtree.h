/**
 * \file misc/quadtree.h
 *
 * Provides a Quadtree data structure, which is a 4-children tree that subdivides the screen into 4 equal rectangular
 * regions. This provides a baseline for approximating colliders, so we can skip checking pairs of colliders that we
 * know are **definitely not touching**, instead of having to rely on more expensive checks.
 */

#pragma once

#include "engine/collision.h"
#include "misc/list.h"

/**
 * The main node of a quadtree.
 */
typedef struct QuadtreeNode
{
    List *colliders;
    AABBCollider region;
    int depth;
    struct QuadtreeNode *children[4];
} QuadtreeNode;

/**
 * Initializes a new quad tree.
 *
 * \returns a new quadtree node
 */
QuadtreeNode *quadtree_init(void);

/**
 * Adds a node to a quadtree.
 *
 * This addition will cause a subdivision if it is appropriate to do so.
 *
 * \param root the root node
 * \param item the item to add to the quadtree
 * \returns true if it was added successfully, false if it can't add or can't
 * allocate memory for the collider.
 */
bool quadtree_add(QuadtreeNode *root, const Collider *item);

/**
 * Removes a node from a quadtree.
 *
 * This removal will cause a merge if it is appropriate to do so.
 *
 * \param root the root node
 * \param item the item to remove from the quadtree
 * \returns true if it was removed, false if it failed to remove, error merging
 * nodes, etc.
 */
bool quadtree_remove(QuadtreeNode *root, const Collider *item);

/**
 * Checks the quadtree for any items that may collide with the provided
 * collider.
 *
 * The `list` is populated with all items that may collide with the provided collider.
 *
 * \param root the root node
 * \param collider the collider to check
 * \param list the list of potential colliders to populate with
 */
void quadtree_query(const QuadtreeNode *root, const Collider *collider, List *list);

/**
 * Destroys a quadtree recursively.
 *
 * \warning This invalidates the provided pointer
 * \param root the quadtree to destroy
 */
void quadtree_destroy(QuadtreeNode *root);
