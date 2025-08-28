#include "engine/scene.h"

#include "app.h"
#include "engine/sprite.h"
#include "misc/hashmap.h"
#include "misc/list.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"

#include <string.h>

/**
 * \brief Computes the ease-out bounce relative progress
 *
 * Given the absolute progress of the animation, ranging from 0-1, what is the relative
 * progress when mapped on an ease-out-bounce curve.
 *
 * \param progress the absolute progress
 * \returns the relative progress
 */
double ease_out_bounce(const double progress)
{
    const double n1 = 7.5625;
    const double d1 = 2.75;

    if (progress < 1 / d1)
    {
        return n1 * progress * progress;
    }
    else if (progress < 2 / d1)
    {
        const double v = progress - 1.5 / d1;
        return n1 * v * v + 0.75;
    }
    else if (progress < 2.5 / d1)
    {
        const double v = progress - 2.25 / d1;
        return n1 * v * v + 0.9375;
    }

    const double v = progress - 2.625 / d1;
    return n1 * v * v + 0.984375;
}

/**
 * \brief Computes the relative progress using a curve function.
 *
 * Thanks https://easings.net.
 *
 * \param curve The curve to go along
 * \param progress The true progress, based on time
 */
double animation_curve_calc(const AnimationCurve curve, const double progress)
{
    switch (curve)
    {
    case ANIMATION_CURVE_LINEAR:
        return progress;
    case ANIMATION_CURVE_EASE_IN:
        return 1 - SDL_cos((progress * SDL_PI_D) / 2);
    case ANIMATION_CURVE_EASE_OUT:
        return SDL_sin((progress * SDL_PI_D) / 2);
    case ANIMATION_CURVE_EASE_IN_OUT:
        return -(SDL_cos(SDL_PI_D * progress) - 1) / 2;
    case ANIMATION_CURVE_EASE_IN_BOUNCE:
        return 1 - (ease_out_bounce(1 - progress));
    case ANIMATION_CURVE_EASE_OUT_BOUNCE:
        return ease_out_bounce(progress);
    case ANIMATION_CURVE_EASE_IN_OUT_BOUNCE:
        return progress < 0.5 ? (1 - ease_out_bounce(1 - 2 * progress)) / 2
                              : (1 + ease_out_bounce(2 * progress - 1)) / 2;
    }

    return progress;
}

/**
 * Define the scene transition here to hide it away from engine users (me).
 */
typedef struct
{
    SceneTransitionInfo info;
    SDL_Texture *texture;
    double elapsed;
    bool active;
} SceneTransition;

Scene *scene_init(void)
{
    Scene *scene = SDL_calloc(1, sizeof(Scene));
    scene->zindex = 0;
    scene->quadtree = NULL;
    scene->data = NULL;
    scene->enabled = true;
    scene->accepting_signals = true;
    scene->captures_focus = false;
    scene->stops_propagation = false;
    scene->moved_colliders = list_init();
    scene->colliders = hash_map_init();
    scene->sprites = hash_map_init();
    return scene;
}

void scene_destroy(Scene *scene)
{
    if (!scene)
        return;

    if (scene->ondestroy)
    {
        scene->ondestroy(scene);
    }
    if (scene->quadtree)
    {
        quadtree_destroy(scene->quadtree);
        scene->quadtree = NULL;
    }
    if (scene->data)
    {
        SDL_free(scene->data);
        scene->data = NULL;
    }

    if (scene->colliders)
    {
        uint32_t *keys;
        void **vals;
        hash_map_iterate(scene->colliders, &keys, (const void ***)&vals);

        for (uint32_t i = 0; i < scene->colliders->size; i++)
        {
            SDL_free(vals[i]);
        }
        SDL_free(keys);
        SDL_free(vals);
        hash_map_destroy(scene->colliders);
        scene->colliders = NULL;
    }

    if (scene->sprites)
    {
        uint32_t *keys;
        void **vals;
        hash_map_iterate(scene->sprites, &keys, (const void ***)&vals);

        for (uint32_t i = 0; i < scene->sprites->size; i++)
        {
            sprite_destroy(vals[i]);
        }
        SDL_free(keys);
        SDL_free(vals);
        hash_map_destroy(scene->sprites);
        scene->sprites = NULL;
    }

    if (scene->moved_colliders)
    {
        list_destroy(scene->moved_colliders);
        scene->moved_colliders = NULL;
    }

    SDL_free(scene);
}

void scene_transition_destroy(SceneTransition *trans)
{
    if (!trans)
        return;

    SDL_DestroyTexture(trans->texture);
    SDL_free(trans);
}

/**
 * Purges all inactive transitions.
 */
