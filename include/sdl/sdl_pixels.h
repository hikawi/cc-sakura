/**
 * \file sdl/sdl_pixels.h
 *
 * Simple struct wrappers and renames for SDL pixels.
 */

#pragma once

#include <cassert>
#include <cstdint>

namespace sdl
{

/**
 * Represents a color using bytes to represent each channel.
 *
 * Each number ranges from 0-255.
 */
struct color;

/**
 * Represents a color using floating points to represent each channel.
 *
 * Each floating point ranges from 0-1.
 */
struct fcolor;

struct color
{
    uint8_t r; ///< the red component
    uint8_t g; ///< the green component
    uint8_t b; ///< the blue component
    uint8_t a; ///< the alpha component

    /**
     * Constructs an integer-based color.
     *
     * \param r the red component
     * \param g the green component
     * \param b the blue component
     * \param a the alpha component
     */
    color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept;

    /**
     * Converts a floating point color to a integer-based color.
     *
     * \param color the fcolor to convert
     */
    color(const fcolor &color) noexcept;

    bool operator==(const color &other) const noexcept;
};

struct fcolor
{
    float r; ///< the red component
    float g; ///< the green component
    float b; ///< the blue component
    float a; ///< the alpha component

    /**
     * Constructs a floating point based color.
     *
     * \param r the red component
     * \param g the green component
     * \param b the blue component
     * \param a the alpha component
     */
    fcolor(const float r, const float g, const float b, const float a) noexcept;

    /**
     * Converts an integer-based color to a floating point color.
     *
     * \param color the color to convert
     */
    fcolor(const color &color) noexcept;

    bool operator==(const fcolor &other) const noexcept;
};

} // namespace sdl
