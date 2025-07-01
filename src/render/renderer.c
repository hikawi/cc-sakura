#include "render/renderer.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "app.h"
#include "chars/sakura.h"
#include "chars/sprites.h"
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

void init_window_and_renderer(void)
{
  if (!SDL_CreateWindowAndRenderer(APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH,
                                   APPLICATION_ORIGINAL_HEIGHT, SDL_WINDOW_RESIZABLE,
                                   &window, &renderer))
  {
    SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Unable to initiate SDL's window and renderer");
    exit(1);
  }
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

void render_test_screen(void)
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  Sakura *skr = get_sakura();
  Sprite *spr = skr->idle;

  SDL_FRect dstrect;
  dstrect.x = 0;
  dstrect.y = 0;
  dstrect.h = spr->textures[spr->ani_idx].srcrect.h * 4;
  dstrect.w = spr->textures[spr->ani_idx].srcrect.w * 4;
  SDL_RenderTexture(renderer, spr->textures[spr->ani_idx].texture,
                    &spr->textures[spr->ani_idx].srcrect, &dstrect);
  advance_animation_tick(spr);

  SDL_RenderPresent(renderer);
}

void destroy_window_and_renderer(void)
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  window = NULL;
  renderer = NULL;
}
