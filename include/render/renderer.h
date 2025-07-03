// render/renderer.h
//
// The main component for rendering and handle SDL's windows
// and renderers.

#pragma once

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "app.h"
#include "misc/vector.h"

/**
 * Represents the enum used to calculate where to actually drawing,
 * this marks the origin of the source sprite. The renderer would calculate
 * based on the origin and the sprite, to find the top left position since
 * SDL always renders from the top left position.
 */
typedef enum
{
  RENDER_ORIGIN_TOP_LEFT,
  RENDER_ORIGIN_TOP_CENTER,
  RENDER_ORIGIN_TOP_RIGHT,
  RENDER_ORIGIN_MIDDLE_LEFT,
  RENDER_ORIGIN_MIDDLE_CENTER,
  RENDER_ORIGIN_MIDDLE_RIGHT,
  RENDER_ORIGIN_BOTTOM_LEFT,
  RENDER_ORIGIN_BOTTOM_CENTER,
  RENDER_ORIGIN_BOTTOM_RIGHT,
} RenderingOriginType;

/**
 * A struct for configuring what to render and where to as a more
 * centralized solution.
 *
 * To get a copy of the default struct, refer to
 * `create_default_rendering_options()`.
 */
typedef struct
{
  SDL_Texture *texture;      // The texture to render.
  SDL_FRect *srcrect;        // The rectangle to render from the texture.
  SDL_FRect *dstrect;        // The rectangle to render the texture to. Height and width is not required.
  RenderingOriginType align; // The alignment for the origin.
  Vector2 scale;             // How much to scale the texture by. Default to 1x1.
  bool flipped_horizontally; // Whether to flip the texture horizontally.
  bool flipped_vertically;   // Whether to flip the texture vertically.
  bool is_tiled;             // Whether to tile the texture repeatedly in the dstrect.
} RenderingOptions;

/**
 * Initializes SDL's window and renderer.
 */
bool init_window_and_renderer(void);

/**
 * Retrieves the app's current window. This is never NULL.
 */
SDL_Window *get_current_window(void);

/**
 * Retrieves the app's current renderer. This is never NULL.
 */
SDL_Renderer *get_current_renderer(void);

/**
 * Retrieves a copy of the default rendering options.
 */
RenderingOptions create_default_rendering_options(void);

/**
 * Renders an aligned texture on screen.
 */
void render_aligned_texture(RenderingOptions options);

/**
 * Starts the rendering process.
 */
void render(AppState *state);

/**
 * Destroys the allocated window and renderer.
 */
void destroy_window_and_renderer(void);
