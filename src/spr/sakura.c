#include "spr/sakura.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "app.h"
#include "engine/collision.h"
#include "engine/physics.h"
#include "misc/vector.h"
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

  int w, h;
  SDL_GetWindowSize(get_current_window(), &w, &h);

  sakura = malloc(sizeof(Sakura));
  sakura->pos.x = w / 2.0;
  sakura->pos.y = h / 2.0;
  sakura->is_moving_down = false;
  sakura->is_moving_left = false;
  sakura->is_moving_right = false;
  sakura->is_moving_up = false;

  // Initialize Sakura's collider
  sakura->collider = malloc(sizeof(Collider));
  sakura->collider->collider_type = COLLIDER_TYPE_CAPSULE;
  sakura->collider->collision_type = COLLISION_DYNAMIC;
  sakura->collider->name = "sakura";

  // Initialize her idle animation textures.
  // Oh no, I'm PirateSoftware with how many magic numbers.
  sakura->idle = malloc(sizeof(Sprite));
  sakura->idle->textures = malloc(sizeof(SpriteTexture) * SAKURA_IDLE_ANIMATION_LENGTH);
  sakura->idle->textures_len = SAKURA_IDLE_ANIMATION_LENGTH;
  sakura->idle->ani_idx = 0;
  sakura->idle->fps = SAKURA_ANIMATION_FPS;
  sakura->idle->dt_accumulator = 0;

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

Sakura *get_sakura(void)
{
  SDL_assert(sakura != NULL);
  return sakura;
}

void update_sakura(AppState *app, double dt)
{
  (void)app;
  Sakura *skr = sakura;

  advance_animation_tick(skr->idle, dt);
}

void fixed_update_sakura(AppState *app)
{
  Sakura *skr = sakura;

  // We can apply gravity if Sakura is not on the ground.
  // Sakura is on the ground if she collides with the ground.
  apply_velocity(&sakura->pos, &sakura->velo);

  // Move Sakura.
  Vector2 skr_dir;
  skr_dir.x = (int)skr->is_moving_right - (int)skr->is_moving_left;
  skr_dir.y = (int)skr->is_moving_down - (int)skr->is_moving_up;
  Vector2 old_pos = skr->pos;
  skr->pos = apply_movement(skr->pos, skr_dir, 5);

  // Move Sakura's bounding box.
  SpriteTexture cur_text = skr->idle->textures[skr->idle->ani_idx];
  double h = (double)cur_text.srcrect.h * 2;
  double w = (double)cur_text.srcrect.w * 2;
  CapsuleCollider old_capsule = skr->collider->capsule;
  skr->collider->capsule.p1.x = skr->pos.x;
  skr->collider->capsule.p1.y = skr->pos.y - h / 2;
  skr->collider->capsule.p2.x = skr->pos.x;
  skr->collider->capsule.p2.y = skr->pos.y + h / 2;
  skr->collider->capsule.r = w / 2;

  // Check for collisions.
  bool can_move = true;
  for (int i = 0; i < app->floor_colliders->length; i++)
  {
    if (check_collision(skr->collider, app->floor_colliders->list[i]))
    {
      // Don't allow move.
      can_move = false;
      break;
    }
  }

  if (!can_move)
  {
    skr->collider->capsule = old_capsule;
    skr->pos = old_pos;
  }
}

void destroy_sakura(void)
{
  if (sakura == NULL)
    return;

  free(sakura->collider);
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
