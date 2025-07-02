#include "render/renderer.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "app.h"
#include "render/char_renderer.h"
#include "render/font_renderer.h"
#include "spr/sakura.h"
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

    draw_font(FONT_FACE_DAYDREAM, buf, color,
              w / 2.0, 10, FONT_ORIGIN_TOP_CENTER);
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

    draw_font(FONT_FACE_RAINY_HEARTS, buf, color,
              w / 2.0, 50, FONT_ORIGIN_TOP_CENTER);
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
