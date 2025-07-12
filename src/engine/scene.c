#include "engine/scene.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "app.h"
#include "misc/hashmap.h"
#include "misc/list.h"
#include "misc/stack.h"
#include <string.h>

#define SCENE_BLOCK_PHYSTICS_TICKS 1
#define SCENE_BLOCK_TICKS 2
#define SCENE_BLOCK_SIGNALS 4

Scene *scene_init(void)
{
    Scene *scene = SDL_calloc(1, sizeof(Scene));
    scene->id = SCENE_ID_EMPTY;
    scene->colliders = hash_map_init();
    scene->sprites = hash_map_init();
    return scene;
}

void scene_destroy(Scene *scene)
{
    if (!scene)
        return;

    if (scene->ondestroy)
        scene->ondestroy(scene);

    hash_map_destroy(scene->colliders);
    hash_map_destroy(scene->sprites);
    SDL_free(scene);
}

/**
 * Purges all inactive transitions.
 */
void scene_mgr_purge_transitions(SceneManager *mgr)
{
    Uint32 i = 0;
    while (i < mgr->transitions->length)
    {
        SceneTransition *trans = (SceneTransition *)mgr->transitions->items[i];
        if (!trans->active)
        {
            list_remove_at(mgr->transitions, i);
            SDL_DestroyTexture(trans->from_txt);
            SDL_DestroyTexture(trans->to_txt);
            SDL_free(trans);
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
    SDL_Texture *target = SDL_GetRenderTarget(win.renderer);

    // Render the from scene first.
    SDL_SetRenderTarget(win.renderer, trans->from_txt);
    if (trans->from_scene->ondraw)
    {
        trans->from_scene->ondraw(trans->from_scene, win.renderer);
    }

    // Render the to scene then.
    SDL_SetRenderTarget(win.renderer, trans->to_txt);
    if (trans->to_scene->ondraw)
    {
        trans->to_scene->ondraw(trans->to_scene, win.renderer);
    }

    SDL_SetRenderTarget(win.renderer, target);
}

/**
 * Renders the none transition.
 */
void scene_mgr_transition_render_none(SceneManager *mgr, SceneTransition *trans,
                                      double progress)
{
    (void)mgr;
    (void)progress;

    // Instantly end the transition.
    trans->elapsed = trans->duration;
}

/**
 * Renders the fading transition.
 */
void scene_mgr_transition_render_fade(SceneManager *mgr, SceneTransition *trans,
                                      double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Then we start the fade out based on the progress.
    // progress = 0 means from is fully visible
    // progress = 1 means to is fully visible
    SDL_SetTextureAlphaModFloat(trans->from_txt, 1 - (float)progress);
    SDL_SetTextureAlphaModFloat(trans->to_txt, (float)progress);

    // Get the dstrect for full screen.
    SDL_FRect dstrect = {
        .x = 0,
        .y = 0,
        .h = win.h,
        .w = win.w,
    };

    SDL_RenderTexture(win.renderer, trans->to_txt, NULL, &dstrect);
    SDL_RenderTexture(win.renderer, trans->from_txt, NULL, &dstrect);

    // Clean up after yourselves.
    SDL_SetTextureAlphaModFloat(trans->from_txt, 1);
    SDL_SetTextureAlphaModFloat(trans->to_txt, 1);
}

/**
 * Animates the sliding left transition. This will makes it so the first scene
 * moves left to reveal the second scene underneath.
 */
void scene_mgr_transition_render_slide_left(SceneManager *mgr,
                                            SceneTransition *trans,
                                            double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // We render the to_scene first, to make it appear underneath.
    SDL_FRect dstrect = {
        .x = 0,
        .y = 0,
        .h = win.h,
        .w = win.w,
    };
    SDL_RenderTexture(win.renderer, trans->to_txt, NULL, &dstrect);

    // Then render the from_scene above, but offset by a slight amount.
    dstrect.x -= (float)(progress * win.w);
    SDL_RenderTexture(win.renderer, trans->from_txt, NULL, &dstrect);
}

/**
 * Animates the pushing up transition. The after scene would push the before
 * scene upwards.
 */
void scene_mgr_transition_render_push_up(SceneManager *mgr,
                                         SceneTransition *trans,
                                         double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Calculate the offset (up) needed to render from_scene.
    SDL_FRect dstrect = {
        .x = 0,
        .y = 0,
        .h = win.h,
        .w = win.w,
    };
    dstrect.y -= (float)(win.h * progress);
    SDL_RenderTexture(win.renderer, trans->from_txt, NULL, &dstrect);

    // Render the to_scene.
    dstrect.y += dstrect.h;
    SDL_RenderTexture(win.renderer, trans->to_txt, NULL, &dstrect);
}

/**
 * Animates the pushing down transition. The after scene would push the before
 * scene downwards.
 */
void scene_mgr_transition_render_push_down(SceneManager *mgr,
                                           SceneTransition *trans,
                                           double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Calculate the offset needed to render from_scene.
    SDL_FRect dstrect = {
        .x = 0,
        .y = 0,
        .h = win.h,
        .w = win.w,
    };
    dstrect.y += (float)(win.h * progress);
    SDL_RenderTexture(win.renderer, trans->from_txt, NULL, &dstrect);

    // Render the to_scene.
    dstrect.y -= dstrect.h;
    SDL_RenderTexture(win.renderer, trans->to_txt, NULL, &dstrect);
}

/**
 * Animates the pushing left transition. The after scene would push the before
 * scene left.
 */
void scene_mgr_transition_render_push_left(SceneManager *mgr,
                                           SceneTransition *trans,
                                           double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Calculate the offset needed to render from_scene.
    SDL_FRect dstrect = {
        .x = 0,
        .y = 0,
        .h = win.h,
        .w = win.w,
    };
    dstrect.x -= (float)(win.w * progress);
    SDL_RenderTexture(win.renderer, trans->from_txt, NULL, &dstrect);

    // Render the to_scene.
    dstrect.x += dstrect.w;
    SDL_RenderTexture(win.renderer, trans->to_txt, NULL, &dstrect);
}

/**
 * Animates the pushing right transition. The after scene would push the before
 * scene right.
 */
void scene_mgr_transition_render_push_right(SceneManager *mgr,
                                            SceneTransition *trans,
                                            double progress)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;
    scene_mgr_transition_render(mgr, trans);

    // Calculate the offset needed to render from_scene.
    SDL_FRect dstrect = {
        .x = 0,
        .y = 0,
        .h = win.h,
        .w = win.w,
    };
    dstrect.x += (float)(win.w * progress);
    SDL_RenderTexture(win.renderer, trans->from_txt, NULL, &dstrect);

    // Render the to_scene.
    dstrect.x -= dstrect.w;
    SDL_RenderTexture(win.renderer, trans->to_txt, NULL, &dstrect);
}

/**
 * Applies a transition effect on the renderer.
 */
void scene_mgr_handle_transition(SceneManager *mgr, SceneTransition *trans,
                                 double dt)
{

    // If the transition is inactive, then do nothing.
    if (!trans->active)
    {
        return;
    }

    // Here the transition is still in play.

    // Handle the transition ending.
    if (trans->elapsed > trans->duration)
    {
        trans->active = false; // Mark for deletion.
        SDL_assert(trans->from_scene != NULL && trans->to_scene != NULL);

        // Do the actual scene swapping.
        // Since the scene being swapped may not be on top of the stack, we want
        // to swap in place.
        for (int i = 0; i < mgr->scenes->length; i++)
        {
            if (mgr->scenes->items[i] == trans->from_scene)
            {
                if (trans->from_scene->ondestroy)
                    trans->from_scene->ondestroy(trans->from_scene);

                mgr->scenes->items[i] = trans->to_scene;

                // Start the scene, the transition is finished.
                if (trans->to_scene->onstart)
                    trans->to_scene->onstart(trans->to_scene);

                // Whether to free the "from_scene" after.
                if (trans->destroys_after)
                {
                    scene_destroy(trans->from_scene);
                }

                break;
            }
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
    for (int i = mgr->scenes->length - 1; i >= 0; i--)
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
    for (int i = mgr->scenes->length - 1; i >= 0; i--)
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

void scene_mgr_start_transition(SceneManager *mgr, SceneTransition transition)
{
    AppState *appstate = app_get();
    WindowStatus win = appstate->window;

    bool found = false;
    for (int i = 0; i < mgr->scenes->length; i++)
    {
        if (mgr->scenes->items[i] == transition.from_scene)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Scene Transition called with a scene that does not exist yet.");
        return;
    }

    SceneTransition *trans = SDL_malloc(sizeof(SceneTransition));
    *trans = transition;

    trans->from_txt = SDL_CreateTexture(win.renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_TARGET, win.w, win.h);
    trans->to_txt = SDL_CreateTexture(win.renderer, SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_TARGET, win.w, win.h);

    if (!trans->from_txt || !trans->to_txt)
    {
        SDL_Log("Failed to create transition textures: %s", SDL_GetError());
        if (trans->from_txt)
            SDL_DestroyTexture(trans->from_txt);
        if (trans->to_txt)
            SDL_DestroyTexture(trans->to_txt);

        SDL_free(trans);
        return;
    }

    SDL_SetTextureBlendMode(trans->from_txt, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(trans->to_txt, SDL_BLENDMODE_BLEND);

    // Initialize the to transition.
    if (transition.to_scene->oninit)
    {
        transition.to_scene->oninit(transition.to_scene);
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
        if (trans->active && trans->from_scene == scene)
            return trans;
    }

    return NULL;
}

void scene_mgr_draw(SceneManager *mgr)
{
    AppState *appstate = app_get();
    SDL_Renderer *renderer = appstate->window.renderer;

    // We're gonna go through each scene in the current active stack and render
    // them.
    for (int i = 0; i < mgr->scenes->length; i++)
    {
        Scene *scene = mgr->scenes->items[i];

        // Reset the renderer before starting
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_SetRenderTarget(renderer, mgr->target);
        SDL_RenderClear(renderer);

        // If a scene is not enabled, we don't render them anyway.
        if (!scene || !scene->enabled)
            continue;

        // If a scene is within a transition, we ignore their draws.
        SceneTransition *trans = scene_get_active_transition(mgr, scene);
        if (trans != NULL)
        {
            // Make sure these are rendered.
            // We can add interpolation functions here after.
            double progress = SDL_clamp(
                trans->duration == 0 ? 1 : trans->elapsed / trans->duration, 0,
                1);

            // Clear up transitioning targets also.
            SDL_SetRenderTarget(renderer, trans->from_txt);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, trans->to_txt);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, mgr->target);

            // Depends on the transition type, we have to call each different
            // transition handler.
            switch (trans->type)
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
            case TRANSITION_PUSH_UP:
                scene_mgr_transition_render_push_up(mgr, trans, progress);
                break;
            case TRANSITION_PUSH_DOWN:
                scene_mgr_transition_render_push_down(mgr, trans, progress);
                break;
            case TRANSITION_PUSH_LEFT:
                scene_mgr_transition_render_push_left(mgr, trans, progress);
                break;
            case TRANSITION_PUSH_RIGHT:
                scene_mgr_transition_render_push_right(mgr, trans, progress);
                break;
            }

            // Apply and clear for next frame.
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderTexture(renderer, mgr->target, NULL, NULL);

            continue;
        }

        // Otherwise, we let it draw normally.
        if (scene->ondraw)
        {
            scene->ondraw(scene, renderer);

            // Draw on the scene and reset.
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderTexture(renderer, mgr->target, NULL, NULL);
        }
    }
}

bool scene_mgr_push_scene(SceneManager *mgr, Scene *scene)
{
    if (stack_push(mgr->scenes, scene))
    {
        if (scene->oninit)
            scene->oninit(scene);
        if (scene->onstart)
            scene->onstart(scene);
        return true;
    }

    return false;
}

void scene_mgr_on_signal(SceneManager *mgr, Signal *signal)
{
    for (int i = 0; i < mgr->scenes->length; i++)
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
