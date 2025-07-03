#include "engine/engine.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "engine/events.h"
#include "engine/physics.h"
#include "misc/vector.h"
#include "spr/sakura.h"
#include "spr/sprites.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Calls every frame.
 */
void app_tick(AppState *app, double dt)
{
  (void)app;

  Sakura *skr = get_sakura();

  // Move Sakura.
  Vector2 skr_dir;
  skr_dir.x = (int)skr->is_moving_right - (int)skr->is_moving_left;
  skr_dir.y = (int)skr->is_moving_down - (int)skr->is_moving_up;
  skr->pos = apply_movement(skr->pos, skr_dir, 100, dt);
  advance_animation_tick(skr->idle, dt);
}

/**
 * Calls every fixed tick. Mainly n times per second,
 * based on the fps cap.
 */
void app_fixed_tick(AppState *app)
{
  (void)app;
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
