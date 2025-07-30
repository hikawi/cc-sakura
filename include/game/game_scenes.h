/**
 * \file game/game_scenes.h
 *
 * Composite header for initializing all of the scenes available in the game.
 */

#pragma once

#include "engine/scene.h"
#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_pixels.h"

/**
 * Sets up the starting scene.
 */
void scene_setup(void);

/**
 * Sets up a simple empty scene that paints the texture with the provided color.
 *
 * \param color the color to paint with
 * \returns an empty scene
 */
Scene *scene_empty(const SDL_Color color);

/**
 * Sets up a simple FPS scene that paints a FPS counter on the top right of the
 * screen of the provided color.
 *
 * \param color the color of the text
 * \returns an fps scene
 */
Scene *scene_fps(const SDL_Color color);

/**
 * Sets up a loading screen while waiting for some data to load.
 *
 * As soon as the mutex is acquireable by this scene. It will start transitioning out with an
 * intermediary. When the thread STARTS transitioning out, the callback is called with the provided user data.
 *
 * \param mutex the mutex to try acquiring
 * \param min the minimum duration of time to wait until we transition to next scene, to not flash the user on quick
 * transitions. This is in seconds.
 * \param callback the callback to call when transitioning out
 * \param userdata the userdata to call the callback with
 * \returns a loading scene configured
 */
Scene *scene_loading(SDL_Mutex *mutex, double min, void (*callback)(void *), void *userdata);

Scene *scene_000_main(void); // Scene 000: The main menu.
