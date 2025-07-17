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
    black_scr->zindex = 1;

    // Force the black screen in.
    SceneTransitionInfo info = {
        .scene = black_scr,
        .entry = true,
        .type = TRANSITION_NONE,
        .duration = 2,
    };
    scene_mgr_start_transition(&app->scene_mgr, info);

    // Force the main screen in.
    Scene *main_000_sc = scene_000_main();
    info.scene = main_000_sc;
    info.entry = true;
    info.type = TRANSITION_NONE;
    scene_mgr_start_transition(&app->scene_mgr, info);

    // Animate the black screen out.
    info.entry = false;
    info.type = TRANSITION_SPLIT_HORIZONTAL;
    info.scene = black_scr;
    scene_mgr_start_transition(&app->scene_mgr, info);
}
