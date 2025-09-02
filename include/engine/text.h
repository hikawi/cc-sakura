/**
 * \file engine/text.h
 *
 * Font-rendering module of the engine.
 */

#pragma once

#include "sdl/sdl_stdinc.h"

#include <functional>

namespace ccsakura
{

/**
 * Enumerations for available typefaces in the engine/
 */
enum class typeface
{
    rainy_hearts, ///< the main typeface of the game
    daydream,     ///< the typeface for large decorative text pieces
    unifont,      ///< unicode-compatible typeface, mostly for debugging purposes
};

/**
 * Represents a combination of options for a font.
 *
 * This is cached. You can always reuse fonts by specifying the same
 */
struct font
{
    typeface typeface; ///< the typeface of the font
    float sp;          ///< the font's size in sp
};

/**
 * Represents a piece of text on the screen using a font, rendered with a font engine.
 */
class text
{
};

} // namespace ccsakura

namespace std
{

template <> struct hash<ccsakura::font>
{
    std::size_t operator()(const ccsakura::font &f) const noexcept
    {
        uint32_t hash = sdl::murmur3(static_cast<uint32_t>(f.typeface));
        hash = sdl::murmur3(f.sp, hash);
        return static_cast<std::size_t>(hash);
    }
};

} // namespace std
