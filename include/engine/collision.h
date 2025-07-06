// engine/collision.h
//
// The header responsible for creating collisions and handling
// collisions on objects. These don't know about sprites, they
// are just simple bounding boxes.

#pragma once

#include "SDL3/SDL_pixels.h"
#include "misc/vector.h"

#define MAX_QUADTREE_DEPTH 6
#define MAX_COLLIDERS_PER_NODE 10

/**
 * Represents an enumeration of collider types.
 */
typedef enum
{
    COLLIDER_TYPE_CAPSULE,
    COLLIDER_TYPE_AABB,
    COLLIDER_TYPE_OBB,
    COLLIDER_TYPE_CIRCLE,
} ColliderType;

/**
 * Represents the collider's collision type, for physical and debugging
 * purposes.
 */
typedef enum
{
    COLLISION_SOLID,   // The collider is an unmoving solid. (floors, walls)
    COLLISION_DYNAMIC, // The collider is a moveable solid. (chars, enemies,
                       // boxes)
    COLLISION_SENSOR,  // The collider is invisible, but needed to detect
                       // something in a range. (checkpoints, doors)
    COLLISION_HITBOX,  // The collider is meant to deal damage. (sword strikes,
                       // projectiles)
    COLLISION_HURTBOX, // The collider is meant to take damage. (the body,
                       // hitbox touches here to deal damage)
    COLLISION_GHOST,   // The collider is non-interactive. Ignored by physics
                       // (ghosts, decorations)
    COLLISION_ONE_WAY, // The collider is interactable from one way. (one-way
                       // doors, platforms)
    COLLISION_DEBUG_ZONE // The collider for debugging zones.
} CollisionType;

/**
 * Represents a bounding box in the shape of a capsule.
 */
typedef struct
{
    Vector2 p1; // The line segment connecting the center line of the capsule.
    Vector2 p2;
    double r; // The radius, half width of the capsule.
} CapsuleCollider;

/**
 * Represents a bounding box in the shape of a rectangle,
 * that can not be rotated.
 */
typedef struct
{
    double x; // The center's X.
    double y; // The center's Y.
    double w; // The rectangle's width.
    double h; // The rectangle's height.
} AABBCollider;

/**
 * Represents a bounding box in the shape of a rectangle
 * that can be rotated.
 */
typedef struct
{
    double x;     // The center's X.
    double y;     // The center's Y.
    double w;     // The rectangle's width.
    double h;     // The rectangle's height.
    double angle; // The rectangle's rotation in radians.
} OBBCollider;

/**
 * Represents a bounding box in the shape of a circle.
 */
typedef struct
{
    double x;
    double y;
    double r;
} CircleCollider;

/**
 * Represents a struct of a collider, with multiple types.
 */
typedef struct
{
    ColliderType collider_type;
    CollisionType collision_type;
    const char *name;
    union
    {
        CapsuleCollider capsule;
        AABBCollider aabb;
        OBBCollider obb;
        CircleCollider circle;
    };
} Collider;

/**
 * Represents a structure containing the collision, and explains
 * how the collision occurred.
 */
typedef struct
{
    bool is_colliding;
    Vector2 normal;
    double depth;
} Collision;

/**
 * Represents a list of colliders. Meant to hold multiple colliders,
 * usually with the same collision type.
 */
typedef struct
{
    Collider **list; // A list of pointers, since Collider should be held by the
                     // actors themselves.
    int length;
    int capacity;
} ColliderList;

/**
 * Represents a quadtree of collider regions, meant to subdivide colliders
 * into small regions on the screen, so on one physical tick, it only has to
 * clash with objects in the region, not everywhere.
 */
typedef struct QuadtreeNode
{
    AABBCollider bounds;            // Where the node surrounds.
    int depth;                      // The current depth. 0 is on the root.
    ColliderList *colliders;        // The list of colliders in this region.
    struct QuadtreeNode **children; // The children, in order of TL, TR, BR, BL.
} QuadtreeNode;

/**
 * Retrieves the color needed to debug a collision type.
 */
SDL_Color get_collision_type_debug_color(CollisionType type);

/**
 * Checks collisions of two colliders.
 *
 * This is important, a collision happens when one object tries to "go inside"
 * another object. The one doing the "penetration" is called the "colliding
 * object", and the other is called the collided object. In this function, c1
 * should ALWAYS be the collided, and c2 should ALWAYS be the colliding. Calling
 * opposite might cause normal vectors to be inverted.
 *
 * The normal vector is DEFINED (by me) to be the vector that is pointing
 * OUTWARDS from the surface that is being collided (c1). Applying this vector
 * to c2 at the length of "depth" would completely separate both objects.
 */
Collision check_collision(Collider *c1, Collider *c2);

/**
 * Creates a collider list.
 */
ColliderList *create_collider_list(void);

/**
 * Removes a collider from list.
 */
void add_collider_to_list(ColliderList *list, Collider *collider);

/**
 * Removes the collider at a certain index in the list.
 */
void remove_collider_at_index(ColliderList *list, int idx);

/**
 * Removes a collider by the pointer value. This removes only
 * the first instance.
 */
void remove_collider_from_list(ColliderList *list, Collider *collider);

/**
 * Removes a collider by the name of the collider. This removes only
 * the first instance.
 */
void remove_collider_by_name(ColliderList *list, const char *name);

/**
 * Copies all elements from the src collider list to the dst collider list
 * as new elements appending.
 */
void join_collider_lists(ColliderList *dst, ColliderList *src);

/**
 * Destroys the collider list. The pointer is now freed.
 */
void destroy_collider_list(ColliderList *list);

/**
 * Creates a new quadtree node.
 */
QuadtreeNode *create_quadtree_node(AABBCollider bounds, int depth);

/**
 * Looks for the nearest node that holds a collider in a quadtree.
 */
void query_quadtree_node(QuadtreeNode *root, Collider *collider,
                         ColliderList *list);

/**
 * Populates the quadtree node after filling it with colliders.
 */
void subdivide_quadtree(QuadtreeNode *node);

/**
 * Destroys the populated quadtree node.
 */
void destroy_quadtree_node(QuadtreeNode *node);
