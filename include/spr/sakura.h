// spr/sakura.h
//
// Includes all information about Sakura's sprites and animations.
// This only provides the necessary data to render, but it does not
// have the rendering job.

#pragma once

#include "misc/vector.h"
#include "spr/sprites.h"

/**
 * Represents the player, Sakura.
 */
typedef struct
{
  Vector2 pos;  // Sakura's position.
  Vector2 dir;  // Sakura's current direction, we only allow x to move.
  Sprite *idle; // The set of sprites for Sakura's idle animation.
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
 * Destroys Sakura's pointer.
 */
void destroy_sakura(void);
