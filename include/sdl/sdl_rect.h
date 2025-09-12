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
    int x; ///< the top left corner of the rectangle
    int y; ///< the top right corner of the rectangle
    int w; ///< the full width extent of the rectangle
    int h; ///< the full height extent of the rectangle

    /**
     * Constructs an all-zero rectangle.
     */
    rect() noexcept;

    /**
     * Constructs a rectangle with pre-configured values.
     *
     * \param x the x component
     * \param y the y component
     * \param w the w component
     * \param h the h component
     */
    rect(const int x, const int y, const int w, const int h) noexcept;

    /**
     * Converts a floating rectangle to an integer rectangle.
     *
     * \param frect the floating-based rectangle to convert
     */
    rect(const frect &frect) noexcept;
};

struct frect
{
    float x; ///< the top left corner of the rectangle
    float y; ///< the top right corner of the rectangle
    float w; ///< the full width extent of the rectangle
    float h; ///< the full height extent of the rectangle

    /**
     * Constructs an all-zero floating-based rectangle.
     */
    frect() noexcept;

    /**
     * Constructs a floating-based rectangle with pre-configured values.
     *
     * \param x the x component
     * \param y the y component
     * \param w the w component
     * \param h the h component
     */
    frect(const float x, const float y, const float w, const float h) noexcept;

    /**
     * Converts an integer rectangle to floating-based rectangle.
     *
     * \param rect the integer rectangle to convert
     */
    frect(const rect &rect) noexcept;
};

} // namespace sdl
