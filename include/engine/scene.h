// engine/scene.h
//
// Represents various scenes available in the game.

#pragma once

#include "SDL3/SDL_render.h"
#include "engine/signal.h"
#include "misc/hashmap.h"
#include "misc/list.h"

/**
 * Represents a scene in the game.
 */
typedef struct Scene
{
    int zindex;

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
    void *data;
} Scene;

/**
 * Represents the type of a transition a scene can do.
 */
typedef enum
{
    TRANSITION_NONE,
    TRANSITION_FADE,
    TRANSITION_SLIDE_LEFT,
    TRANSITION_SLIDE_RIGHT,
    TRANSITION_SLIDE_UP,
    TRANSITION_SLIDE_DOWN,
    TRANSITION_SPLIT_HORIZONTAL,
} TransitionType;

/**
 * The animation curve meant to animate the transition with.
 */
typedef enum
{
    ANIMATION_CURVE_LINEAR,
    ANIMATION_CURVE_EASE_IN,
    ANIMATION_CURVE_EASE_OUT,
    ANIMATION_CURVE_EASE_IN_OUT,
} AnimationCurve;

/**
 * Represents data for the scene transition.
 */
typedef struct
{
    TransitionType type;  // The transition animation type.
    AnimationCurve curve; // How to smooth out that animation.
    bool entry;      // Whether it's an entrance animation or an exit animation.
    double duration; // The duration of the animation. Must be greater than 0.
    Scene *scene;    // The scene responsible for the animation.
} SceneTransitionInfo;

/**
 * Opaque handle for a scene transition.
 */
typedef struct SceneTransition SceneTransition;

/**
 * Represents the manager of scene.
 */
typedef struct
{
    List *scenes;
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
 * Destroys a scene transition.
 */
void scene_transition_destroy(SceneTransition *trans);

/**
 * Ticks the scene manager at a variable rate.
 */
void scene_mgr_tick(SceneManager *mgr, double dt);

/**
 * Requests the scene manager to reorder its scenes based on the z-index. For
 * use when you change the z-index at runtime.
 */
void scene_mgr_reorder(SceneManager *mgr);

/**
 * Starts a new transition from a scene to another.
 *
 * The caller should not allocate any transitions and let the scene manager
 * handle it.
 */
void scene_mgr_start_transition(SceneManager *mgr, SceneTransitionInfo info);

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

/**
 * Destroys the inner of a scene manager.
 */
void scene_mgr_destroy(SceneManager mgr);
