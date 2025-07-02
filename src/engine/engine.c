#include "engine/engine.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

/**
 * Calls every frame.
 */
void app_tick(AppState *app, double dt)
{
}

/**
 * Calls every fixed tick. Mainly n times per second,
 * based on the fps cap.
 */
void app_fixed_tick(AppState *app)
{
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
