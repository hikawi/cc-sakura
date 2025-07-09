#include "app.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_hints.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include <string.h>

static AppState *appstate = NULL;

AppState *init_app_state(void)
{
    AppState *state = SDL_malloc(sizeof(AppState));

    // Setup frames data
    state->frame_data.frame_count = 0;
    state->frame_data.frame_accum = 0;
    state->frame_data.frame_time = 0;
    state->frame_data.last_frame_tick = SDL_GetTicks();
    state->frame_data.fps = 0;

    // Memset keyboard state to all 0, since it's only bools.
    SDL_memset(&state->keyboard, 0, sizeof(KeyboardStatus));

    // Create window and renderer.
    if (!SDL_CreateWindowAndRenderer(
            APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH,
            APPLICATION_ORIGINAL_HEIGHT, SDL_WINDOW_RESIZABLE, &state->window,
            &state->renderer))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM,
                    "Can't initialize window & renderer. %s", SDL_GetError());
        SDL_free(state);
        return NULL;
    }

    SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND);
    SDL_GetRenderOutputSize(state->renderer, &state->w, &state->h);

    appstate = state;
    return state;
}

AppState *get_app_state(void)
{
    SDL_assert(appstate != NULL);
    return appstate;
}

void destroy_app_state(AppState *state)
{
    SDL_DestroyWindow(state->window);
    SDL_DestroyRenderer(state->renderer);
    SDL_free(state);
    appstate = NULL;
}

char *get_resource_path(const char *subpath)
{
    char *path = SDL_calloc(1024, sizeof(char));

    strcat(path, SDL_GetBasePath());
    strcat(path, "/");
    strcat(path, subpath);

    return path;
}
