#include "spr/sakura.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
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
  sakura->velo.x = 0;
  sakura->velo.y = 0;
  sakura->is_on_ground = false;

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

/**
 * Moves Sakura's collider to align with her current position and sprite.
 */
void move_sakura_collider(void)
{
  Sakura *skr = sakura;

  SpriteTexture cur_text = skr->idle->textures[skr->idle->ani_idx];
  double h = (double)cur_text.srcrect.h * 2;
  double w = (double)cur_text.srcrect.w * 2;
  skr->collider->capsule.p1.x = skr->pos.x;
  skr->collider->capsule.p1.y = skr->pos.y - h / 2;
  skr->collider->capsule.p2.x = skr->pos.x;
  skr->collider->capsule.p2.y = skr->pos.y + h / 2;
  skr->collider->capsule.r = w / 2 - 2;
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

  // Apply gravity.
  skr->velo.y += SAKURA_GRAVITY;

  // Apply movements based on keyboard states for Sakura.
  skr->velo.x = 0;
  if (app->keyboard.key_a)
    skr->velo.x -= SAKURA_SPEED;
  if (app->keyboard.key_d)
    skr->velo.x += SAKURA_SPEED;

  // Jump if she is on the ground and key space pressed.
  if (app->keyboard.key_space && skr->is_on_ground)
    skr->velo.y = SAKURA_JUMP_VELOCITY;
  skr->is_on_ground = false;

  ColliderList *list;

  // Move on the X axis discretely and check.
  // We only push her back on the X axis on the most prominent collision.
  skr->pos.x += skr->velo.x;
  list = create_collider_list();
  move_sakura_collider();
  query_quadtree_node(app->quadtree, skr->collider, list);
  Collision deepest_x_info = {.is_colliding = false};
  for (int i = 0; i < list->length; i++)
  {
    if (list->list[i] == skr->collider)
      continue;

    Collision info = check_collision(list->list[i], skr->collider);
    if (info.is_colliding)
    {
      // Ensure it's primarily an X-axis collision (important for corners)
      if (SDL_fabs(info.normal.x) > SDL_fabs(info.normal.y))
        if (!deepest_x_info.is_colliding || info.depth > deepest_x_info.depth)
          deepest_x_info = info;
    }
  }

  // Resolve X collisions.
  if (deepest_x_info.is_colliding)
  {
    skr->pos = add_vector2(skr->pos, scale_vector2(deepest_x_info.normal, deepest_x_info.depth));
    skr->velo.x = 0;
    move_sakura_collider();
  }
  destroy_collider_list(list);

  // Move on the Y axis discretely and check.
  skr->pos.y += skr->velo.y;
  list = create_collider_list();
  move_sakura_collider();
  query_quadtree_node(app->quadtree, skr->collider, list);
  Collision deepest_y_info = {.is_colliding = false};
  for (int i = 0; i < list->length; i++)
  {
    if (list->list[i] == skr->collider)
      continue;

    Collision info = check_collision(list->list[i], skr->collider);
    if (info.is_colliding)
    {
      if (SDL_fabs(info.normal.y) > SDL_fabs(info.normal.x))
        if (!deepest_y_info.is_colliding || info.depth > deepest_y_info.depth)
          deepest_y_info = info;
    }
  }

  // Resolve Y collisions.
  if (deepest_y_info.is_colliding)
  {
    skr->pos = add_vector2(skr->pos, scale_vector2(deepest_y_info.normal, deepest_y_info.depth));
    skr->velo.y = 0;
    move_sakura_collider();
    SDL_Log("Resolving collision of depth %.2f at (%.2f, %.2f)", deepest_y_info.depth, deepest_y_info.normal.x, deepest_y_info.normal.y);
  }
  destroy_collider_list(list);

  // The on-ground check.
  // There's a chance Sakura's Y has moved. We need to refresh the list.
  list = create_collider_list();
  move_sakura_collider();
  query_quadtree_node(app->quadtree, skr->collider, list);

  // Find the feet collision. Since she's a capsule, she can be rotated arbitrarily, therefore,
  // we get her feet by averaging her x axis, and take the larger of her y coords + alpha.
  AABBCollider feet;
  feet.x = (skr->collider->capsule.p1.x + skr->collider->capsule.p2.x) / 2;
  feet.y = SDL_max(skr->collider->capsule.p1.y, skr->collider->capsule.p2.y) + APPLICATION_SCALE;
  feet.h = APPLICATION_SCALE;
  feet.w = skr->collider->capsule.r;

  for (int i = 0; i < list->length; i++)
  {
    Collision info;
    if (list->list[i] == skr->collider || list->list[i]->collision_type != COLLISION_SOLID)
      continue;

    // Make collider
    Collider collider;
    collider.collider_type = COLLIDER_TYPE_AABB;
    collider.collision_type = COLLISION_DYNAMIC;
    collider.name = "sakura-feet";
    collider.aabb = feet;

    info = check_collision(list->list[i], &collider);
    if (info.is_colliding && info.normal.y < 0)
    {
      skr->is_on_ground = true;
      skr->velo.y = 0;
      break;
    }
  }
  destroy_collider_list(list);
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
