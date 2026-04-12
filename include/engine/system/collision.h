/**
 * \file engine/system/collision.h
 *
 * Provides the collision detection system.
 */

#pragma once

#include "engine/collision.h"
#include "engine/component/collider.h"

namespace ccsakura::system
{

/**
 * Checks for the collisions between two colliders.
 *
 * \param self the current collider
 * \param other the other collider
 * \returns the collision struct, with the normal pointing from A to B
 */
collision check_collisions(const components::collider &self, const components::collider &other);

} // namespace ccsakura::system
