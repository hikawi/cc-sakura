// spr/sprites.h
//
// Includes commonly used structs related to sprites and animations.

#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

/**
 * Represents a singular texture to be painted. This can include sprite sheets,
 * then the texture would represent the entire sheet.
 *
 * When you want to use this for sprite sheets, multiple can be made, holding the
 * same pointer to the same texture, but srcrects would be different.
 */
typedef struct
{
  SDL_Texture *texture; // Pointer to an array of textures. This can be shared.
  SDL_FRect srcrect;    // The FRect that denotes where in the texture to draw.
} SpriteTexture;

/**
 * A structure that holds a pointer to an array of textures that can be rendered
 * by SDL, with each texture having its own original height and width.
 */
typedef struct
{
  SpriteTexture *textures;
  uint32_t textures_len;
  uint8_t ani_idx;
  uint8_t last_ani_tick;
  uint8_t fps;
} Sprite;

/**
 * Loads a texture from disk and draw it on a GPU-optimized texture plane.
 *
 * This always returns a value. Check the `texture` property of the struct,
 * it's only non-null if it was successful.
 */
SpriteTexture load_texture_from_image(SDL_Renderer *renderer, const char *file);

/**
 * Advances the animation index of a sprite if it's time to do so.
 *
 * It's time when the last_ani_tick is at equal or greater than its fps.
 */
void advance_animation_tick(Sprite *spr);
