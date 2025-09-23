/**
 * \file engine/sprite.h
 *
 * Represents a single sprite holding a texture.
 */

#pragma once

#include "engine/render.h"
#include "engine/vec2d.h"
#include "sdl/sdl_iostream.h"
#include "sdl/sdl_rect.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_storage.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef __APPLE__
namespace std
{

template <> struct hash<std::string>
{
    size_t operator()(const std::string &s) const noexcept
    {
        size_t hash = 1469598103934665603ULL;
        for (const char c : s)
        {
            hash ^= static_cast<unsigned char>(c);
            hash *= 1099511628211ULL;
        }
        return hash;
    }
};

} // namespace std
#endif

namespace ccsakura
{

/**
 * Represents a single frame of a sprite.
 */
struct sprite_frame
{
    sdl::rect frame;           ///< The frame where the sprite is in the main texture.
    sdl::rect spr_source_size; ///< The sprite's frame in the original source box.
    uint32_t source_w;         ///< Width of the original source box.
    uint32_t source_h;         ///< Height of the original source box.
    uint32_t duration;         ///< The duration that an animation can stay for.
};

/**
 * Represents a tag that tags a sequence of frames with a name.
 */
struct sprite_frame_tag
{
    std::string name; ///< The name of the tag
    uint32_t from;    ///< The index where the tag sequence starts, inclusive
    uint32_t to;      ///< The index where the tag sequence ends, inclusive
};

/**
 * Virtual interface for a Sprite Object.
 */
class isprite
{
  public:
    virtual ~isprite() = default;

    /**
     * Renders the sprite onto a renderer at a certain position.
     *
     * \param renderer the renderer to use
     * \param pos the position to render at
     * \param origin the origin type to shift the position by
     */
    virtual void render(const sdl::irenderer &renderer, const vec2d pos, const render_origin origin) const noexcept = 0;
};

/**
 * A virtual interface to provide a cache for sprites.
 */
class isprite_cache
{
  public:
    virtual ~isprite_cache() = default;

    /**
     * Retrieves the sprite using a name.
     *
     * \param name the name of the sprite to get
     */
    virtual isprite &operator[](std::string name) = 0;

    /**
     * Inserts a value into the sprite cache.
     *
     * \param key the key to associate to
     * \param value the sprite to associate with
     */
    virtual void insert(const std::string key, std::unique_ptr<isprite> value) noexcept = 0;

    /**
     * Checks if the current cache contains the provided key.
     *
     * \param name the key to query
     * \returns true if the cache contains the key
     */
    virtual bool has(const std::string name) const noexcept = 0;

    /**
     * Clears all cached sprites.
     */
    virtual void clear() noexcept = 0;
};

/**
 * Concrete implementation for a sprite cache that uses a static map.
 */
class sprite_cache : public isprite_cache
{
  public:
    sprite_cache();
    ~sprite_cache() override;

    isprite &operator[](std::string name) override;
    void insert(const std::string key, std::unique_ptr<isprite> value) noexcept override;
    bool has(const std::string name) const noexcept override;
    void clear() noexcept override;

  private:
    std::unordered_map<std::string, std::unique_ptr<isprite>> m_spr_cache;
};

/**
 * Represents a sprite object in the engine.
 *
 * A sprite has owner over a texture, that can be rendered. A sprite can belong to multiple objects, but a sprite should
 * only be loaded once.
 */
class sprite : public isprite
{
  public:
    /**
     * Loads and saves a sprite.
     *
     * \param name the name to load the sprite, and also the name to retrieve it back by
     * \param io the io stream containing data for a .sprite file
     * \param renderer the renderer to use
     */
    sprite(std::string name, sdl::iiostream &io, sdl::irenderer &renderer);
    ~sprite();

    void render(const sdl::irenderer &renderer, const vec2d pos, const render_origin origin) const noexcept override;

    /**
     * Retrieves a sprite with a name.
     *
     * \param name the sprite name
     * \returns a reference to the saved sprite
     */
    static isprite &named(const std::string name);

    /**
     * Applies the cache that the sprite is using.
     *
     * \param cache the cache to use
     */
    static void use_cache(isprite_cache &cache);

    /**
     * Applies the storage opener for the sprite class to use.
     *
     * This function should open a unique pointer to a storage.
     *
     * \param func the function to open a unique pointer to a stoarge
     */
    static void use_storage_opener(std::function<std::unique_ptr<sdl::istorage>()> func);

    /**
     * Applies the renderer to use for the sprite class.
     *
     * \param renderer the renderer to use
     */
    static void use_renderer(sdl::irenderer &renderer);

  private:
    void setup(sdl::iiostream &io, sdl::irenderer &renderer);
    void setup_v1(sdl::iiostream &io, sdl::irenderer &renderer);

  private:
    std::unique_ptr<sdl::itexture> m_texture;                 ///< The texture that this sprite owns
    std::string m_name;                                       ///< The name of the sprite
    std::vector<sprite_frame> m_frames;                       ///< A list of frames inside the sprite
    std::unordered_map<std::string, sprite_frame_tag> m_tags; ///< A mapping from name to a frame tag
    uint32_t m_height;                                        ///< the height of the texture, cached
    uint32_t m_width;                                         ///< the width of the texture, cached

    static isprite_cache *s_cache;
    static std::function<std::unique_ptr<sdl::istorage>()> s_open_storage;
    static sdl::irenderer *s_renderer;
};

} // namespace ccsakura
