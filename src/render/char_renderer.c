#include "render/char_renderer.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "render/renderer.h"
#include "spr/sakura.h"
#include "spr/sprites.h"

void render_sakura(void)
{
  Sakura *skr = get_sakura();
  SDL_Window *window = get_current_window();
  SDL_Renderer *renderer = get_current_renderer();

  // Render her sprite.
  SpriteTexture spr = skr->idle->textures[skr->idle->ani_idx];

  SDL_FRect dstrect;
  dstrect.x = (float)skr->pos.x;
  dstrect.y = (float)skr->pos.y;
  dstrect.h = spr.srcrect.h * 2;
  dstrect.w = spr.srcrect.w * 2;

  // Render flipped if she's facing left.
  if (skr->is_moving_left)
  {
    SDL_RenderTextureRotated(renderer, spr.texture, &spr.srcrect, &dstrect, 0, NULL, SDL_FLIP_HORIZONTAL);
  }
  else
  {
    SDL_RenderTexture(renderer, spr.texture, &spr.srcrect, &dstrect);
  }
}
