#include "engine/engine.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "app.h"
#include "engine/scene.h"
#include "engine/text.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
        scene_mgr_phys_tick(&app->scene_mgr);
    }

    // Tick every frame.
    scene_mgr_tick(&app->scene_mgr, dt);

    // Before rendering, we update the FPS counter.
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
    // Handle the event itself.
    switch (event->type)
    {
    case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetRenderOutputSize(app->window.renderer, &app->window.w,
                                &app->window.h);
        SDL_DestroyTexture(app->scene_mgr.target);
        app->scene_mgr.target = SDL_CreateTexture(
            app->window.renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, app->window.w, app->window.h);

        // Reapply render target.
        if (SDL_GetRenderTarget(app->window.renderer) != NULL)
            SDL_SetRenderTarget(app->window.renderer, app->scene_mgr.target);
        else
            SDL_SetRenderTarget(app->window.renderer, NULL);
        SDL_SetTextureBlendMode(app->scene_mgr.target, SDL_BLENDMODE_BLEND);

        SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Resized rendering target.");
        break;
    case SDL_EVENT_KEY_DOWN:
        app->input.keyboard[event->key.scancode] = true;
        break;
    case SDL_EVENT_KEY_UP:
        app->input.keyboard[event->key.scancode] = false;
        break;
    }
}

void engine_render(AppState *app)
{
    // Clear the renderer.
    SDL_SetRenderDrawColor(app->window.renderer, 255, 255, 255, 255);
    SDL_RenderClear(app->window.renderer);

    // Render the scenes I guess
    scene_mgr_draw(&app->scene_mgr);

    // Present.
    SDL_RenderPresent(app->window.renderer);
}

bool engine_init(AppState *app)
{
    bool success = true;

    if (!font_engine_init(app))
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

void engine_destroy(void)
{
    font_engine_destroy();
}
