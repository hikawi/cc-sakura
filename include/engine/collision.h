/**
 * \file engine/collision.h
 *
 * \brief Collision-related structs, checkers and handlers
 *
 * This header is responsible for providing related structs for collisions, such as a polymorphic collider for objects.
 * Colliders do not know about the existence of sprites, both must be handled simulatenously and independently.
 */

#pragma once

#include "misc/vector.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"

/**
 * Represents an enumeration of collider shapes.
 */
typedef enum
{
    /**
     * The collider is of primitive type capsule.
     */
    COLLIDER_SHAPE_TYPE_CAPSULE,

    /**
     * The collider is of primitive type axis-aligned bounding box. This box can not be rotated.
     */
    COLLIDER_SHAPE_TYPE_AABB,

    /**
     * The collider is of primitive type oriented bounding box. This can be considered a rotated AABB.
     */
    COLLIDER_SHAPE_TYPE_OBB,

    /**
     * The collider is of primitive type circle.
     */
    COLLIDER_SHAPE_TYPE_CIRCLE,
} ColliderShapeType;

/**
 * Represents the collider's collision type, for physical checking and debugging purposes.
 *
 * This somewhat tells the engine to ignore what pairs of colliders to **ignore** checking to reduce computational load.
 * In the table below, any columns without a `Yes`, that pair will be ignored by the collision checker.
 *
 * | Layer ↓ / Layer → | Static | Dynamic | Projectile | Sensor | Hitbox | Hurtbox | Ghost |
 * |-:-----------------|-:-:----|-:-:-----|-:-:--------|-:-:----|-:-:----|-:-:-----|-:-:---|
 * | **Static**        |        | Yes     | Yes        |        |        |         | Yes   |
 * | **Dynamic**       | Yes    | Yes     | Yes        | Yes    |        | Yes     | Yes   |
 * | **Projectile**    | Yes    | Yes     |            | Yes    |        | Yes     | Yes   |
 * | **Sensor**        |        | Yes     | Yes        |        |        |         | Yes   |
 * | **Hitbox**        |        |         |            |        |        | Yes     | Yes   |
 * | **Hurtbox**       |        | Yes     | Yes        |        | Yes    |         | Yes   |
 * | **Ghost**         | Yes    | Yes     | Yes        | Yes    | Yes    | Yes     | Yes   |
 */
typedef enum
{
    /**
     * The collider is an unmovable solid.
     *
     * Including floors, walls, ceilings, etc.
     */
    COLLIDER_TYPE_STATIC,

    /**
     * The collider is movable.
     *
     * This includes characters, enemies, boxes, etc.
     */
    COLLIDER_TYPE_DYNAMIC,

    /**
     * The collider is a projectile.
     *
     * Projectiles in general don't collide with each other.
     */
    COLLIDER_TYPE_PROJECTILE,

    /**
     * The collider is an interactive sensor.
     *
     * This collider shouldn't be collided with, but they can check as a collider. For example, to check for prompts to
     * interact with objects, NPCs that you can't push but can talk to, etc.
     */
    COLLIDER_TYPE_SENSOR,

    /**
     * The collider is a damage-dealing collider.
     *
     * This collider generally interacts with \ref COLLIDER_TYPE_HURTBOX to deal damage to said hurtboxes.
     */
    COLLIDER_TYPE_HITBOX,

    /**
     * The collider is a damage-taking collider.
     *
     * Hitbox colliders that hit this collider should be able to deal damage to the character or object holding this
     * collider.
     */
    COLLIDER_TYPE_HURTBOX,

    /**
     * The collider is a ghost.
     *
     * This is meant to be used as placeholders for unknown colliders. Ghost colliders will always be checked.
     */
    COLLIDER_TYPE_GHOST,
} ColliderType;

/**
 * Represents a bounding box in the shape of a capsule.
 *
 * A capsule is defined by two endpoints (the shaft) of the capsule and the radius. Imagine you can split the capsule
 * into a rectangle and two semicircles, the two endpoints would define the midpoints of the two rectangle's edges.
 */
typedef struct
{
    /**
     * First endpoint of the segment.
     */
    Vector2 p1;

    /**
     * Second endpoint of the segment.
     */
    Vector2 p2;

    /**
     * The radius, half-width of the capsule.
     */
    double r;
} CapsuleCollider;

/**
 * Represents a bounding box in the shape of a rectangle,
 * that can not be rotated.
 *
 * This bounding box is defined to be the origin of the rectangle (x, y) with full-extents (w, h).
 */
typedef struct
{
    /**
     * The center's X.
     */
    double x;

    /**
     * The center's Y.
     */
    double y;

    /**
     * The rectangle's full width.
     */
    double w;

    /**
     * The rectangle's full height.
     */
    double h;
} AABBCollider;

