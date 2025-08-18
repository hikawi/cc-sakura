#include "app.h"

#include "engine/collision.h"
#include "engine/scene.h"
#include "game/save.h"
#include "misc/list.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_messagebox.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "SDL3_image/SDL_image.h"

#include <stdlib.h>
#include <string.h>

#define APP_MOUSE_DIMENSION 5

static AppState *appstate = NULL;

AppState *app_init(void)
{
    AppState *state = SDL_malloc(sizeof(AppState));
    appstate        = state;

    // Setup frames data
    state->frame_data.frame_count     = 0;
    state->frame_data.frame_accum     = 0;
    state->frame_data.frame_time      = 0;
    state->frame_data.last_frame_tick = SDL_GetTicks();
    state->frame_data.fps             = 0;

    // Memset keyboard state to all 0, since it's only bools.
    SDL_memset(&state->input, 0, sizeof(state->input));

    // Setup mouse.
    state->input.mouse.collider_shape_type = COLLIDER_SHAPE_TYPE_AABB;
    state->input.mouse.collider_type       = COLLIDER_TYPE_GHOST;
    state->input.mouse.aabb                = (AABBCollider){
                       .x = 0,
                       .y = 0,
                       .w = APP_MOUSE_DIMENSION,
                       .h = APP_MOUSE_DIMENSION,
    };

    // Create window and renderer.
    if (!SDL_CreateWindowAndRenderer(APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH, APPLICATION_ORIGINAL_HEIGHT,
                                     SDL_WINDOW_RESIZABLE, &state->window.window, &state->window.renderer))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "Can't initialize window & renderer. %s", SDL_GetError());
        SDL_free(state);
        return NULL;
    }

    SDL_SetRenderDrawBlendMode(state->window.renderer, SDL_BLENDMODE_BLEND);
    SDL_GetRenderOutputSize(state->window.renderer, &state->window.w, &state->window.h);

    // Create the texture to render into.
    state->scene_mgr.target = SDL_CreateTexture(state->window.renderer, SDL_PIXELFORMAT_RGBA8888,
                                                SDL_TEXTUREACCESS_TARGET, state->window.w, state->window.h);
    if (!state->scene_mgr.target)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Can't create scene rendering target.");
        SDL_DestroyWindow(state->window.window);
        SDL_DestroyRenderer(state->window.renderer);
        SDL_free(state);
        return NULL;
    }
    SDL_SetTextureBlendMode(state->scene_mgr.target, SDL_BLENDMODE_BLEND);

    // Setup the window icon with SDL3, the icon is available in the assets folder.
    char buf[1024] = {0};
    SDL_snprintf(buf, sizeof(buf), "%sassets/icon.ico", SDL_GetBasePath());
    SDL_Surface *icon = IMG_Load(buf);
    if (icon)
    {
        SDL_SetWindowIcon(appstate->window.window, icon);
        SDL_DestroySurface(icon);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Unable to apply a window icon. Error: %s", SDL_GetError());
    }

    // Create scene manager.
    state->scene_mgr.scenes      = list_init();
    state->scene_mgr.transitions = list_init();

    // Set app settings
    state->settings.max_fps              = 60;
    state->settings.scale                = 2;
    state->settings.show_debug_colliders = true;

    state->running = true;

    return state;
}

AppState *app_get(void)
{
    SDL_assert(appstate != NULL);
    return appstate;
}

void app_panic(const char *errmsg)
{
    AppState *state = app_get();
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, APPLICATION_NAME, errmsg, state->window.window);
    state->running = false;
}

void app_destroy(AppState *state)
{
    if (!state)
        return;

    game_settings_save(state->settings);

    scene_mgr_destroy(state->scene_mgr);
    SDL_DestroyWindow(state->window.window);
    SDL_DestroyRenderer(state->window.renderer);
    SDL_free(state);
    appstate = NULL;
}
