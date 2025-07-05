// main.c
//
// The main entrypoint of the program, any events handling, movements, app state
// updating should be delegated to other source files. Do not do actual handling
// in this file.

#include "engine/collision.h"
#define SDL_MAIN_USE_CALLBACKS

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_timer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "engine/engine.h"
#include "render/font_renderer.h"
#include "render/renderer.h"
#include "spr/sakura.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Calls when the program starts.
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  (void)argc;
  (void)argv;

  SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
  SDL_SetAppMetadata(APPLICATION_NAME, APPLICATION_VERSION, APPLICATION_IDENTIFIER);

  // Initializes components
  if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL.");
    return SDL_APP_FAILURE;
  }
  else
  {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Loaded SDL successfully.");
  }

  if (!TTF_Init())
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL_ttf.");
    return SDL_APP_FAILURE;
  }
  else
  {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Loaded SDL_ttf successfully.");
  }

  if (!init_window_and_renderer())
  {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize window and renderer.");
    return SDL_APP_FAILURE;
  }
  else
  {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Loaded SDL window and renderer successfully.");
  }

  // Initialize fonts
  if (!init_font_faces())
  {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize font faces.");
    return SDL_APP_FAILURE;
  }
  else
  {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Loaded application's fonts successfully.");
  }

  // Initialize Sakura
  if (!init_sakura())
  {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize Sakura.");
    return SDL_APP_FAILURE;
  }
  else
  {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Loaded Sakura successfully.");
  }

  AppState *state = malloc(sizeof(AppState));
  *appstate = state;

  // Setup the app state.
  state->last_frame_tick = SDL_GetTicks();
  state->frame_accumulator = 0;

  state->frames_counter = 0;
  state->frames_elapsed = 0;
  state->frames_per_sec = 0;

  {
    state->floor_colliders = create_collider_list();
    SDL_Window *window = get_current_window();
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    Collider *floor = malloc(sizeof(Collider));
    floor->name = "floor";
    floor->collider_type = COLLIDER_TYPE_AABB;
    floor->collision_type = COLLISION_SOLID;
    floor->aabb.x = w / 2.0;
    floor->aabb.y = h / 2.0 + 200;
    floor->aabb.h = 60;
    floor->aabb.w = w;
    add_collider_to_list(state->floor_colliders, floor);

    Collider *wall = malloc(sizeof(Collider));
    wall->name = "wall";
    wall->collider_type = COLLIDER_TYPE_AABB;
    wall->collision_type = COLLISION_SOLID;
    wall->aabb.x = w / 2.0 + 400;
    wall->aabb.y = h / 2.0 + 100;
    wall->aabb.h = 150;
    wall->aabb.w = 70;
    add_collider_to_list(state->floor_colliders, wall);
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Bootstrapped application successfully.");
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
  AppState *state = (AppState *)appstate;

  engine_iterate(state);
  render(state);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  // Always quit the app after.
  if (event->type == SDL_EVENT_QUIT)
  {
    return SDL_APP_SUCCESS;
  }

  // Parse events if it's not a quit command.
  AppState *app = (AppState *)appstate;
  engine_handle_event(app, event);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  AppState *app = (AppState *)appstate;

  destroy_window_and_renderer();
  destroy_sakura();
  free(app->floor_colliders->list[0]);
  destroy_quadtree_node(app->quadtree);
  destroy_collider_list(app->floor_colliders);
  free(appstate);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Closing application with result %d", result);

  TTF_Quit();
  SDL_Quit();
}
