#include "render/renderer.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "app.h"
#include "render/char_renderer.h"
#include "render/font_renderer.h"
#include "spr/sakura.h"
#include <stdbool.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

bool init_window_and_renderer(void)
{
  return SDL_CreateWindowAndRenderer(APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH,
                                     APPLICATION_ORIGINAL_HEIGHT, SDL_WINDOW_RESIZABLE,
                                     &window, &renderer);
}

SDL_Window *get_current_window(void)
{
  SDL_assert(window != NULL);
  return window;
}

SDL_Renderer *get_current_renderer(void)
{
  SDL_assert(renderer != NULL);
  return renderer;
}

RenderingOptions create_default_rendering_options(void)
{
  RenderingOptions opts;

  opts.texture = NULL;
  opts.align = RENDER_ORIGIN_TOP_LEFT;
  opts.scale.x = 1;
  opts.scale.y = 1;
  opts.srcrect = NULL;
  opts.flipped_vertically = false;
  opts.flipped_horizontally = false;
  opts.is_tiled = false;

  return opts;
}

void render_aligned_texture(RenderingOptions options)
{
  SDL_assert(options.texture != NULL);

  // Shift to where we need to start drawing.
  SDL_FRect dstrect;
  if (options.dstrect != NULL)
  {
    dstrect.x = options.dstrect->x;
    dstrect.y = options.dstrect->y;
    dstrect.w = options.dstrect->w;
    dstrect.h = options.dstrect->h;
  }

  if (dstrect.w == 0 || dstrect.h == 0)
  {
    SDL_GetTextureSize(options.texture, &dstrect.w, &dstrect.h);
  }

  switch (options.align)
  {
  case RENDER_ORIGIN_TOP_LEFT: // Do nothing.
    break;
  case RENDER_ORIGIN_TOP_CENTER: // Move origin X to left by half.
    dstrect.x -= dstrect.w / 2;
    break;
  case RENDER_ORIGIN_TOP_RIGHT: // Move origin X to left full.
    dstrect.x -= dstrect.w;
    break;
  case RENDER_ORIGIN_MIDDLE_LEFT: // Move origin Y down by half.
    dstrect.y -= dstrect.h / 2;
    break;
  case RENDER_ORIGIN_MIDDLE_CENTER: // Move both origins by half.
    dstrect.x -= dstrect.w / 2;
    dstrect.y -= dstrect.h / 2;
    break;
  case RENDER_ORIGIN_MIDDLE_RIGHT: // Move X by full and Y by half.
    dstrect.x -= dstrect.w;
    dstrect.y -= dstrect.h / 2;
    break;
  case RENDER_ORIGIN_BOTTOM_LEFT: // Move Y by full.
    dstrect.y -= dstrect.h;
    break;
  case RENDER_ORIGIN_BOTTOM_CENTER: // Move Y full and X half.
    dstrect.x -= dstrect.w / 2;
    dstrect.y -= dstrect.h;
    break;
  case RENDER_ORIGIN_BOTTOM_RIGHT: // Move Y full and X full.
    dstrect.x -= dstrect.w;
    dstrect.y -= dstrect.h;
    break;
  }

  // Scale the texture.
  dstrect.w *= (float)options.scale.x;
  dstrect.h *= (float)options.scale.y;

  if (options.flipped_horizontally || options.flipped_vertically)
  {
    SDL_FlipMode flag = options.flipped_horizontally ? SDL_FLIP_HORIZONTAL : 0;
    flag |= options.flipped_vertically ? SDL_FLIP_VERTICAL : 0;

    SDL_RenderTextureRotated(renderer, options.texture, options.srcrect,
                             &dstrect, 0, NULL, flag);
  }
  else if (options.is_tiled)
  {
    if (options.scale.x != options.scale.y)
    {
      SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Tiling texture's scale is not equal. Using scale=%.2f", options.scale.x);
    }

    SDL_RenderTextureTiled(renderer, options.texture,
                           options.srcrect, (float)options.scale.x, &dstrect);
  }
  else
  {
    SDL_RenderTexture(renderer, options.texture, options.srcrect, &dstrect);
  }
}

void render(AppState *state)
{
  // Clear the screen every frame.
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  // Get the window's width and height.
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  // Render the FPS if enabled for debugging purposes.
  if (APPLICATION_SHOW_FPS)
  {
    char buf[21];
    SDL_snprintf(buf, 20, "%d FPS", state->frames_per_sec);

    SDL_Color color;
    color.r = 20;
    color.g = 20;
    color.b = 255;
    color.a = 255;

    draw_font(FONT_FACE_DAYDREAM, buf, w / 2.0, 10, color, RENDER_ORIGIN_TOP_CENTER);
  }

  // Render Sakura's position.
  {
    Sakura *skr = get_sakura();

    char buf[31];
    SDL_snprintf(buf, 30, "(%.2f, %.2f)", skr->pos.x, skr->pos.y);

    SDL_Color color;
    color.r = 100;
    color.g = 20;
    color.b = 20;
    color.a = 255;

    draw_font(FONT_FACE_RAINY_HEARTS, buf, w / 2.0, 50, color, RENDER_ORIGIN_TOP_CENTER);
  }

  render_sakura();

  // Present the change on screen.
  SDL_RenderPresent(renderer);
}

void destroy_window_and_renderer(void)
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  window = NULL;
  renderer = NULL;
}
