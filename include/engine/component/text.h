/**
 * \file engine/component/text.h
 *
 * Provides a data struct for text-renderable entity.
 */

#pragma once

#include "engine/component.h"
#include "engine/render_origin.h"
#include "sdl/sdl_pixels.h"

#include <string>

namespace ccsakura::components
{

/**
 * Represents a text component rendered using the "font" sprite sheet.
 * Frame 0 = ASCII 32 (space), frame N = ASCII (32 + N), up to ASCII 126.
 * The scene manager resolves the font sprite via sprite::named("font") at render time.
 */
struct text : public component
{
    sdl::fcolor color{1.0f, 1.0f, 1.0f, 1.0f};      ///< Tint color applied to glyphs.
    render_origin origin = render_origin::top_left; ///< Anchor point for the entire text block.
    std::string value{""};                          ///< Text content

    /**
     * Sets the text content.
     *
     * \param str the new text to display
     */
    void set(std::string_view str) noexcept;
};

} // namespace ccsakura::components
