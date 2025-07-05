// engine/physics.h
//
// Physics component of the engine.

#pragma once

#include "misc/vector.h"

#define GRAVITY 1200.0

/**
 * Apply the direction to the current position, taking account for
 * the delta time.
 *
 * Parameters:
 * - pos: The current position
 * - dir: The direction, might or might not be normalized
 * - spd: The actor's speed
 */
Vector2 apply_movement(Vector2 pos, Vector2 dir, double spd);

/**
 * Apply the velocity to the current position.
 *
 * This also changes the position and the velocity passed in.
 */
void apply_velocity(Vector2 *pos, Vector2 *velo);
