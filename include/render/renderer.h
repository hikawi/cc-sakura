// render/renderer.h
//
// The main component for rendering and handle SDL's windows
// and renderers.

#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "app.h"

/**
 * Initializes SDL's window and renderer.
 */
bool init_window_and_renderer(void);

/**
 * Retrieves the app's current window. This is never NULL.
 */
SDL_Window *get_current_window(void);

/**
 * Retrieves the app's current renderer. This is never NULL.
 */
SDL_Renderer *get_current_renderer(void);

/**
 * Starts the rendering process.
 */
void render(AppState *state);

/**
 * Destroys the allocated window and renderer.
 */
void destroy_window_and_renderer(void);
