/**
 * \file engine/collision.h
 *
 * Provides a framework for handling collisions and making collider checks and how to resolve such collisions with
 * normal vectors.
 */

#pragma once

#include "engine/vec2d.h"

namespace ccsakura
{

/**
 * PODO for a collision.
 */
struct collision
{
    bool is_colliding = false; ///< if the collision is happening
    double depth = 0;          ///< how far are the colliders in each other
    vec2d normal;              ///< the normal vector pointing towards the shortest direction to resolve the collision
};

} // namespace ccsakura
