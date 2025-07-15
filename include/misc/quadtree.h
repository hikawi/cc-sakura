// misc/quadtree.h
//
// Represents a Quadtree data structure to handle separating collisions.

#include "engine/collision.h"
#include "misc/list.h"

#define MAX_COLLIDERS_PER_NODE 10
#define MAX_QUADTREE_NODE_DEPTH 7

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
 */
QuadtreeNode *quadtree_init(void);

/**
 * Adds a node to a quadtree.
 *
 * Returns true if it was added successfully, false if it can't add or can't
 * allocate memory for the collider.
 */
bool quadtree_add(QuadtreeNode *root, Collider *item);

/**
 * Removes a node from a quadtree.
 *
 * Returns true if it was removed, false if it failed to remove, error merging
 * nodes, etc.
 */
bool quadtree_remove(QuadtreeNode *root, Collider *item);

/**
 * Checks the quadtree for any items that may collide with the provided
 * collider.
 */
List *quadtree_query(QuadtreeNode *root, Collider *collider);

/**
 * Destroys a quadtree recursively.
 */
void quadtree_destroy(QuadtreeNode *root);
