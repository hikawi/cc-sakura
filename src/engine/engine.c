#include "engine/engine.h"

#include "app.h"
#include "common.h"
#include "engine/collision.h"
#include "engine/scene.h"
#include "engine/signal.h"
#include "engine/text.h"
#include "misc/hashmap.h"
#include "misc/hashset.h"
#include "misc/list.h"
#include "misc/quadtree.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static List *signals_queue      = NULL;
static SDL_Mutex *signals_mutex = NULL;

/**
 * Attempts to recurse a quadtree through each node and finds all potential
 * collisions in a broad-phase scan.
 */
void engine_broad_phase_collisions(const QuadtreeNode *root, List *ancestors, HashSet *potentials)
{
    if (!root)
    {
        return;
    }

    for (int i = 0; i < (int)root->colliders->length; i++)
    {
        const Collider this = collision_convert_to_aabb(root->colliders->items[i]);

        // Intra-node collisions
        for (int j = i + 1; j < (int)root->colliders->length; j++)
        {
            if (collision_partial_check(&this, root->colliders->items[j]).is_colliding)
            {
                CollisionPair *potential = SDL_malloc(sizeof(CollisionPair));
                potential->a             = root->colliders->items[i];
                potential->b             = root->colliders->items[j];
                hash_set_add(potentials, potential);
            }
        }

        // Ancestor collisions.
        for (int j = 0; j < (int)ancestors->length; j++)
        {
            if (collision_partial_check(&this, ancestors->items[j]).is_colliding)
            {
                CollisionPair *potential = SDL_malloc(sizeof(CollisionPair));
                potential->a             = root->colliders->items[i];
                potential->b             = ancestors->items[j];
                hash_set_add(potentials, potential);
            }
        }
    }

    // Recurse
    List *list = list_init();
    list_join(list, ancestors);       // Copy over
    list_join(list, root->colliders); // Copy current
    for (int i = 0; i < 4; i++)
    {
        if (root->children[i])
        {
            engine_broad_phase_collisions(root->children[i], list, potentials);
        }
    }

    list_destroy(list);
}

void engine_handle_collisions(AppState *app)
{
    Signal signal;
    for (int i = 0; i < (int)app->scene_mgr.scenes->length; i++)
    {
        Scene *s = (Scene *)app->scene_mgr.scenes->items[i];

        // If the scene does not have onsignal or not accepting signal, why
        // would we try?
        if (!s || !s->onsignal || !s->accepting_signals)
        {
            continue;
        }

        // Get all colliders, only if we haven't created the quadtree.
        if (!s->quadtree)
        {
            Uint32 size = s->colliders->size;
            Collider **colliders;
            Uint32 *keys;
            hash_map_iterate(s->colliders, &keys, (const void ***)&colliders);

            s->quadtree = quadtree_init();
            for (int j = 0; j < (int)size; j++)
            {
                quadtree_add(s->quadtree, colliders[j]);
            }

            SDL_free(colliders);
            SDL_free(keys);
        }

        // Add mouse's collider
        quadtree_add(s->quadtree, &app->input.mouse);

        // Handle dirty colliders.
        for (int j = 0; j < (int)s->moved_colliders->length; j++)
        {
            quadtree_remove(s->quadtree, s->moved_colliders->items[j]);
            quadtree_add(s->quadtree, s->moved_colliders->items[j]);
        }
        list_clear(s->moved_colliders);

        // Now for each collider possible, combined with the quadtree, we would
        // like to check collisions for "probably colliding" pairs.
        // We do this by recursing through the quadtree as needed.
        HashSet *potentials = hash_set_init();
        potentials->compare = (CompareFunction)collision_pair_comp;
        potentials->hash    = (HashFunction)collision_pair_hash;
        List *ancestors     = list_init();
        engine_broad_phase_collisions(s->quadtree, ancestors, potentials);
        list_destroy(ancestors);

        // Second step, narrow-phase.
        const void **potentials_list = NULL;
        uint32_t potentials_length;
        hash_set_iterate(potentials, &potentials_length, &potentials_list);
        hash_set_destroy(potentials);

        for (uint32_t j = 0; j < potentials_length; j++)
        {
            CollisionPair *pair = (void *)potentials_list[j];
            Collision info      = collision_check(pair->a, pair->b);
            if (!info.is_colliding)
            {
                continue;
            }

            signal.timestamp      = SDL_GetTicks();
            signal.type           = SIGNAL_COLLISION;
            signal.collision.pair = *pair;
            signal.collision.info = info;

            s->onsignal(s, &signal);
        }

        // Remove mouse's collider
        quadtree_remove(s->quadtree, &app->input.mouse);

        // Free all the potentials, it's safer to do it here idk.
        for (uint32_t j = 0; j < potentials_length; j++)
        {
            SDL_free((void *)potentials_list[j]);
        }
        SDL_free(potentials_list);
    }
}

