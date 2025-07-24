/**
 * \file app.h
 *
 * Header file that defines app-wide configurations.
 */

#pragma once

#include "engine/collision.h"
#include "engine/scene.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"

#define APPLICATION_NAME            "Sakura and the Clow Cards"
#define APPLICATION_IDENTIFIER      "dev.luny.ccsakura"
#define APPLICATION_ORGANIZATION    "Luny"
#define APPLICATION_APP_NAME        "Cardcaptor Sakura"

#define APPLICATION_ORIGINAL_WIDTH  1600
#define APPLICATION_ORIGINAL_HEIGHT 900

#define APPLICATION_MAX_FPS         60
#define APPLICATION_SCALE           2

#define APPLICATION_MAP_TILE        16

/**
 * A special struct for holding a few necessary values for computing when to
 * dispatch a physical update tick.
 */
typedef struct
{
    /**
     * The timestamp when the last frame was rendered.
     */
    uint64_t last_frame_tick;

    /**
     * The accumulated time before the next fixed update is dispatched.
     */
    double frame_accum;

    /**
     * The counter for frames per second.
     */
    uint32_t frame_count;

    /**
     * How much time has passed for calculating FPS, in seconds.
     *
     * Each second passed, \ref frame_count will be reset.
     */
    double frame_time;

    /**
     * The final counter that the FPS display should use.
     *
     * This value is updated every second by \ref frame_count.
     */
    uint32_t fps;
} FrameData;

/**
 * Represents the app's current mouse and keyboard input status.
 */
typedef struct
{
    /**
     * Keyboard input status in the form of SDL scancodes.
     */
    bool keyboard[SDL_SCANCODE_COUNT];

    /**
     * The mouse's collider.
     *
     * This will be updated every frame or every time the mouse moves to reflect the mouse's position.
     */
    Collider mouse;
} InputStatus;

/**
 * Represents SDL's current window and renderer status.
 */
typedef struct
{
    /**
     * SDL's renderer, should be created in the app state creation pipeline.
     *
     * \warning Do not touch with user code
     */
    SDL_Renderer *renderer;

    /**
     * SDL's window, should be created in the app creation pipeline.
     *
     * \warning Do not touch with user code
     */
    SDL_Window *window;

    /**
     * The window's width.
     */
    int w;

    /**
     * The window's height.
     */
    int h;
} WindowStatus;

/**
 * The logger module's data.
 */
typedef struct
{
    /**
     * The IOStream bound to a file, should be initialized with \ref logger_init.
     */
    SDL_IOStream *logio;
} Logger;

/**
 * Represents a struct that holds the main information about the game.
 */
typedef struct
{
    /**
     * App's frame data for calculating FPS and tick dispatcher.
     */
    FrameData frame_data;

    /**
     * Input status data holder.
     */
    InputStatus input;

    /**
     * SDL's window data.
     */
    WindowStatus window;

    /**
     * The app's main scene manager.
     */
    SceneManager scene_mgr;

    /**
     * The app's logger module.
     */
    Logger logger;

    /**
     * Whether the app should run on next tick, or tear down gracefully.
     */
    bool running;
} AppState;

/**
 * Initializes the first app state.
 *
 * The caller is responsible for handling the pointer.
 *
 * \returns an app state pointer
 */
AppState *app_init(void);

/**
 * Retrieves the app state, cached globally, as this should be used by a lot.
 *
 * \warning This should only be used by main thread.
 * \returns the cached global app state
 */
AppState *app_get(void);

/**
 * Panics and logs a last error message to the application before it quits on
 * the next frame.
 *
 * \param errmsg the message to log
 */
void app_panic(const char *errmsg);

/**
 * Destroys the app state.
 *
 * This also frees up the AppState pointer itself. Accessing the state after destroying is an undefined behavior.
 *
 * \param app the app state to destroy
 */
void app_destroy(AppState *app);
