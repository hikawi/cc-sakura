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

/**
 * Represents a struct that holds the main information about the game.
 */
typedef struct
{
  Uint64 last_frame_tick;   // When the last frame was rendered.
  double frame_accumulator; // The amount of time accumulated before the next fixed update.

  Uint32 frames_counter; // Counter for frames to calculate FPS.
  double frames_elapsed; // How much time in seconds have elapsed.
  Uint32 frames_per_sec; // The final value for FPS to display.
} AppState;