void scene_mgr_purge_transitions(SceneManager *mgr)
{
    uint32_t i = 0;

    while (i < mgr->transitions->length)
    {
        SceneTransition *trans = (SceneTransition *)mgr->transitions->items[i];
        if (!trans->active)
        {
            list_remove_at(mgr->transitions, i);
            scene_transition_destroy(trans);
        }
        else
        {
            i++;
        }
    }
}

/**
 * Just makes it so the transitioning scenes render their scenes on the provided
 * texture.
 */
void scene_mgr_transition_render(SceneManager *mgr, SceneTransition *trans)
{
    (void)mgr;

    AppState *appstate = app_get();
    WindowStatus win = appstate->window;

    // Render it to the texture layer.
    SDL_Texture *target = SDL_GetRenderTarget(win.renderer);
    SDL_SetRenderTarget(win.renderer, trans->texture);
    if (trans->info.scene->ondraw)
    {
        trans->info.scene->ondraw(trans->info.scene, win.renderer);
    }
    SDL_SetRenderTarget(win.renderer, target);
}

/**
 * Renders the none transition.
 */
void scene_mgr_transition_render_none(SceneManager *mgr, SceneTransition *trans, double progress)
{
    (void)progress;

    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Instantly end the transition.
    trans->elapsed = trans->info.duration + 1;

    // Render once.
    SDL_RenderTexture(win.renderer, trans->texture, NULL, NULL);
}

/**
 * Renders the fading transition.
 */
void scene_mgr_transition_render_fade(SceneManager *mgr, SceneTransition *trans, double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Then we start the fading based on the progress.
    // progress = 0 is full opacity if it is an exit transition,
    // otherwise no opacity if it is an entry transition.
    if (trans->info.entry)
    {
        SDL_SetTextureAlphaModFloat(trans->texture, (float)progress);
    }
    else
    {
        SDL_SetTextureAlphaModFloat(trans->texture, 1 - (float)progress);
    }

    // Render it to the target.
    SDL_RenderTexture(win.renderer, trans->texture, NULL, NULL);

    // Clean up after yourselves.
    SDL_SetTextureAlphaModFloat(trans->texture, 1);
}

/**
 * Animates the sliding left transition. This will animate the scene sliding to
 * move out of the screen or the scene sliding in to move inside of the screen.
 */
void scene_mgr_transition_render_slide_left(SceneManager *mgr, SceneTransition *trans, double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Render the scene in an offset to animate it moving, depending on the
    // transition type.
    SDL_FRect dstrect = {
        .x = trans->info.entry ? (float)win.w : 0,
        .y = 0,
        .h = (float)win.h,
        .w = (float)win.w,
    };
    dstrect.x -= (float)(progress * win.w);
    SDL_RenderTexture(win.renderer, trans->texture, NULL, &dstrect);
}

/**
 * Animates the sliding right transition. This will animate the scene sliding to
 * move out of the screen or the scene sliding in to move inside of the screen.
 */
void scene_mgr_transition_render_slide_right(SceneManager *mgr, SceneTransition *trans, double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Render the scene in an offset to animate it moving, depending on the
    // transition type.
    SDL_FRect dstrect = {
        .x = trans->info.entry ? (float)-win.w : 0,
        .y = 0,
        .h = (float)win.h,
        .w = (float)win.w,
    };
    dstrect.x += (float)(progress * win.w);
    SDL_RenderTexture(win.renderer, trans->texture, NULL, &dstrect);
}
/**
 * Animates the sliding up transition. This will animate the scene sliding to
 * move out of the screen or the scene sliding in to move inside of the screen.
 */
void scene_mgr_transition_render_slide_up(SceneManager *mgr, SceneTransition *trans, double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Render the scene in an offset to animate it moving, depending on the
    // transition type.
    SDL_FRect dstrect = {
        .x = 0,
        .y = trans->info.entry ? (float)win.h : 0,
        .h = (float)win.h,
        .w = (float)win.w,
    };
    dstrect.y -= (float)(progress * win.h);
    SDL_RenderTexture(win.renderer, trans->texture, NULL, &dstrect);
}
/**
 * Animates the sliding down transition. This will animate the scene sliding to
 * move out of the screen or the scene sliding in to move inside of the screen.
 */
void scene_mgr_transition_render_slide_down(SceneManager *mgr, SceneTransition *trans, double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Render the scene in an offset to animate it moving, depending on the
    // transition type.
    SDL_FRect dstrect = {
        .x = 0,
        .y = trans->info.entry ? (float)-win.h : 0,
        .h = (float)win.h,
        .w = (float)win.w,
    };
    dstrect.y += (float)(progress * win.h);
    SDL_RenderTexture(win.renderer, trans->texture, NULL, &dstrect);
}

/**
 * Animates the sequence of splitting horizontally.
 */
