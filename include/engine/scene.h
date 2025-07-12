// engine/scene.h
//
// Represents various scenes available in the game.

#pragma once

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "engine/signal.h"
#include "misc/hashmap.h"
#include "misc/list.h"
#include "misc/stack.h"

/**
 * Represents the enumeration type IDs for various scene types.
 */
typedef enum
{
    // Special case scene. This scene holds only transparent pixels.
    SCENE_ID_EMPTY = 0,
    SCENE_ID_FPS,
    SCENE_ID_LOADING,
    SCENE_ID_MAIN_MENU,

    NUM_SCENE_IDS,
} SceneId;

/**
 * Represents internal data for an empty scene.
 */
typedef struct
{
    SDL_Color color;
    SDL_FRect frect;
} SceneEmpty;

/**
 * Represents internal data for an FPS scene.
 */
typedef struct
{
    SDL_Color color;
} SceneFPS;

/**
 * Represents a scene in the game.
 */
typedef struct Scene
{
    SceneId id; // The ID of the scene. Used for querying.

    // Lifecycle of a scene:
    //
    // When it is requested to be transitioned into or added into the scene
    // manager, `oninit` is called. After the transition is finished, `onstart`
    // is called, then the scene goes into a running state.
    //
    // oninit is called when the scene is loaded to the game.
    // For each frame it is loaded, ontick is called.
    // For each engine physical frame is run, onphystick is called.
    // When the scene is unloaded, ondestroy is called.
    void (*oninit)(struct Scene *scene);
    void (*onstart)(struct Scene *scene);
    void (*ontick)(struct Scene *scene, double dt);
    void (*ondraw)(struct Scene *scene, SDL_Renderer *renderer);
    void (*onphystick)(struct Scene *scene);
    void (*onsignal)(struct Scene *scene, Signal *signal);
    void (*ondestroy)(struct Scene *scene);

    // The scene's own colliders and sprites. These are handled by the callers,
    // scene destruction won't destroy those, as these can be reused by other
    // scenes.
    HashMap *colliders;
    HashMap *sprites;

    // Scene's flags.
    //
    // enabled
    // -- If false, tick, draw, phystick won't get called.
    // -- If true, it's a normal working scene.
    //
    // accepting_signals
    // -- If false, onsignal won't get called.
    // -- If true, onsignal is passed the signal caught.
    //
    // captures_focus
    // -- If false, this does nothing.
    // -- If true, scenes below this in the stack won't have tick and phystick
    // called. Esentially paused.
    //
    // stops_propagation
    // -- If false, this does nothing.
    // -- If true, all scenes below don't get signals passed to them.
    bool enabled;
    bool accepting_signals;
    bool captures_focus;
    bool stops_propagation;

    // The scene's private data
    union SceneData
    {
        SceneEmpty empty;
        SceneFPS fps;
    } data;
} Scene;

/**
 * Represents the type of a transition a scene can do.
 */
typedef enum
{
    TRANSITION_NONE,
    TRANSITION_FADE,
    TRANSITION_SLIDE_LEFT,
    TRANSITION_PUSH_DOWN,
    TRANSITION_PUSH_UP,
    TRANSITION_PUSH_LEFT,
    TRANSITION_PUSH_RIGHT,
} TransitionType;

/**
 * Represents data for the scene transition.
 */
typedef struct
{
    TransitionType type;
    double duration;
    double elapsed;

    Scene *from_scene;
    Scene *to_scene;

    bool active;
    bool destroys_after; // Whether to destroy from_scene after transitioning.
    bool stops_physics;  // Whether to stop physics during transitioning.
    bool stops_signals; // Whether to stop passing signals during transitioning.

    // Rendering primitives.
    SDL_Texture *from_txt;
    SDL_Texture *to_txt;
} SceneTransition;

/**
 * Represents the manager of scene.
 */
typedef struct
{
    Stack *scenes;
    List *transitions;
    SDL_Texture *target;
} SceneManager;

/**
 * Initializes a default, good behaving scene.
 */
Scene *scene_init(void);

/**
 * Destroys the scene. Makes sure this scene is not referenced elsewhere as this
 * will invalidate the pointer.
 */
void scene_destroy(Scene *scene);

/**
 * Ticks the scene manager at a variable rate.
 */
void scene_mgr_tick(SceneManager *mgr, double dt);

/**
 * Pushes a scene unconditionally to the stack. This returns false if the stack
 * is full. This is mainly used to setup starting scenes that do not need
 * transitions.
 */
bool scene_mgr_push_scene(SceneManager *mgr, Scene *scene);

/**
 * Starts a new transition from a scene to another.
 *
 * The caller should not allocate any transitions and let the scene manager
 * handle it.
 */
void scene_mgr_start_transition(SceneManager *mgr, SceneTransition transition);

/**
 * Ticks the scene manager physically. Only at a rate of 16ms per tick.
 */
void scene_mgr_phys_tick(SceneManager *mgr);

/**
 * Calls when the engine receives a signal and would like the scene manager to
 * handle it.
 */
void scene_mgr_on_signal(SceneManager *mgr, Signal *signal);

/**
 * Renders the current scene manager.
 */
void scene_mgr_draw(SceneManager *mgr);
