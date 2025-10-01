/**
 * \file engine/render.h
 *
 * Simple rendering options as utility for objects rendering themselves.
 */

#pragma once

#include "engine/vec2d.h"
#include "sdl/sdl_rect.h"

namespace ccsakura
{

/**
 * Specifies the location to start rendering.
 *
 * This is to deal with the fact that SDL rendering only starts at top left, this origin would shift all pixels back a
 * certain amount to simulate the process of drawing from a different origin.
 */
enum class render_origin
{
    top_left,
    top_center,
    top_right,
    middle_left,
    middle_center,
    middle_right,
    bottom_left,
    bottom_center,
    bottom_right,
};

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
 * Shifts the provided rectangle to the correct origin for SDL to render from the top left.
 *
 * \param rect the rectangle to shift
 * \param origin the new render origin
 */
void shift_origin(sdl::frect &frect, const render_origin origin);

} // namespace ccsakura