void scene_mgr_transition_render_split_horiz(SceneManager *mgr, SceneTransition *trans, double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;

    scene_mgr_transition_render(mgr, trans);

    // Split the texture into two.
    SDL_FRect topsrc = {
        .x = 0,
        .y = 0,
        .h = (float)win.h / 2.0f,
        .w = (float)win.w,
    };
    SDL_FRect btmsrc = {
        .x = 0,
        .y = (float)win.h / 2.0f,
        .h = (float)win.h / 2.0f,
        .w = (float)win.w,
    };

    // If it's an entry then the split should close in, otherwise it should
    // split outwards. Either way, we need to render each half at different
    // offsets.
    SDL_FRect topdst = topsrc;
    SDL_FRect btmdst = btmsrc;

    if (trans->info.entry)
    {
        // Close in.
        topdst.y = (float)-win.h / 2.0f;
        topdst.y += (float)progress * topdst.h;
        btmdst.y = (float)win.h;
        btmdst.y -= (float)progress * topdst.h;
    }
    else
    {
        // Recede.
        topdst.y = 0;
        topdst.y -= (float)progress * topdst.h;
        btmdst.y = (float)win.h / 2.0f;
        btmdst.y += (float)progress * topdst.h;
    }

    SDL_RenderTexture(win.renderer, trans->texture, &topsrc, &topdst);
    SDL_RenderTexture(win.renderer, trans->texture, &btmsrc, &btmdst);
}

/**
 * Applies a transition effect on the renderer.
 */
void scene_mgr_handle_transition(SceneManager *mgr, SceneTransition *trans, double dt)
{

    // If the transition is inactive, then do nothing.
    if (!trans->active)
    {
        return;
    }

    // Here the transition is still in play.
    // Handle the transition ending.
    if (trans->elapsed > trans->info.duration)
    {
        trans->active = false; // Mark for deletion.

        // Do the actual scene bootstrapping or ending.
        // If it is an entry transition, then we want to start it. Otherwise, we
        // want to destroy it.
        if (trans->info.entry)
        {
            if (trans->info.scene->onstart)
            {
                trans->info.scene->onstart(trans->info.scene);
            }
        }
        else
        {
            list_remove(mgr->scenes, trans->info.scene);
            scene_destroy(trans->info.scene);
        }

        return;
    }

    // Finally, update the transition state.
    // The rendering part is delegated to `scene_mgr_draw`.
    trans->elapsed += dt;
}

void scene_mgr_tick(SceneManager *mgr, double dt)
{
    // Handle the transitions.
    for (int i = 0; i < (int)mgr->transitions->length; i++)
    {
        SceneTransition *trans = (SceneTransition *)mgr->transitions->items[i];
        scene_mgr_handle_transition(mgr, trans, dt);
    }

    // We want to let top scenes capture focus if needed. So we iterate from top
    // to bottom.
    bool focus_captured = false;
    for (int i = (int)mgr->scenes->length - 1; i >= 0; i--)
    {
        Scene *scene = (Scene *)mgr->scenes->items[i];
        if (!scene)
            continue;

        // A scene only receives a tick if focus is not captured yet, and it is
        // enabled.
        if (scene->enabled && !focus_captured && scene->ontick)
        {
            scene->ontick(scene, dt);
        }
        focus_captured = focus_captured || scene->captures_focus;
    }

    scene_mgr_purge_transitions(mgr);
}

void scene_mgr_phys_tick(SceneManager *mgr)
{
    // The scene is a stack emplaced at back. Therefore running 1 -> length is a
    // bottom to top approach.
    bool focus_captured = false;
    for (int i = (int)mgr->scenes->length - 1; i >= 0; i--)
    {
        Scene *scene = (Scene *)mgr->scenes->items[i];
        if (!scene)
            return;

        // A scene gets a physical tick if focus is not captured and the scene
        // is enabled.
        if (scene->enabled && scene->onphystick && !focus_captured)
        {
            scene->onphystick(scene);
        }
        focus_captured = focus_captured || scene->captures_focus;
    }
}

void scene_mgr_start_transition(SceneManager *mgr, SceneTransitionInfo info)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;

    if (info.duration < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't have a negative duration transition.");
        return;
    }

    if (!info.scene)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't have a transition with a null scene.");
        return;
    }

    SceneTransition *trans = SDL_calloc(1, sizeof(SceneTransition));
    trans->info = info;
    trans->elapsed = 0;
    trans->active = true;
    trans->texture = SDL_CreateTexture(win.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, win.w, win.h);

    if (!trans->texture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to allocate texture for transition");
        SDL_free(trans);
        return;
    }

    // If it is an inward transitioning, we initialize it.
    if (info.entry)
    {
        if (info.scene->oninit)
            info.scene->oninit(info.scene);
        list_add(mgr->scenes, info.scene);
        scene_mgr_reorder(mgr);
    }
    list_add(mgr->transitions, trans);
}

