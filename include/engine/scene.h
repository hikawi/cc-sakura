/**
 * \file engine/scene.h
 *
 * Provides a scene-based rendering system
 *
 * This provides us with a layer by layer, where each layer is called a scene composed in a scene manager. Each scene is
 * expected to handle its own logic, with its holding colliders and sprites. As of now, two scenes may not interact with
 * each other to promote coupling. This may be changed in the future.
 */

#pragma once

#include "engine/signal.h"
#include "misc/hashmap.h"
#include "misc/list.h"
#include "misc/quadtree.h"
#include "SDL3/SDL_render.h"

/**
 * \struct Scene
 *
 * Represents a simple layer on the screen
 *
 * Each scene should be managed independently of each other, but hacky ways may be attempted to pass scenes as data to
 * each other, but is not advised. Please don't touch the scene's quadtree, it is used to optimize looking for
 * collisions in a scene, and not meant for user use.
 *
 * Each scene has its own lifecycle, starting from when it is transitioned in up until it is fully transitioned out. For
 * each lifecycle function, refer to their own documentation.
 *
 * \see oninit, onstart, ontick, onphystick, ondraw, onsignal, ondestroy
 */
typedef struct Scene
{
    /**
     * The z-index of the scene.
     *
     * The higher it is, the later it is rendered in the rendering pipeline, the sooner it is to be invoked with a
     * signal caught from the screen. A scene with zindex = 0 will always be rendered below zindex = 1.
     */
    int zindex;

    /**
     * The scene's quadtree.
     *
     * An approximation of the scene's colliders in a quadtree format for fast approximating guesses of which pair of
     * colliders that may be colliding. Users should not be concerned with this.
     *
     * \warning Tampering with this may cause inaccuracies in engine detection
     */
    QuadtreeNode *quadtree;

    /**
     * Called when the scene is initialized and started to transition in.
     *
     * \param scene The current scene
     */
    void (*oninit)(struct Scene *scene);

    /**
     * Called when the scene is finished transitioning in.
     *
     * \param scene The current scene
     */
    void (*onstart)(struct Scene *scene);

    /**
     * Called every frame since the scene starts transitioning in.
     *
     * This may not get called if \ref enabled is false.
     *
     * \param scene The current scene
     * \param dt The time in seconds that has passed since last frame
     */
    void (*ontick)(struct Scene *scene, double dt);

    /**
     * Called every frame when the renderer needs to draw this scene.
     *
     * This may not get called if \ref enabled is false.
     *
     * \param scene The current scene
     * \param renderer The renderer
     */
    void (*ondraw)(struct Scene *scene, SDL_Renderer *renderer);

    /**
     * Called every physical frame when the engine wants to update physics.
     *
     * This may not get called if \ref enabled is false. How often this gets called depends on the application's physics
     * time step, usually 60FPS meaning one call every roughly 16ms.
     *
     * \param scene The current scene
     */
    void (*onphystick)(struct Scene *scene);

    /**
     * Called every time when a signal is caught.
     *
     * This may not get called if \ref accepting_signals is false, or a scene with a higher \ref zindex has blocked
     * propagation.
     *
     * \param scene The current scene
     * \param signal The signal pointer, is never null
     */
    void (*onsignal)(struct Scene *scene, Signal *signal);

    /**
     * Called when the scene transitions out of the screen.
     *
     * You are expected to clean up allocated data here, but the following data is cleared by the scene manager, so
     * avoid clearing them yourselves:
     * - Colliders map \ref colliders
     * - Sprites map \ref sprites
     * - User data \ref data (the main pointer, any internal data is not free-d)
     *
     * If your custom user data contains more various pointers, you are responsible for clearing them.
     *
     * \param scene The current scene
     */
    void (*ondestroy)(struct Scene *scene);

    /**
     * The scene's own colliders.
     */
    HashMap *colliders;

    /**
     * The scene's own sprites.
     */
    HashMap *sprites;

    /**
     * The scene's list of dirty colliders.
     *
     * This is meant to be used in a physics step when the scene moved some colliders around, leading to the result that
     * their approximation is no longer accurate. Adding them here to request the engine to redo the approximation step
     * for next collision checks.
     */
    List *moved_colliders;

    /**
     * Whether this scene gets action
     *
     * This property decides whether \ref ontick, \ref ondraw or \ref onphystick gets called. If this is false, all the
     * provided functions won't be invoked. The scene is essentially in a paused state.
     */
    bool enabled;

    /**
     * Whether this scenes gets signals
     *
     * This property decides whether \ref onsignal gets called. If this is false, no signals are passed.
     */
    bool accepting_signals;

    /**
     * Whether to capture window's focus
     *
     * This property on a scene stack will stop all scenes with a lower zindex to act as disabled scenes. Meaning
     * ontick, onphystick and ondraw will not be invoked on any scenes below this scene.
     */
    bool captures_focus;

    /**
     * Whether to stop signals propagation
     *
     * This property on a scene stack will stop all scenes with a lower zindex to act as not accepting signals. Signals
     * that hit this scene will be handled by this scene and stop.
     */
    bool stops_propagation;

    /**
     * The scene's custom user data
     *
     * If this is not `NULL` on scene_destroy, it will be freed by the scene manager.
     */
    void *data;
} Scene;

