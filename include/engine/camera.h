/**
 * \file engine/camera.h
 *
 * Provides free functions for handling a camera2d.
 */

#pragma once

#include "engine/vec2d.h"

namespace ccsakura
{

/**
 * Represents a camera in 2D space.
 */
struct camera2d
{
    vec2d position;      ///< the camera's world coordinates
    double rotation = 0; ///< the camera's tilt or rotation, based on the x axis
    double zoom = 1;     ///< the camera's zoom level
    vec2d viewport;      ///< the camera's viewport to render
};

/**
 * Converts a world-space position to screen-space using the given camera.
 *
 * \param world the world position to convert
 * \param cam the active camera
 * \returns the screen-space position
 */
sdl::fpoint world_to_screen(const vec2d &world, const camera2d &cam) noexcept;

/**
 * Converts a world-space position to screen-space using the given camera.
 *
 * \param world the world rectangle
 * \param cam the active camera
 * \returns the screen-space position
 */
sdl::frect world_to_screen(const sdl::frect &world, const camera2d &cam) noexcept;

} // namespace ccsakura
