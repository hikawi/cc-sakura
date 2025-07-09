// engine/engine.h
//
// The main engine component of the game. This should handle
// physics and state updates.

#pragma once

#include "SDL3/SDL_events.h"
#include "app.h"

/**
 * Initializes the engine of this application.
 */
bool init_engine(AppState *app);

/**
 * Parses the event the application caught.
 */
void engine_handle_event(AppState *app, SDL_Event *event);

/**
 * Make the engine run one iteration of the application.
 */
void engine_iterate(AppState *app);

/**
 * Destroys the game engine and shuts down all components.
 */
void destroy_engine(void);
