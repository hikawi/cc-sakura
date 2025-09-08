/**
 * \file sdl/sdl_ttf.h
 *
 * Wrappers for SDL's TTF module. Use this module to handle TTF files and rendering fonts on the screen with SDL3's new
 * text engine.
 */

#pragma once

#include "sdl/sdl_iostream.h"
#include "sdl/sdl_pixels.h"

#include <memory>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace sdl::ttf
{

/**
 * Represents a font style for rendering fonts.
 */
enum class font_style
{
    normal = TTF_STYLE_NORMAL,               ///< normal font
    bold = TTF_STYLE_BOLD,                   ///< boldened font
    italic = TTF_STYLE_ITALIC,               ///< italicized font
    underline = TTF_STYLE_UNDERLINE,         ///< underlined font
    strikethrough = TTF_STYLE_STRIKETHROUGH, ///< struckthrough font
};

/**
 * Hints at the TTF font for better rendering resolutions.
 */
enum class font_hint
{
    none = TTF_HINTING_NONE,                     ///< don't use any grid-fitting
    mono = TTF_HINTING_MONO,                     ///< monochrome, hints to render better at low resolutions
    normal = TTF_HINTING_NORMAL,                 ///< standard grid-fitting font
    light = TTF_HINTING_LIGHT,                   ///< use subtle adjustments when rendering
    light_subpixel = TTF_HINTING_LIGHT_SUBPIXEL, ///< use subtle adjustments when rendering at subpixel level
    invalid = TTF_HINTING_INVALID,               ///< just invalid font hint lol
};

/**
 * Combines multiple font styles together.
 *
 * \param lhs the font style on the left
 * \param rhs the font style on the right
 */
font_style operator|(const font_style lhs, const font_style rhs) noexcept;

/**
 * Virtual interface as a wrapper for a TTF Font.
 */
class ifont
{
  public:
    virtual ~ifont() = default;
};

/**
 * Represents a font with a specified sp.
 */
class font
{
  public:
    /**
     * Wraps a font.
     *
     * \param io the iostream wrapping a ttf buffer
     * \param sp the font size
     */
    font(const sdl::iostream &io, const float sp);

    /**
     * Sets a font style.
     *
     * \param style the style to pick
     */
    void set_style(const font_style style) const noexcept;

    /**
     * Sets the font's size.
     *
     * \param sp the font size to set to
     */
    void set_size(const float sp) const noexcept;

    /**
     * Hints at the font rendering.
     *
     * \param hint the hint to tell the TTF renderer
     */
    void set_hint(const font_hint hint) const noexcept;

    void render_text_blended(const std::string text, const sdl::color color) const noexcept;

  private:
    std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> m_font;
};

} // namespace sdl::ttf