/**
 * Tells the engine to handle all the signals or delegate it down to the scenes.
 */
void engine_pump_signals(AppState *app)
{
    (void)app;

    SDL_LockMutex(signals_mutex);

    // Fake handling pumped signals
    list_clear(signals_queue);

    SDL_UnlockMutex(signals_mutex);
}

void engine_iterate(AppState *app)
{
    // Calculate delta time
    Uint64 cur_frame                = SDL_GetTicks();
    double dt                       = (cur_frame - app->frame_data.last_frame_tick) / 1000.0;
    app->frame_data.last_frame_tick = cur_frame;

    if (dt > 0.1)
        dt = 0.1; // Clamp dt to only be 0.1s desync max.

    // Add dt to frame accumulator.
    app->frame_data.frame_accum += dt;

    engine_pump_signals(app);

    // Call fixed update if and only if frame_accum has passed
    // enough for 1 frame time (1 / FPS), for 60FPS this is about 16ms.
    while (app->frame_data.frame_accum >= (1.0 / APPLICATION_MAX_FPS))
    {
        app->frame_data.frame_accum -= (1.0 / APPLICATION_MAX_FPS);
        scene_mgr_phys_tick(&app->scene_mgr);
        engine_handle_collisions(app);
    }

    // Tick every frame.
    scene_mgr_tick(&app->scene_mgr, dt);

    // Before rendering, we update the FPS counter.
    app->frame_data.frame_time += dt;
    app->frame_data.frame_count++;
    if (app->frame_data.frame_time >= 1)
    {
        app->frame_data.fps         = app->frame_data.frame_count;
        app->frame_data.frame_count = 0;
        app->frame_data.frame_time -= 1;
    }
}

void engine_handle_event(AppState *app, SDL_Event *event)
{
    // Handle the event itself.
    switch (event->type)
    {
    case SDL_EVENT_MOUSE_MOTION:
        app->input.mouse.aabb.x = (double)(event->motion.x + event->motion.xrel);
        app->input.mouse.aabb.y = (double)(event->motion.y + event->motion.yrel);
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetRenderOutputSize(app->window.renderer, &app->window.w, &app->window.h);
        SDL_DestroyTexture(app->scene_mgr.target);
        app->scene_mgr.target = SDL_CreateTexture(app->window.renderer, SDL_PIXELFORMAT_RGBA8888,
                                                  SDL_TEXTUREACCESS_TARGET, app->window.w, app->window.h);

        // Reapply render target.
        if (SDL_GetRenderTarget(app->window.renderer) != NULL)
            SDL_SetRenderTarget(app->window.renderer, app->scene_mgr.target);
        else
            SDL_SetRenderTarget(app->window.renderer, NULL);
        SDL_SetTextureBlendMode(app->scene_mgr.target, SDL_BLENDMODE_BLEND);

        SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Resized rendering target.");
        break;
    case SDL_EVENT_KEY_DOWN:
        app->input.keyboard[event->key.scancode] = true;
        break;
    case SDL_EVENT_KEY_UP:
        app->input.keyboard[event->key.scancode] = false;
        break;
    }
}

void engine_push_signal(Signal *signal)
{
    SDL_LockMutex(signals_mutex);
    list_add(signals_queue, signal);
    SDL_UnlockMutex(signals_mutex);
}

void engine_render(AppState *app)
{
    // Clear the renderer.
    SDL_SetRenderTarget(app->window.renderer, NULL);
    SDL_SetRenderDrawColor(app->window.renderer, 255, 255, 255, 0);
    SDL_RenderClear(app->window.renderer);

    // Render the scenes I guess
    SDL_SetRenderTarget(app->window.renderer, app->scene_mgr.target);
    scene_mgr_draw(&app->scene_mgr);

    // Present.
    SDL_SetRenderTarget(app->window.renderer, NULL);
    SDL_RenderPresent(app->window.renderer);
}

bool engine_init(AppState *app)
{
    bool success = true;

    signals_queue = list_init();
    signals_mutex = SDL_CreateMutex();

    if (!font_engine_init(app))
    {
        success = false;
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to start font engine");
    }

    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Some errors happened while initializing engine");
    }

    return success;
}

void engine_destroy(void)
{
    font_engine_destroy();
}
