// game/game_scenes.h
//
// Header for creating all types of game scenes.

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
 * @param color - The color to paint with.
 */
Scene *scene_empty(const SDL_Color color);

/**
 * Sets up a simple FPS scene that paints a FPS counter on the top right of the
 * screen of the provided color.
 *
 * @param color - The text's color.
 */
Scene *scene_fps(const SDL_Color color);

/**
 * Sets up a loading screen while waiting for some data to load. As soon as the
 * mutex is acquireable by this scene. It will start transitioning out with an
 * intermediary.
 */
Scene *scene_loading(SDL_Mutex *mutex);

Scene *scene_000_main(void); // Scene 000: The main menu.
