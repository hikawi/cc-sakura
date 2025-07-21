#include "app.h"
#include "engine/scene.h"
#include "game/game_scenes.h"
#include "misc/threading.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_timer.h"

// Only sets up the first time.
static bool setup = false;

void setup_next_scene(void *userdata)
{
    Scene *main              = scene_000_main();
    SceneTransitionInfo info = {
        .scene    = main,
        .curve    = ANIMATION_CURVE_LINEAR,
        .duration = 2,
        .entry    = true,
        .type     = TRANSITION_SLIDE_DOWN,
    };
    scene_mgr_start_transition(userdata, info);
}

int fake_thread_busy(void *lol)
{
    ThreadData *data = (ThreadData *)lol;

    SDL_LockMutex(data->mutex);
    SDL_Log("Starting thread");

    SDL_LockMutex(data->started_mutex);
    SDL_BroadcastCondition(data->started);
    SDL_UnlockMutex(data->started_mutex);

    for (int i = 0; i < 100; i++)
    {
        SDL_Delay(100);
        SDL_Log("Logging from fake thread: %d", i);
    }

    // First we unlock the mutex. Now the loading scene should know that the
    // mutex is now available.
    SDL_UnlockMutex(data->mutex);

    // The thread has the responsibility to clean up when done.
    // But the mutex is destroyed by the scene_loading, so we don't do it here.
    SDL_DestroyCondition(data->started);
    SDL_DestroyMutex(data->started_mutex);
    SDL_free(data);

    return 0;
}

void scene_setup(void)
{
    if (setup)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "First scenes already set up. Ignoring...");
        return;
    }
    setup = true;

    AppState *app = app_get();

    // Test loading screen.
    ThreadData td = thread_background_init(fake_thread_busy, "fakethread", NULL);

    SDL_DetachThread(td.thread);
    SDL_LockMutex(td.started_mutex);
    SDL_WaitCondition(td.started, td.started_mutex);
    SDL_UnlockMutex(td.started_mutex);

    SceneTransitionInfo info = {
        .scene    = scene_loading(td.mutex, setup_next_scene, &app->scene_mgr),
        .entry    = true,
        .type     = TRANSITION_NONE,
        .duration = 2,
        .curve    = ANIMATION_CURVE_LINEAR,
    };
    scene_mgr_start_transition(&app->scene_mgr, info);
}
