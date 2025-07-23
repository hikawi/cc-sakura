/**
 * \file engine/engine.h
 *
 * As the name implies, the main engine coordinator of the system. Everything else should be delegated from here.
 */

#pragma once

#include "app.h"
#include "SDL3/SDL_events.h"

/**
 * Initializes the engine of this application.
 *
 * \param app the application
 * \returns true if the engine was initialized correctly
 */
bool engine_init(AppState *app);

/**
 * Pushes a signal to the engine to be handled on the next frame.
 *
 * The engine will pump and handle signals the first thing in each frame. So any signals pushed from this frame will not
 * be handled on the same frame.
 *
 * \param signal the signal to push
 */
void engine_push_signal(Signal *signal);

/**
 * Parses the event the application caught.
 *
 * \param app the application
 * \param event the SDl event to parse
 */
void engine_handle_event(AppState *app, SDL_Event *event);

/**
 * Make the engine run one iteration of the application.
 *
 * \param app the application
 */
void engine_iterate(AppState *app);

/**
 * Called when the app needs to render on the window.
 *
 * \param app the application
 */
void engine_render(AppState *app);

/**
 * Destroys the game engine and shuts down all components.
 */
void engine_destroy(void);
