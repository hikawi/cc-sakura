// engine/collision.h
//
// The header responsible for creating collisions and handling
// collisions on objects. These don't know about sprites, they
// are just simple bounding boxes.

#pragma once

#include "SDL3/SDL_pixels.h"

/**
 * Represents an enumeration of collider types.
 */
typedef enum
{
  COLLIDER_TYPE_CAPSULE,
  COLLIDER_TYPE_AABB,
  COLLIDER_TYPE_OBB,
} ColliderType;

/**
 * Represents the collider's collision type, for physical and debugging
 * purposes.
 */
typedef enum
{
  COLLISION_SOLID,     // The collider is an unmoving solid. (floors, walls)
  COLLISION_DYNAMIC,   // The collider is a moveable solid. (chars, enemies, boxes)
  COLLISION_SENSOR,    // The collider is invisible, but needed to detect something in a range. (checkpoints, doors)
  COLLISION_HITBOX,    // The collider is meant to deal damage. (sword strikes, projectiles)
  COLLISION_HURTBOX,   // The collider is meant to take damage. (the body, hitbox touches here to deal damage)
  COLLISION_GHOST,     // The collider is non-interactive. Ignored by physics (ghosts, decorations)
  COLLISION_ONE_WAY,   // The collider is interactable from one way. (one-way doors, platforms)
  COLLISION_DEBUG_ZONE // The collider for debugging zones.
} CollisionType;

/**
 * Represents a bounding box in the shape of a capsule.
 */
typedef struct
{
  double x; // The center's X.
  double y; // The center's Y.
  double w; // The capsule's full width. This / 2 is the radius horizontally.
  double h; // The capsule's full height. This / 2 is the radius vertically.
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
 * Represents a struct of a collider, with multiple types.
 */
typedef struct
{
  ColliderType collider_type;
  CollisionType collision_type;
  union
  {
    CapsuleCollider capsule;
    AABBCollider aabb;
    OBBCollider obb;
  };
} Collider;

/**
 * Retrieves the color needed to debug a collision type.
 */
SDL_Color get_collision_type_debug_color(CollisionType type);
