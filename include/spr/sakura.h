// spr/sakura.h
//
// Includes all information about Sakura's sprites and animations.
// This only provides the necessary data to render, but it does not
// have the rendering job.

#pragma once

#include "app.h"
#include "engine/collision.h"
#include "misc/vector.h"
#include "spr/sprites.h"
#include <stdbool.h>

/**
 * Represents the player, Sakura.
 */
typedef struct
{
  Vector2 pos;        // Sakura's position.
  Vector2 velo;       // Sakura's current velocity.
  Sprite *idle;       // The set of sprites for Sakura's idle animation.
  Collider *collider; // Sakura's collider.

  bool is_moving_up;    // Flag to see whether she's moving up.
  bool is_moving_down;  // Flag to see whether she's moving down.
  bool is_moving_left;  // Flag to see whether she's moving left.
  bool is_moving_right; // Flag to see whether she's moving right.
} Sakura;

/**
 * Initializes Sakura's main thing.
 *
 * Returns true if it was successful.
 */
bool init_sakura(void);

/**
 * Retrieves Sakura's pointer. This can never be NULL.
 */
Sakura *get_sakura(void);

/**
 * Requests Sakura's handler to handle her on a variable update tick.
 */
void update_sakura(AppState *, double dt);

/**
 * Requests Sakura's handler to handle her on a fixed update tick.
 */
void fixed_update_sakura(AppState *);

/**
 * Destroys Sakura's pointer.
 */
void destroy_sakura(void);
