// game/scenes/scene_loading.c
//
// Represents the loading scene with a mutex.

#include "app.h"
#include "engine/renderer.h"
#include "engine/scene.h"
#include "engine/sprite.h"
#include "game/game_scenes.h"
#include "misc/hashmap.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

typedef enum
{
    SPR_SAKURA_FLOWER,
} SceneLoadingSprite;

typedef struct
{
    SDL_Mutex *mutex;
    uint64_t start_time;
    double min;
    void (*callback)(void *);
    void *userdata;
} SceneLoadingData;

void scene_loading_oninit(Scene *s)
{
    Sprite *spr = sprite_load("object");
    if (!spr)
    {
        app_panic("Couldn't load object sprite for loading scene.");
        return;
    }

    hash_map_put(s->sprites, SPR_SAKURA_FLOWER, spr);
    sprite_select_tag(spr, "sakura");

    // Mark the start time
    SceneLoadingData *data = (SceneLoadingData *)s->data;
    data->start_time       = SDL_GetTicks();
}

void scene_loading_ontick(Scene *s, double dt)
{
    AppState *app          = app_get();
    SceneLoadingData *data = (SceneLoadingData *)s->data;

    double elapsed = (SDL_GetTicks() - data->start_time) / 1000.0;
    if (elapsed >= data->min && data->mutex && SDL_TryLockMutex(data->mutex))
    {
        SDL_Log("Succeeded in locking mutex, transitioning out.");
        SDL_UnlockMutex(data->mutex);
        SDL_DestroyMutex(data->mutex);

        data->mutex = NULL;

        // Transition itself out.
        SceneTransitionInfo trans = {
            .scene    = s,
            .curve    = ANIMATION_CURVE_LINEAR,
            .duration = 2,
            .entry    = false,
            .type     = TRANSITION_FADE,
        };
        scene_mgr_start_transition(&app->scene_mgr, trans);

        // Callbacks.
        SDL_Log("Running a callback on %p", (void *)data->callback);
        if (data->callback)
        {
            data->callback(data->userdata);
        }
    }

    // Regardless of failing or succeeding, we keep rendering, so even when
    // fading out, the animation stays smooth.
    Sprite *spr_object = (Sprite *)hash_map_get(s->sprites, SPR_SAKURA_FLOWER);
    spr_object->rotation += 64 * SDL_PI_D * dt;
}

void scene_loading_ondraw(Scene *s, SDL_Renderer *renderer)
{
    AppState *appstate = app_get();
    WindowStatus winst = appstate->window;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Vector2 botright;
    botright.x = winst.w - 64;
    botright.y = winst.h - 64;

    Sprite *spr_object = (Sprite *)hash_map_get(s->sprites, SPR_SAKURA_FLOWER);

    SpriteRenderProperties props;
    props.renderer = renderer;
    props.pos      = botright;
    props.spr      = spr_object;
    props.origin   = RENDER_ORIGIN_BOTTOM_RIGHT;
    sprite_render(props);
}

void scene_loading_ondestroy(Scene *s)
{
    SceneLoadingData *data = (SceneLoadingData *)s->data;

    if (data->mutex)
    {
        SDL_DestroyMutex(data->mutex);
        data->mutex = NULL;
    }
}

Scene *scene_loading(SDL_Mutex *mutex, double min, void (*callback)(void *), void *userdata)
{
    SDL_assert(mutex != NULL);
    SDL_assert(min > 0);

    Scene *s               = scene_init();
    SceneLoadingData *data = SDL_malloc(sizeof(SceneLoadingData));

    if (!s || !data)
    {
        app_panic("Unable to allocate memory for scene Loading");
        SDL_free(s);
        SDL_free(data);
        return NULL;
    }

    data->mutex      = mutex;
    data->start_time = 0;
    data->min        = min;
    data->callback   = callback;
    data->userdata   = userdata;

    s->captures_focus = true;
    s->data           = data;
    s->oninit         = scene_loading_oninit;
    s->ontick         = scene_loading_ontick;
    s->ondraw         = scene_loading_ondraw;
    s->ondestroy      = scene_loading_ondestroy;

    return s;
}
