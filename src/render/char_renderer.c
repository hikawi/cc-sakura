#include "render/char_renderer.h"
#include "render/collider_renderer.h"
#include "render/renderer.h"
#include "spr/sakura.h"
#include "spr/sprites.h"

void render_sakura(void)
{
  Sakura *skr = get_sakura();

  // Render her sprite.
  SpriteTexture spr = skr->idle->textures[skr->idle->ani_idx];

  SDL_FRect dstrect;
  dstrect.x = (float)skr->pos.x;
  dstrect.y = (float)skr->pos.y;
  dstrect.h = spr.srcrect.h * 2;
  dstrect.w = spr.srcrect.w * 2;

  // Start handling rendering.
  RenderingOptions opts = create_default_rendering_options();
  opts.texture = spr.texture;
  opts.srcrect = &spr.srcrect;
  opts.dstrect = &dstrect;
  opts.flipped_horizontally = skr->is_moving_left;
  opts.align = RENDER_ORIGIN_MIDDLE_CENTER;

  render_aligned_texture(opts);
  render_collider(skr->collider);
}