/**
 * Retrieves the active transition for the scene. This is definitely a slow
 * operation since it's doing this for every single scene. But if there are
 * maximum like 5 scenes at play, and therefore 5 transitions max, it's 25
 * pointer checks per frame, which is negligible compared to loading textures.
 */
SceneTransition *scene_get_active_transition(SceneManager *mgr, Scene *scene)
{
    for (int i = 0; i < (int)mgr->transitions->length; i++)
    {
        SceneTransition *trans = (SceneTransition *)mgr->transitions->items[i];
        if (trans->active && trans->info.scene == scene)
            return trans;
    }

    return NULL;
}

int scene_comparator(const void *a, const void *b)
{
    Scene *l = (Scene *)a;
    Scene *r = (Scene *)b;
    return l->zindex - r->zindex;
}

void scene_mgr_reorder(SceneManager *mgr)
{
    list_sort(mgr->scenes, scene_comparator);
}

void scene_mgr_draw(SceneManager *mgr)
{
    AppState *appstate = app_get();
    SDL_Renderer *renderer = appstate->window.renderer;

    // We're gonna go through each scene in the current active stack and render
    // them. We assume it's in correct scenes.
    for (int i = 0; i < (int)mgr->scenes->length; i++)
    {
        Scene *scene = (Scene *)mgr->scenes->items[i];

        // If a scene is not enabled, we don't render them anyway.
        if (!scene || !scene->enabled)
            continue;

        // Reset the renderer before starting
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_SetRenderTarget(renderer, mgr->target);
        SDL_RenderClear(renderer);

        // If a scene is within a transition, we ignore their draws.
        SceneTransition *trans = scene_get_active_transition(mgr, scene);
        if (trans != NULL)
        {
            // Make sure these are rendered.
            // We can add interpolation functions here after.
            double progress = trans->info.duration == 0 ? 1 : trans->elapsed / trans->info.duration;
            progress = SDL_clamp(progress, 0, 1);
            progress = animation_curve_calc(trans->info.curve, progress);

            // Clear up transitioning targets also.
            SDL_SetRenderTarget(renderer, trans->texture);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, mgr->target);

            // Depends on the transition type, we have to call each different
            // transition handler.
            switch (trans->info.type)
            {
            case TRANSITION_NONE:
                scene_mgr_transition_render_none(mgr, trans, progress);
                break;
            case TRANSITION_FADE:
                scene_mgr_transition_render_fade(mgr, trans, progress);
                break;
            case TRANSITION_SLIDE_LEFT:
                scene_mgr_transition_render_slide_left(mgr, trans, progress);
                break;
            case TRANSITION_SLIDE_RIGHT:
                scene_mgr_transition_render_slide_right(mgr, trans, progress);
                break;
            case TRANSITION_SLIDE_UP:
                scene_mgr_transition_render_slide_up(mgr, trans, progress);
                break;
            case TRANSITION_SLIDE_DOWN:
                scene_mgr_transition_render_slide_down(mgr, trans, progress);
                break;
            case TRANSITION_SPLIT_HORIZONTAL:
                scene_mgr_transition_render_split_horiz(mgr, trans, progress);
                break;
            }
        }
        else if (scene->ondraw)
        {
            scene->ondraw(scene, renderer);
        }

        // Draw overlays on colliders
        if (appstate->settings.show_debug_colliders)
        {
            uint32_t *keys = NULL;
            const Collider **colliders = NULL;
            hash_map_iterate(scene->colliders, &keys, (const void ***)&colliders);

            if (keys && colliders)
            {
                for (uint32_t j = 0; j < scene->colliders->size; j++)
                {
                    collider_render(colliders[j], renderer);
                }
            }

            SDL_free(keys);
            SDL_free(colliders);
        }

        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderTexture(renderer, mgr->target, NULL, NULL);
    }
}

void scene_mgr_on_signal(SceneManager *mgr, Signal *signal)
{
    for (int i = (int)mgr->scenes->length - 1; i >= 0; i--)
    {
        Scene *scene = (Scene *)mgr->scenes->items[i];
        if (scene->accepting_signals && scene->onsignal)
        {
            scene->onsignal(scene, signal);
        }

        if (scene->stops_propagation)
            break;
    }
}

void scene_mgr_destroy(SceneManager mgr)
{
    if (mgr.scenes)
    {
        for (int i = 0; i < (int)mgr.scenes->length; i++)
        {
            scene_destroy((void *)mgr.scenes->items[i]);
        }
        list_destroy(mgr.scenes);
    }

    if (mgr.target)
    {
        SDL_DestroyTexture(mgr.target);
    }

    if (mgr.transitions)
    {
        for (int i = 0; i < (int)mgr.transitions->length; i++)
        {
            scene_transition_destroy((void *)mgr.transitions->items[i]);
        }
        list_destroy(mgr.transitions);
    }
}
