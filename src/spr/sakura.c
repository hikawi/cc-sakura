#include "spr/sakura.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "render/renderer.h"
#include "spr/sprites.h"
#include <stdlib.h>

static const char *SAKURA_SPRITESHEET_PATH = "assets/char/sakura.png";
static const int SAKURA_ANIMATION_FPS = 10;
static const int SAKURA_IDLE_ANIMATION_LENGTH = 7;

static Sakura *sakura = NULL;
static SDL_Texture *sakura_txt = NULL;

/**
 * The animation boundaries for each sprite in the sprite sheet, as the sheet
 * contains multiple sprites stuck together.
 *
 * For each sprite (each line), this goes (starting x, starting y, width, height)
 * for the sprite itself only, cut out borders and paddings.
 *
 * For the values that have additions, it means "starting frame box" + "sprite offset",
 * since some sprites might be in the middle, and don't cover the entire 64x64 box.
 */
static const int SAKURA_IDLE_ANIMATION_BOUNDARY[][4] = {
    {0, 0 + 0, 24, 64},
    {24, 0 + 1, 24, 64},
    {48, 0 + 0, 24, 64},
    {72, 0 + 0, 24, 64},
    {96, 0 + 0, 24, 64},
    {120, 0 + 0, 24, 64},
    {144, 0 + 0, 24, 64},
};

/**
 * This function should be idempotent. Multiple invocations should
 * not load the texture again.
 */
bool init_sakura_sprites(void)
{
  SDL_Renderer *renderer = get_current_renderer();

  // Load the texture up.
  if (sakura_txt != NULL)
  {
    SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "Sakura spritesheet already loaded. Skipping.");
    return true; // Don't do anything if already loaded
  }

  SpriteTexture sprtxt = load_texture_from_image(renderer, SAKURA_SPRITESHEET_PATH);
  if (sprtxt.texture == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to load file %s", SAKURA_SPRITESHEET_PATH);
    return false;
  }

  sakura_txt = sprtxt.texture;
  SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "Loaded sakura's spritesheet.");
  return true;
}

bool init_sakura(void)
{
  if (sakura_txt == NULL)
  {
    SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "Sakura's sprites are not loaded yet. Loading them now.");
    if (!init_sakura_sprites())
    {
      return false;
    }
  }
  if (sakura != NULL)
  {
    SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "Sakura already initialized. Skipping.");
    return true;
  }

  sakura = malloc(sizeof(Sakura));
  sakura->pos.x = 0;
  sakura->pos.y = 0;
  sakura->is_moving_down = false;
  sakura->is_moving_left = false;
  sakura->is_moving_right = false;
  sakura->is_moving_up = false;

  sakura->idle = malloc(sizeof(Sprite));
  sakura->idle->textures = malloc(sizeof(SpriteTexture) * SAKURA_IDLE_ANIMATION_LENGTH);
  sakura->idle->textures_len = SAKURA_IDLE_ANIMATION_LENGTH;
  sakura->idle->ani_idx = 0;
  sakura->idle->fps = SAKURA_ANIMATION_FPS;
  sakura->idle->dt_accumulator = 0;

  // Initialize her idle animation textures.
  // Oh no, I'm PirateSoftware with how many magic numbers.
  for (int i = 0; i < SAKURA_IDLE_ANIMATION_LENGTH; i++)
  {
    SpriteTexture *texture = sakura->idle->textures + i;
    const int *data = SAKURA_IDLE_ANIMATION_BOUNDARY[i];

    texture->texture = sakura_txt;
    texture->srcrect.x = data[0];
    texture->srcrect.y = data[1];
    texture->srcrect.w = data[2];
    texture->srcrect.h = data[3];
  }

  return true;
}

Sakura *get_sakura()
{
  SDL_assert(sakura != NULL);
  return sakura;
}

void destroy_sakura(void)
{
  if (sakura == NULL)
    return;

  free(sakura->idle->textures);
  free(sakura->idle);
  free(sakura);
  sakura = NULL;

  if (sakura_txt != NULL)
  {
    SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "Destroyed Sakura but her textures are still loaded. Destroying the textures also.");
    SDL_DestroyTexture(sakura_txt);
    sakura_txt = NULL;
  }
}
