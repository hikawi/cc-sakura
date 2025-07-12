#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
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

    // Here we want to setup a few scenes.
    // Let's setup some scenes to move out of the way.
    SDL_Color black = {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255,
    };
    Scene *black_scr = scene_empty_init(black);

    // The FPS scene.
    Scene *fps =
        scene_fps_init((SDL_Color){.r = 50, .g = 50, .b = 200, .a = 255});
    Scene *fps2 =
        scene_fps_init((SDL_Color){.r = 50, .g = 200, .b = 50, .a = 255});
    fps2->zindex = 1000;

    SceneTransitionInfo info = {
        .scene = black_scr,
        .entry = true,
        .type = TRANSITION_SPLIT_HORIZONTAL,
        .duration = 2,
    };
    scene_mgr_start_transition(&app->scene_mgr, info);

    info.scene = fps;
    info.type = TRANSITION_SLIDE_LEFT;
    info.duration = 2;
    scene_mgr_start_transition(&app->scene_mgr, info);

    info.scene = fps2;
    info.type = TRANSITION_SLIDE_RIGHT;
    info.duration = 4;
    scene_mgr_start_transition(&app->scene_mgr, info);

    // info.scene = black_scr;
    // info.entry = false;
    // info.type = TRANSITION_SPLIT_HORIZONTAL;
    // info.duration = 2;
    // scene_mgr_start_transition(&app->scene_mgr, info);
}