/**
 * Represents a bounding box in the shape of a rectangle
 * that can be rotated.
 *
 * This is defined by similar things to \ref AABBCollider, but there's an extra `angle`.
 */
typedef struct
{
    /**
     * The center's X.
     */
    double x;

    /**
     * The center's Y.
     */
    double y;

    /**
     * The rectangle's full width.
     */
    double w;

    /**
     * The rectangle's full height.
     */
    double h;

    /**
     * The rectangle's angle in radians.
     */
    double angle;
} OBBCollider;

/**
 * Represents a bounding box in the shape of a circle.
 */
typedef struct
{
    /**
     * The center's x.
     */
    double x;

    /**
     * The center's y.
     */
    double y;

    /**
     * The circle's radius.
     */
    double r;
} CircleCollider;

/**
 * Represents a struct of a collider, with multiple types.
 */
typedef struct
{
    /**
     * The collider's shape type.
     *
     * This defines how the collision checks are performed.
     */
    ColliderShapeType collider_shape_type;

    /**
     * The collider's type.
     */
    ColliderType collider_type;

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
    /**
     * Whether this collision event is happening.
     */
    bool is_colliding;

    /**
     * The direction vector to apply to the colliding collider to separate both.
     *
     * This is defined to be the normal vector of the surface that was collided with. If B runs into A from the left,
     * then this normal vector should push B to the left.
     */
    Vector2 normal;

    /**
     * The scalar to scale the direction vector with.
     */
    double depth;
} Collision;

/**
 * \internal
 *
 * Holds data for a potential collision pair for broad-range checking.
 */
typedef struct
{
    const Collider *a;
    const Collider *b;
} CollisionPair;

/**
 * Retrieves the color needed to debug a collider type.
 *
 * \param type the type of collider to debug for
 * \returns the color
 */
SDL_Color collision_get_debug_color(const ColliderType type);

/**
 * Converts a collider into an AABB collider for much faster broad-phase checking.
 *
 * \param collider the collider to check
 * \returns an rvalue collider of the AABB representation
 */
Collider collision_convert_to_aabb(const Collider *collider);

/**
 * Checks if the outer collider fully encloses the inner collider.
 *
 * \warning This only works with AABB colliders.
 * \param outer the outer collider which will enclose the inner
 * \param inner the inner collider which will be enclosed
 * \returns true if `outer` fully encloses `inner`
 */
bool collision_is_fully_enclosed(const Collider *outer, const Collider *inner);

/**
 * Checks collisions of two colliders.
 *
 * This is important, a collision happens when one object tries to "go inside"
 * another object. The one doing the "penetration" is called the "colliding
 * object", and the other is called the collided object. In this function, `c1`
 * should ALWAYS be the collided, and `c2` should ALWAYS be the colliding. Calling
 * opposite might cause normal vectors to be inverted.
 *
 * The normal vector is DEFINED (by me) to be the vector that is pointing
 * OUTWARDS from the surface that is being collided (`c1`). Applying this vector
 * to `c2` at the length of "depth" would completely separate both objects.
 *
 * \param c1 the collided object
 * \param c2 the colliding object
 * \returns a collision information struct
 */
Collision collision_check(const Collider *c1, const Collider *c2);

/**
 * Runs a partial collision check by approximating the closest AABB on both
 * colliders and compare the AABBs instead.
 *
 * \warning This may be inaccurate due to approximations
 * \param c1 the collided object
 * \param c2 the colliding object
 * \returns a collision information on the approximations
 */
Collision collision_partial_check(const Collider *c1, const Collider *c2);

/**
 * Hashes a collision pair.
 *
 * \param pair the pair to hash
 * \returns the hash information
 */
uint64_t collision_pair_hash(const CollisionPair *pair);

/**
 * Compares two collision pairs.
 *
 * A collision pair pair is considered equal if the holding pointers are equal, or the pairs are equal meaningfully.
 * For example (A, B) would be considered equal to (B, A).
 *
 * A collision pair is considered less than another, if its minimum pointer is less than the other minimum pointer, and
 * vice versa for greater than. Generally we don't mess with this part, as we only care if two pairs are equal.
 *
 * \param a the first pair
 * \param b the second pair
 * \returns a number below 0 if a < b, 0 if a = b, and > 0 if a > b.
 */
int collision_pair_comp(const CollisionPair *a, const CollisionPair *b);

/**
 * Renders a collider on the target renderer.
 *
 * \param collider the collider to draw
 * \param renderer the renderer to draw to
 */
void collider_render(const Collider *collider, SDL_Renderer *renderer);
