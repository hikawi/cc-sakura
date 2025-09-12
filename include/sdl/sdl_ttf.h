/**
 * \file sdl/sdl_ttf.h
 *
 * Wrappers for SDL's TTF module. Use this module to handle TTF files and rendering fonts on the screen with SDL3's new
 * text engine.
 */

#pragma once

#include "sdl/sdl_iostream.h"
#include "sdl/sdl_pixels.h"
#include "sdl/sdl_surface.h"

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

    /**
     * Retrieves the underlying TTF Font pointer.
     *
     * \returns the underlying pointer
     */
    virtual TTF_Font *get() const noexcept = 0;

    /**
     * Sets a font style.
     *
     * \param style the style to pick
     */
    virtual void set_style(const font_style style) const noexcept = 0;

    /**
     * Sets the font's size.
     *
     * \param sp the font size to set to
     */
    virtual void set_size(const float sp) const noexcept = 0;

    /**
     * Hints at the font rendering.
     *
     * \param hint the hint to tell the TTF renderer
     */
    virtual void set_hint(const font_hint hint) const noexcept = 0;

    /**
     * Renders a piece of text in blended mode to an SDL surface.
     *
     * \param text the text to render
     * \param color the color to render the text in
     * \returns an SDL surface
     */
    virtual std::unique_ptr<sdl::isurface> render_text_blended(const std::string text,
                                                               const sdl::color color) const noexcept = 0;
};

/**
 * Represents a font with a specified sp.
 */
class font : public ifont
{
  public:
    /**
     * Wraps a font.
     *
     * \param io the iostream wrapping a ttf buffer
     * \param sp the font size
     */
    font(std::shared_ptr<sdl::iiostream> io, const float sp);
    ~font();

    TTF_Font *get() const noexcept override;
    void set_style(const font_style style) const noexcept override;
    void set_size(const float sp) const noexcept override;
    void set_hint(const font_hint hint) const noexcept override;
    std::unique_ptr<sdl::isurface> render_text_blended(const std::string text,
                                                       const sdl::color color) const noexcept override;

  private:
    std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> m_font;
    std::shared_ptr<sdl::iiostream> m_iostream;
};

/**
 * Virtual interface for a piece of text that can be changed on the fly.
 */
class itext
{
  public:
    ~itext();
};

/**
 * Concrete implementation of a piece of text on the screen.
 *
 * This text is tied to the font, changing the font will change this text. The font bound to this text can also be
 * changed to another font. This text owns a part of the font, as long as this text is alive, the font can not be
 * deallocated.
 */
class text : public itext
{
  public:
    explicit text(std::shared_ptr<ifont> font);
    ~text();

  private:
    std::shared_ptr<ifont> m_font;
    std::unique_ptr<TTF_Text, void (*)(TTF_Text *)> m_text;
};

} // namespace sdl::ttf
