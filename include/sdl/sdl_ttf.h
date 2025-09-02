/**
 * \file sdl/sdl_ttf.h
 *
 * Wrappers for SDL's TTF module. Use this module to handle TTF files and rendering fonts on the screen with SDL3's new
 * text engine.
 */

#pragma once

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

} // namespace sdl::ttf
