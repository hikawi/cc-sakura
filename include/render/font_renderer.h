// render/font_renderer.h
//
// Handles most if not all font-related rendering, using the application's
// main font that I downloaded from the Internet.

#pragma once

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include <stdbool.h>

/**
 * Represents a font face to be loaded.
 */
typedef enum
{
  // The font face that should be the main font to be used.
  FONT_FACE_PIXELIFY_SANS,

  // The debug font face that can also display all Unicode characters.
  FONT_FACE_UNIFONT,
} FontFace;

/**
 * Decides where to draw the font at, using the origin space.
 */
typedef enum
{
  FONT_ORIGIN_TOP_LEFT,
  FONT_ORIGIN_TOP_CENTER,
  FONT_ORIGIN_TOP_RIGHT,
  FONT_ORIGIN_MIDDLE_LEFT,
  FONT_ORIGIN_MIDDLE_CENTER,
  FONT_ORIGIN_MIDDLE_RIGHT,
  FONT_ORIGIN_BOTTOM_LEFT,
  FONT_ORIGIN_BOTTOM_CENTER,
  FONT_ORIGIN_BOTTOM_RIGHT,
} FontOrigin;

/**
 * Initializes and loads up the font faces.
 */
bool init_font_faces(void);

/**
 * Draws some text in the font face on the screen
 * at the destination.
 */
void draw_font(FontFace face, const char *str, SDL_Color color,
               double ori_x, double ori_y, FontOrigin ori_type);

/**
 * Destroys and reclaims memory used by the font faces.
 */
void destroy_font_faces(void);
