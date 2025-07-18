// engine/engine.h
//
// The main engine component of the game. This should handle
// physics and state updates.

#pragma once

#include "app.h"
#include "SDL3/SDL_events.h"

/**
 * Initializes the engine of this application.
 */
bool engine_init(AppState *app);

/**
 * Pushes a signal to the engine to be handled on the next frame.
 */
void engine_push_signal(Signal *signal);

/**
 * Parses the event the application caught.
 */
void engine_handle_event(AppState *app, SDL_Event *event);

/**
 * Make the engine run one iteration of the application.
 */
void engine_iterate(AppState *app);

/**
 * Called when the app needs to render.
 */
void engine_render(AppState *app);

/**
 * Destroys the game engine and shuts down all components.
 */
void engine_destroy(void);
