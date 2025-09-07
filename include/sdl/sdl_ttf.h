/**
 * \file sdl/sdl_ttf.h
 *
 * Wrappers for SDL's TTF module. Use this module to handle TTF files and rendering fonts on the screen with SDL3's new
 * text engine.
 */

#pragma once

#include "sdl/sdl_iostream.h"

#include <memory>
#include <SDL3_ttf/SDL_ttf.h>

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
 * Combines multiple font styles together.
 *
 * \param lhs the font style on the left
 * \param rhs the font style on the right
 */
font_style operator|(const font_style lhs, const font_style rhs) noexcept;

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

  private:
    std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> m_font;
};

} // namespace sdl::ttf
