#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "app.h"
#include "engine/scene.h"
#include "game/game_scenes.h"

// Only sets up the first time.
static bool setup = false;

void scene_setup(void)
{
    if (setup)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "First scenes already set up. Ignoring...");
        return;
    }
    setup = true;

    AppState *app = app_get();

    SDL_Color white = {
        .r = 255,
        .g = 255,
        .b = 255,
        .a = 255,
    };
    SDL_FRect frect = {
        .x = 0,
        .y = 0,
        .w = app->window.w,
        .h = app->window.h,
    };

    // Here we want to setup a few scenes.
    Scene *empty = scene_empty_init(white, frect);
    scene_mgr_push_scene(&app->scene_mgr, empty);

    // The placeholder to push fps in.
    Scene *empty2 = scene_empty_init(white, frect);
    scene_mgr_push_scene(&app->scene_mgr, empty2);

    // The FPS scene.
    Scene *fps =
        scene_fps_init((SDL_Color){.r = 50, .g = 50, .b = 200, .a = 255});

    SceneTransition trans = {
        .from_scene = empty2,
        .to_scene = fps,
        .active = true,
        .destroys_after = true,
        .duration = 2,
        .elapsed = 0,
        .type = TRANSITION_PUSH_LEFT,
    };
    scene_mgr_start_transition(&app->scene_mgr, trans);
}
