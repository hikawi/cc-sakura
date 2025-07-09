// app.h
//
// The header file that contains defines and macros for the app-wide
// configuration.

#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#define APPLICATION_NAME "Sakura and the Clow Cards"
#define APPLICATION_VERSION "0.1.0"
#define APPLICATION_IDENTIFIER "dev.frilly.ccsakuraclowcards"

#define APPLICATION_ORIGINAL_WIDTH 1600
#define APPLICATION_ORIGINAL_HEIGHT 900

#define APPLICATION_MAX_FPS 60
#define APPLICATION_SCALE 2
#define APPLICATION_SHOW_FPS 1

#include "SDL3/SDL_stdinc.h"

/**
 * Represents the keyboard's key state.
 */
typedef struct
{
    bool key_a;
    bool key_d;
    bool key_s;
    bool key_w;
    bool key_space;
} KeyboardStatus;

/**
 * A special struct for holding a few necessary values for computing when to
 * dispatch a physical update tick.
 */
typedef struct
{
    Uint64 last_frame_tick; // When the last frame was rendered.
    double frame_accum; // The amount of time accumulated before the next fixed
                        // update.
    Uint32 frame_count; // The counter for frames to display an uncapped FPS
    double frame_time;  // How much time has passed, for calculating FPS. Each
                        // second passed (1.0) is a reset to `frame_count`.
    Uint32 fps; // The final counter for displaying the FPS. This value is used,
                // as `frame_count` is unreliable.
} FrameData;

/**
 * Represents a struct that holds the main information about the game.
 */
typedef struct
{
    FrameData frame_data; // Frames data for calculating FPS and update ticks.
    KeyboardStatus keyboard; // The keyboard struct for querying keys.

    SDL_Renderer *renderer; // The renderer
    SDL_Window *window;     // The window
    int w, h;               // The window's height and width.
} AppState;

/**
 * Initializes the first app state. The caller is responsible for handling
 * how to handle the pointer.
 */
AppState *init_app_state(void);

/**
 * Retrieves the app state, cached globally.
 */
AppState *get_app_state(void);

/**
 * Destroys the app state. This also frees up the AppState pointer itself.
 * Accessing the state after destroying is an undefined behavior.
 */
void destroy_app_state(AppState *app);

/**
 * Computes the path needed to retrieve a certain resource. Mostly done as MacOS
 * app bundles have a weird ass layout.
 *
 * The pointer returned is handled by the user, and should be freed.
 */
char *get_resource_path(const char *subpath);