/**
 * \enum TransitionType
 *
 * Represents the type of a transition a scene can do. This has two modes, configurable by a \ref SceneTransitionInfo
 * struct (entry and exit), but they should be the same.
 */
typedef enum
{
    /**
     * Causes an instant transition.
     *
     * This works by instantly setting the progress of the animation to 100%, effectively ending the animation on the
     * next frame.
     */
    TRANSITION_NONE,

    /**
     * Fades the scene in/out.
     *
     * This works by scaling the opacity from 0% to 100% for an entry transition and 100% to 0% for an exit transition.
     */
    TRANSITION_FADE,

    /**
     * Slides the scene left.
     *
     * If it's an entry transition, slide the scene from the right to the middle of the screen. Otherwise, slide the
     * scene from the middle of the screen off the stage to the left.
     */
    TRANSITION_SLIDE_LEFT,

    /**
     * Slides the scene right.
     *
     * If it's an entry transition, slide the scene from the left to the middle of the screen. Otherwise, slide the
     * scene from the middle of the screen off the stage to the right.
     */
    TRANSITION_SLIDE_RIGHT,

    /**
     * Slides the scene up.
     *
     * If it's an entry transition, slide the scene from the bottom to the middle of the screen. Otherwise, slide the
     * scene from the middle of the screen off the stage up.
     */
    TRANSITION_SLIDE_UP,

    /**
     * Slides the scene down.
     *
     * If it's an entry transition, slide the scene from the bottom to the middle of the screen. Otherwise, slide the
     * scene from the middle of the screen off the stage up.
     */
    TRANSITION_SLIDE_DOWN,

    /**
     * Splits the scene horizontally in half.
     *
     * If it's an entry transition, split in half and move each half from the top and bottom screen to the middle.
     * Otherwise, split in half and move each half from the middle outside to top and bottom.
     */
    TRANSITION_SPLIT_HORIZONTAL,
} TransitionType;

/**
 * \enum AnimationCurve
 *
 * The curve that dictates what function to use to calculate the "easing" effect.
 */
typedef enum
{
    /**
     * Use a linear function.
     */
    ANIMATION_CURVE_LINEAR,

    /**
     * Use a sine easing-in function.
     */
    ANIMATION_CURVE_EASE_IN,

    /**
     * Use a sine easing-out function.
     */
    ANIMATION_CURVE_EASE_OUT,

    /**
     * Use a sine easing-in-out function.
     */
    ANIMATION_CURVE_EASE_IN_OUT,

    /**
     * Use a bouncy function for easing in.
     */
    ANIMATION_CURVE_EASE_IN_BOUNCE,

    /**
     * Use a bouncy function for easing out.
     */
    ANIMATION_CURVE_EASE_OUT_BOUNCE,

    /**
     * Use a bouncy function for both eases.
     */
    ANIMATION_CURVE_EASE_IN_OUT_BOUNCE,
} AnimationCurve;

