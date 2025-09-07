/**
 * \file engine/text.h
 *
 * Font-rendering module of the engine.
 */

#pragma once

#include "sdl/sdl_render.h"
#include "sdl/sdl_stdinc.h"

#include <functional>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace ccsakura
{

/**
 * Enumerations for available typefaces in the engine.
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
    typeface face; ///< the typeface of the font
    float sp;      ///< the font's size in sp

    bool operator==(const font &other) const noexcept;
    bool operator!=(const font &other) const noexcept;
};

/**
 * Represents a piece of text on the screen using a font, rendered with a font engine.
 */
class text
{
  public:
    text(const font font, const std::string text);
    text &operator=(const std::string text) noexcept;
    void render(const sdl::irenderer &renderer) const noexcept;

  private:
    font m_font;
    std::string m_text;
};

} // namespace ccsakura

namespace std
{

template <> struct hash<ccsakura::font>
{
    std::size_t operator()(const ccsakura::font &f) const noexcept
    {
        uint32_t hash = sdl::murmur3(static_cast<uint32_t>(f.face));
        hash = sdl::murmur3(f.sp, hash);
        return static_cast<std::size_t>(hash);
    }
};

} // namespace std
