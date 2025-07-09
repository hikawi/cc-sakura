#include "engine/engine.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "app.h"
#include "engine/events.h"
#include "engine/text.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Calls every frame.
 */
void app_tick(AppState *app, double dt)
{
    (void)app;
    (void)dt;
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
    double dt = (cur_frame - app->frame_data.last_frame_tick) / 1000.0;
    app->frame_data.last_frame_tick = cur_frame;

    if (dt > 0.1)
        dt = 0.1; // Clamp dt to only be 0.1s desync max.

    // Add dt to frame accumulator.
    app->frame_data.frame_accum += dt;

    // Call fixed update if and only if frame_accum has passed
    // enough for 1 frame time (1 / FPS), for 60FPS this is about 16ms.
    while (app->frame_data.frame_accum >= (1.0 / APPLICATION_MAX_FPS))
    {
        app->frame_data.frame_accum -= (1.0 / APPLICATION_MAX_FPS);
        app_fixed_tick(app);
    }

    // Tick every frame.
    app_tick(app, dt);

    // Before rendering, we update the FPS coutner.
    app->frame_data.frame_time += dt;
    app->frame_data.frame_count++;
    if (app->frame_data.frame_time >= 1)
    {
        app->frame_data.fps = app->frame_data.frame_count;
        app->frame_data.frame_count = 0;
        app->frame_data.frame_time -= 1;
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
    case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetRenderOutputSize(app->renderer, &app->w, &app->h);
        break;
    case SDL_EVENT_KEY_DOWN:
        handle_key_down_event(app, event->key);
        break;
    case SDL_EVENT_KEY_UP:
        handle_key_up_event(app, event->key);
        break;
    }
}

bool init_engine(AppState *app)
{
    bool success = true;

    if (!init_font_engine(app))
    {
        success = false;
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to start font engine");
    }

    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Some errors happened while initializing engine");
    }
    return success;
}

void destroy_engine(void)
{
    destroy_font_engine();
}
