// game/game_scenes.h
//
// Header for creating all types of game scenes.

#pragma once

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "engine/scene.h"

/**
 * Sets up the starting scene.
 */
void scene_setup(void);

/**
 * Initializes an empty scene with the render draw color.
 */
Scene *scene_empty_init(SDL_Color color, SDL_FRect rect);

/**
 * Initializes the FPS scene.
 */
Scene *scene_fps_init(SDL_Color color);
