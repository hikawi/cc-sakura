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
    bool dirty;
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
 * Holds data for a potential collision pair for broad-range checking.
 */
typedef struct
{
    const Collider *a;
    const Collider *b;
} CollisionPair;

/**
 * Retrieves the color needed to debug a collision type.
 */
SDL_Color collision_get_debug_color(CollisionType type);

/**
 * Converts a collider into an AABB collider for much faster checking.
 */
Collider collision_convert_to_aabb(const Collider *collider);

/**
 * Checks if the outer collider fully encloses the inner collider. This only
 * works with AABB colliders.
 */
bool collision_is_fully_enclosed(const Collider *outer, const Collider *inner);

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
Collision collision_check(const Collider *c1, const Collider *c2);

/**
 * Runs a partial collision check by approximating the closest AABB on both
 * colliders and compare the AABBs instead.
 */
Collision collision_partial_check(const Collider *c1, const Collider *c2);

/**
 * Hashes a collision pair.
 */
Uint64 collision_pair_hash(const void *pair);

/**
 * Compares two collision pairs.
 */
bool collision_pair_eq(const void *a, const void *b);
