// game/game_scenes.h
//
// Header for creating all types of game scenes.

#pragma once

#include "SDL3/SDL_pixels.h"
#include "engine/scene.h"

/**
 * Sets up the starting scene.
 */
void scene_setup(void);

Scene *scene_empty_init(SDL_Color color);
Scene *scene_fps_init(SDL_Color color);

Scene *scene_000_main(void);
