// app.h
//
// The header file that contains defines and macros for the app-wide
// configuration.

#pragma once

#define APPLICATION_NAME "Sakura and the Clow Cards"
#define APPLICATION_VERSION "0.1.0"
#define APPLICATION_IDENTIFIER "dev.frilly.ccsakuraclowcards"

#define APPLICATION_ORIGINAL_WIDTH 1024
#define APPLICATION_ORIGINAL_HEIGHT 720

#define APPLICATION_MAX_FPS 60
#define APPLICATION_SCALE 2
#define APPLICATION_SHOW_FPS 1

#include "SDL3/SDL_stdinc.h"
#include "engine/collision.h"

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
 * Represents a struct that holds the main information about the game.
 */
typedef struct
{
    Uint64 last_frame_tick;   // When the last frame was rendered.
    double frame_accumulator; // The amount of time accumulated before the next
                              // fixed update.

    Uint32 frames_counter; // Counter for frames to calculate FPS.
    double frames_elapsed; // How much time in seconds have elapsed.
    Uint32 frames_per_sec; // The final value for FPS to display.

    KeyboardStatus keyboard; // The keyboard struct for querying keys.

    ColliderList *floor_colliders;
    QuadtreeNode *quadtree;
} AppState;

/**
 * Computes the path needed to retrieve a certain resource. Mostly done as MacOS
 * app bundles have a weird ass layout.
 *
 * The pointer returned is handled by the user, and should be freed.
 */
char *get_resource_path(const char *subpath);
