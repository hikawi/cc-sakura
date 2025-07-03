// render/font_renderer.h
//
// Handles most if not all font-related rendering, using the application's
// main font that I downloaded from the Internet.

#pragma once

#include "SDL3/SDL_pixels.h"
#include "misc/vector.h"
#include "render/renderer.h"
#include <stdbool.h>

/**
 * Represents a font face to be loaded.
 */
typedef enum
{
  // The font face that should be the main font to be used.
  FONT_FACE_RAINY_HEARTS,

  // The font face that should be used as secondary font.
  FONT_FACE_DAYDREAM,

  // The debug font face that can also display all Unicode characters.
  FONT_FACE_UNIFONT,
} FontFace;

/**
 * Initializes and loads up the font faces.
 */
bool init_font_faces(void);

/**
 * Draws some text in the font face on the screen
 * at the destination.
 */
void draw_font(FontFace face, const char *str, double ori_x, double ori_y,
               SDL_Color color, RenderingOriginType origin);

/**
 * Destroys and reclaims memory used by the font faces.
 */
void destroy_font_faces(void);
