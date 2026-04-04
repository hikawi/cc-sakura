/**
 * \file engine/text.h
 *
 * Font-rendering module of the engine.
 */

#pragma once

#include "sdl/sdl_rect.h"
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
 * Represents a piece of text on the screen using a font, rendered with a font engine.
 *
 * This text can be changed without re-rendering
 */
struct text
{
    typeface face;
    float sp;
    std::string value{""};
    sdl::color color{0, 0, 0, 255};
    sdl::fpoint position{0, 0};

    /**
     * Constructs a new Text object using a typeface and the size.
     *
     * This will reuse a font loaded inside the cache if it matches the parameters.
     * This throws if no cache has been provided.
     *
     * \param face the typeface to use
     * \param sp the font's size
     */
    text(const typeface face, const float sp);
    ~text();

    /**
     * Renders the text into a piece of texture for use with the current renderer.
     *
     * \param renderer the renderer to use
     * \returns a unique pointer to an sdl texture
     */
    std::unique_ptr<sdl::itexture> render(const sdl::irenderer &renderer) const noexcept;

    /**
     * Sets the cache for the text engine to use.
     *
     * \param cache the cache to use
     */
    static void use_cache(ifont_cache &cache);

  private:
    static ifont_cache *s_cache;
};

} // namespace ccsakura
