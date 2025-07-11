#include "app.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "engine/scene.h"
#include "misc/list.h"
#include "misc/stack.h"
#include <string.h>

static AppState *appstate = NULL;

AppState *app_init(void)
{
    AppState *state = SDL_malloc(sizeof(AppState));

    // Setup frames data
    state->frame_data.frame_count = 0;
    state->frame_data.frame_accum = 0;
    state->frame_data.frame_time = 0;
    state->frame_data.last_frame_tick = SDL_GetTicks();
    state->frame_data.fps = 0;

    // Memset keyboard state to all 0, since it's only bools.
    SDL_memset(&state->input, 0, sizeof(state->input));

    // Create window and renderer.
    if (!SDL_CreateWindowAndRenderer(
            APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH,
            APPLICATION_ORIGINAL_HEIGHT, SDL_WINDOW_RESIZABLE,
            &state->window.window, &state->window.renderer))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM,
                    "Can't initialize window & renderer. %s", SDL_GetError());
        SDL_free(state);
        return NULL;
    }

    SDL_SetRenderDrawBlendMode(state->window.renderer, SDL_BLENDMODE_BLEND);
    SDL_GetRenderOutputSize(state->window.renderer, &state->window.w,
                            &state->window.h);

    // Create the texture to render into.
    state->scene_mgr.target = SDL_CreateTexture(
        state->window.renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, state->window.w, state->window.h);
    if (!state->scene_mgr.target)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER,
                     "Can't create scene rendering target.");
        SDL_DestroyWindow(state->window.window);
        SDL_DestroyRenderer(state->window.renderer);
        SDL_free(state);
        return NULL;
    }
    SDL_SetTextureBlendMode(state->scene_mgr.target, SDL_BLENDMODE_BLEND);

    // Create scene manager.
    state->scene_mgr.scenes = stack_init(APPLICATION_MAX_SCENE_COUNT);
    state->scene_mgr.transitions = list_init();

    appstate = state;
    return state;
}

AppState *app_get(void)
{
    SDL_assert(appstate != NULL);
    return appstate;
}

void app_destroy(AppState *state)
{
    if (!state)
        return;

    for (int i = 0; i < state->scene_mgr.scenes->length; i++)
    {
        scene_destroy(state->scene_mgr.scenes->items[i]);
    }
    stack_destroy(state->scene_mgr.scenes);

    for (int i = 0; i < (int)state->scene_mgr.transitions->length; i++)
    {
        SceneTransition *trans = state->scene_mgr.transitions->items[i];
        SDL_DestroyTexture(trans->from_txt);
        SDL_DestroyTexture(trans->to_txt);
        SDL_free(trans);
    }
    list_destroy(state->scene_mgr.transitions);

    SDL_DestroyWindow(state->window.window);
    SDL_DestroyRenderer(state->window.renderer);
    SDL_free(state);
    appstate = NULL;
}

char *app_res_path(const char *subpath)
{
    char *path = SDL_calloc(1024, sizeof(char));

    SDL_strlcat(path, SDL_GetBasePath(), 1024);
    SDL_strlcat(path, "/", 1024);
    SDL_strlcat(path, subpath, 1024);

    return path;
}