/**
 * \struct SceneTransitionInfo
 *
 * Represents data for the scene transition.
 *
 * Through this, you can provide necessary information for the scene manager to start a correct transition.
 */
typedef struct
{
    /**
     * The transition type
     *
     * You may use TRANSITION_TYPE_NONE to have an instant transition, or have the \ref duration be zero.
     */
    TransitionType type;

    /**
     * The animation curve function
     */
    AnimationCurve curve;

    /**
     * Whether to animate the scene in or out
     *
     * Animations will differ based on whether it's an entry transition or an exit transition, refer to their own
     * transition type for more details.
     *
     * \see TransitionType
     */
    bool entry;

    /**
     * The duration of the animation.
     *
     * Must be greater or equal to 0. A 0-length animation essentially stops animation and instantly starts or destroys
     * the scene.
     *
     * \see TRANSITION_TYPE_NONE
     */
    double duration;

    /**
     * The scene related in the transition
     *
     * For an exit transition, if the scene does not exist in the scene manager, it will log a warning and ignore your
     * transition request.
     */
    Scene *scene;
} SceneTransitionInfo;

/**
 * The manager of scenes...
 *
 * The manager is responsible for handling scene transitions, scene's lifecycles and piping signals caught, or generated
 * by the engine to the scene stack in a logical order.
 *
 * \warning Internal members are exposed for engine and app management. Unrelated code should not tamper directly.
 */
typedef struct
{
    List *scenes;
    List *transitions;
    SDL_Texture *target;
} SceneManager;

/**
 * Initializes a default, good behaving scene.
 *
 * \return a newly initialized and zero'd scene
 */
Scene *scene_init(void);

/**
 * Destroys the scene. Makes sure this scene is not referenced elsewhere as this
 * will invalidate the pointer.
 *
 * \param scene the scene to destroy
 * \warning The provided pointer will be invalidated
 */
void scene_destroy(Scene *scene);

/**
 * Ticks the scene manager at a variable rate.
 *
 * This function will be invoked by the engine on every possible frame of the program.
 *
 * \param mgr the scene manager
 * \param dt deltatime since last frame
 */
void scene_mgr_tick(SceneManager *mgr, double dt);

/**
 * Requests the scene manager to reorder its scenes based on the z-index.
 *
 * This can be used when you want to change the scene's zindex at runtime and would like the scene manager to re-sort
 * the rendering order before the next rendering step.
 *
 * \param mgr the scene manager
 */
void scene_mgr_reorder(SceneManager *mgr);

/**
 * Starts a new transition from a scene to another.
 *
 * The caller should not allocate any transitions and let the scene manager
 * handle it
 *
 * \warning Invalid parameters passed in via `info` is undefined behavior.
 *
 * \param mgr the scene manager
 * \param info the information for a scene transition
 */
void scene_mgr_start_transition(SceneManager *mgr, SceneTransitionInfo info);

/**
 * Ticks the scene manager physically.
 *
 * This would tick all relevant scenes on a physical cycle. For a 60FPS game, this would be around 16ms per cycle.
 * Scenes are expected to update their physics-related data.
 *
 * If a collider moves, the scene may put the collider inside \ref Scene::moved_colliders to mark them as dirty for the
 * engine to handle collisions properly later.
 *
 * \param mgr the scene manager
 */
void scene_mgr_phys_tick(SceneManager *mgr);

/**
 * Called when the engine receives a signal and would like the scene manager to
 * handle it.
 *
 * This would propagate down all scenes until a \ref Scene::stops_propagation flag was hit.
 *
 * \param mgr the scene manager
 * \param signal the signal to pass through
 */
void scene_mgr_on_signal(SceneManager *mgr, Signal *signal);

/**
 * Renders the current scene manager.
 *
 * \param mgr the scene manager
 */
void scene_mgr_draw(SceneManager *mgr);

/**
 * Destroys the inner of a scene manager.
 *
 * \param mgr the scene manager
 */
void scene_mgr_destroy(SceneManager mgr);
