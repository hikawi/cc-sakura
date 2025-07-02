// engine/physics.h
//
// Physics component of the engine.

#pragma once

#include "misc/vector.h"

/**
 * Apply the direction to the current position, taking account for
 * the delta time.
 *
 * Parameters:
 * - pos: The current position
 * - dir: The direction, might or might not be normalized
 * - spd: The actor's speed
 * - dt: The deltatime to scale with frame rate
 */
Vector2 apply_movement(Vector2 pos, Vector2 dir, double spd, double dt);
