/**
 * \file sdl/sdl_pixels.h
 *
 * Simple struct wrappers and renames for SDL pixels.
 */

#include <cassert>
#include <cmath>
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
    constexpr color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    /**
     * Converts a floating point color to a integer-based color.
     *
     * \param color the fcolor to convert
     */
    constexpr color(const fcolor &color) noexcept;
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
    constexpr fcolor(const float r, const float g, const float b, const float a) noexcept
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    /**
     * Converts an integer-based color to a floating point color.
     *
     * \param color the color to convert
     */
    constexpr fcolor(const color &color) noexcept;
};

constexpr color::color(const fcolor &color) noexcept
{
    assert(color.r >= 0 && color.r <= 1);
    assert(color.g >= 0 && color.g <= 1);
    assert(color.b >= 0 && color.b <= 1);
    assert(color.a >= 0 && color.a <= 1);
    r = static_cast<uint8_t>(std::lround(color.r * 255));
    g = static_cast<uint8_t>(std::lround(color.g * 255));
    b = static_cast<uint8_t>(std::lround(color.b * 255));
    a = static_cast<uint8_t>(std::lround(color.a * 255));
}

constexpr fcolor::fcolor(const color &color) noexcept
{
    r = static_cast<float>(color.r) / 255;
    g = static_cast<float>(color.g) / 255;
    b = static_cast<float>(color.b) / 255;
    a = static_cast<float>(color.a) / 255;
}

} // namespace sdl
