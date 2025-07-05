#include "engine/engine.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "engine/collision.h"
#include "engine/events.h"
#include "render/renderer.h"
#include "spr/sakura.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Calls every frame.
 */
void app_tick(AppState *app, double dt)
{
  update_sakura(app, dt);
}

/**
 * Calls every fixed tick. Mainly n times per second,
 * based on the fps cap.
 */
void app_fixed_tick(AppState *app)
{
  int w, h;
  SDL_GetWindowSize(get_current_window(), &w, &h);

  if (app->quadtree)
    destroy_quadtree_node(app->quadtree);

  // For each fixed tick, we create a quadtree to pass around.
  // Populate and subdivide.
  app->quadtree = create_quadtree_node((AABBCollider){w / 2.0, h / 2.0, (double)w, (double)h}, 0);
  join_collider_lists(app->quadtree->colliders, app->floor_colliders);
  add_collider_to_list(app->quadtree->colliders, get_sakura()->collider);
  subdivide_quadtree(app->quadtree);

  // Call fixed updates.
  fixed_update_sakura(app);
}

void engine_iterate(AppState *app)
{
  // Calculate delta time
  Uint64 cur_frame = SDL_GetTicks();
  double dt = (cur_frame - app->last_frame_tick) / 1000.0;
  app->last_frame_tick = cur_frame;

  if (dt > 0.1)
    dt = 0.1; // Clamp dt to only be 0.1s desync max.

  // Add dt to frame accumulator.
  app->frame_accumulator += dt;

  // Call fixed update if and only if frame_accum has passed
  // enough for 1 frame time (1 / FPS), for 60FPS this is about 16ms.
  while (app->frame_accumulator >= (1.0 / APPLICATION_MAX_FPS))
  {
    app->frame_accumulator -= (1.0 / APPLICATION_MAX_FPS);
    app_fixed_tick(app);
  }

  // Tick every frame.
  app_tick(app, dt);

  // Before rendering, we update the FPS coutner.
  app->frames_elapsed += dt;
  app->frames_counter++;
  if (app->frames_elapsed >= 1)
  {
    app->frames_per_sec = app->frames_counter;
    app->frames_counter = 0;
    app->frames_elapsed -= 1;
  }
}

void engine_handle_event(AppState *app, SDL_Event *event)
{
  // Retrieve the event data and log it out.
  int buflen = SDL_GetEventDescription(event, NULL, 0);
  char *buf = malloc(sizeof(char) * ((unsigned long)buflen + 1));
  SDL_GetEventDescription(event, buf, buflen);
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", buf);
  free(buf);

  // Handle the event itself.
  switch (event->type)
  {
  case SDL_EVENT_KEY_DOWN:
    handle_key_down_event(app, event->key);
    break;
  case SDL_EVENT_KEY_UP:
    handle_key_up_event(app, event->key);
    break;
  }
}
