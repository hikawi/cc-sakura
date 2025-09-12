/**
 * \file engine/text.h
 *
 * Font-rendering module of the engine.
 */

#pragma once

#include "engine/vec2d.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_stdinc.h"
#include "sdl/sdl_ttf.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

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

    /**
     * Checks if two fonts are equal.
     *
     * \param other the other font to check
     * \returns true if both are equal
     */
    bool operator==(const font &other) const noexcept;

    /**
     * Checks if two fonts are not equal.
     *
     * \param other the other font to check
     * \returns true if both are not equal
     */
    bool operator!=(const font &other) const noexcept;
};

} // namespace ccsakura

// Moved this up to make font hashable.
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

namespace ccsakura
{

/**
 * Virtual interface for a font cache to use.
 *
 * A text should always use a font cache to reduce storage IO. For an embedded controller, maybe reading storage IO is
 * better, but come on.
 */
class ifont_cache
{
  public:
    virtual ~ifont_cache() = default;

    /**
     * Retrieves the cached font instance inside the cache if it exists.
     *
     * Or inserts if it doesn't exist yet.
     *
     * \param font the font to query
     * \returns a reference to the cached font
     */
    virtual sdl::ttf::ifont &operator[](const font font) = 0;

    /**
     * Clears the cache map.
     *
     * This still gets called by the end of the destructor automatically, but you can clear it at anytime for better
     * control of destruction.
     */
    virtual void clear() = 0;
};

/**
 * Concrete implementation of a font cache that uses a simple map.
 */
class font_cache : public ifont_cache
{
  public:
    /**
     * Constructs an empty font cache with an empty font map.
     */
    font_cache();
    ~font_cache();

    sdl::ttf::ifont &operator[](const font font) override;
    void clear() override;

  private:
    std::unordered_map<font, std::unique_ptr<sdl::ttf::ifont>> m_font_map;
};

/**
 * Represents a piece of text on the screen.
 */
class itext
{
  public:
    virtual ~itext() = default;

    /**
     * Sets the underlying of the text in this object.
     *
     * \param text the text to set
     */
    virtual void set_text(const std::string text) noexcept = 0;

    /**
     * Sets the text color to render.
     *
     * \param color the color of the text
     */
    virtual void set_color(const sdl::color color) noexcept = 0;

    /**
     * Sets the position of the text to render.
     *
     * \param pos the position to render at
     */
    virtual void set_position(const ccsakura::vec2d pos) noexcept = 0;

    /**
     * Renders this text instance using the provided renderer to a texture.
     *
     * \param renderer the renderer to use
     * \returns a texture
     */
    virtual std::unique_ptr<sdl::itexture> render(const sdl::irenderer &renderer) const noexcept = 0;
};

/**
 * Represents a piece of text on the screen using a font, rendered with a font engine.
 *
 * This text can be changed without re-rendering
 */
class text : public itext
{
  public:
    /**
     * Constructs a new piece of text handled by the engine.
     *
     * \param font the font to use
     * \param text the starting text
     * \param cache the cache to use for the text
     */
    text(const font font, const std::string text, ifont_cache &cache);
    ~text();

    void set_text(const std::string text) noexcept override;
    void set_color(const sdl::color color) noexcept override;
    void set_position(const ccsakura::vec2d pos) noexcept override;
    std::unique_ptr<sdl::itexture> render(const sdl::irenderer &renderer) const noexcept override;

  private:
    font m_font;
    std::string m_text;
    sdl::color m_color;
    vec2d m_pos;
    ifont_cache &m_cache;
};

} // namespace ccsakura
