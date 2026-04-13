/**
 * \file engine/render_origin.h
 *
 * Provides a concept called a rendering origin, and acts as an anchor to shift
 * pixels while rendering.
 */

#pragma once

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
    top_left,      ///< the top left corner of the rectangle
    top_center,    ///< the top center of the rectangle
    top_right,     ///< the top right corner of the rectangle
    middle_left,   ///< the middle left of the rectangle
    middle_center, ///< the middle center of the rectangle
    middle_right,  ///< the middle right of the rectangle
    bottom_left,   ///< the bottom left corner of the rectangle
    bottom_center, ///< the bottom center of the rectangle
    bottom_right,  ///< the bottom right corner of the rectangle
};

/**
 * Shifts the provided rectangle to the correct origin for SDL to render from the top left.
 *
 * \param frect the rectangle to shift
 * \param origin the new render origin
 */
void shift_origin(sdl::frect &frect, const render_origin origin);

} // namespace ccsakura
