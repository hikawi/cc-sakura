/**
 * \file sdl/sdl_rect.h
 *
 * Simple wrappers for SDL rectangles with conversions towards each other.
 */

#pragma once

namespace sdl
{

/**
 * Represents a simple rectangle on the screen, using integers for values.
 */
struct rect;

/**
 * Represents a simple rectangle on the screen, using floating points for values.
 */
struct frect;

struct rect
{
    int w; ///< the full width extent of the rectangle
    int h; ///< the full height extent of the rectangle
    int x; ///< the top left corner of the rectangle
    int y; ///< the top right corner of the rectangle

    rect(const frect &frect) noexcept;
};

struct frect
{
    float w; ///< the full width extent of the rectangle
    float h; ///< the full height extent of the rectangle
    float x; ///< the top left corner of the rectangle
    float y; ///< the top right corner of the rectangle

    frect(const rect &rect) noexcept;
};

} // namespace sdl
