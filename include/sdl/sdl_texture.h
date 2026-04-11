/**
 * \file sdl/sdl_texture.h
 *
 * SDL texture interface, concrete implementation, and related enumerations.
 */

#pragma once

#include "sdl/sdl_pixels.h"
#include "sdl/sdl_surface.h"

#include <cstdint>
#include <memory>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

namespace sdl
{

/**
 * Textures and renderers pixel format.
 */
enum class pixel_format
{
    rgba8888 = SDL_PIXELFORMAT_RGBA8888, ///< 4 channels, rgba with 8 bits each
};

/**
 * The access pattern allowed for a texture.
 */
enum class texture_access
{
    target = SDL_TEXTUREACCESS_TARGET,       ///< this texture can be used as a rendering target
    unchanging = SDL_TEXTUREACCESS_STATIC,   ///< this texture rarely changes, can't be locked
    streaming = SDL_TEXTUREACCESS_STREAMING, ///< this texture changes a lot, can be locked
};

/**
 * A texture or a renderer's blend mode.
 */
enum class blend_mode
{
    none = SDL_BLENDMODE_NONE,   ///< no blending: dstRGBA = srcRGBA
    blend = SDL_BLENDMODE_BLEND, ///< alpha blending: dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA)), dstA = srcA +
                                 ///< (dstA * (1-srcA))
    blend_premul =
        SDL_BLENDMODE_BLEND_PREMULTIPLIED, ///< pre-multiplied alpha blending: dstRGBA = srcRGBA + (dstRGBA * (1-srcA))
    add = SDL_BLENDMODE_ADD,               ///< additive blending: dstRGB = (srcRGB * srcA) + dstRGB, dstA = dstA
    add_premul =
        SDL_BLENDMODE_ADD_PREMULTIPLIED, ///< pre-multiplied additive blending: dstRGB = srcRGB + dstRGB, dstA = dstA
    mod = SDL_BLENDMODE_MOD,             ///< color modulate: dstRGB = srcRGB * dstRGB, dstA = dstA
    mul = SDL_BLENDMODE_MUL,         ///< color multiply: dstRGB = (srcRGB * dstRGB) + (dstRGB * (1-srcA)), dstA = dstA
    invalid = SDL_BLENDMODE_INVALID, ///< placeholder for invalid mode
};

/**
 * Abstract interface for \ref sdl::texture for mocking purposes.
 */
class itexture
{
  public:
    virtual ~itexture() = default;

    /**
     * Retrieves the underlying SDL texture pointer.
     *
     * \returns the underlying pointer
     */
    virtual SDL_Texture *get() const noexcept = 0;

    /**
     * Retrieves the current blend mode for the texture.
     *
     * \returns the current blend mode
     */
    virtual blend_mode get_blend_mode() const noexcept = 0;

    /**
     * Sets the blend mode for a texture.
     *
     * \param mode the mode to blend the texture
     */
    virtual void set_blend_mode(const blend_mode mode) const noexcept = 0;

    /**
     * Retrieves the current scale mode of the texture.
     *
     * \returns the current scale mode
     */
    virtual scale_mode get_scale_mode() const noexcept = 0;

    /**
     * Changes the texture's scale mode.
     *
     * \param mode the mode to change to
     */
    virtual void set_scale_mode(const scale_mode mode) const noexcept = 0;

    /**
     * Retrieves the current alpha modulation of the texture (0–255).
     *
     * \returns the current alpha mod value
     */
    virtual uint8_t get_alpha_mod() const noexcept = 0;

    /**
     * Sets the alpha modulation applied when this texture is rendered.
     *
     * \param alpha the alpha mod value (0 = fully transparent, 255 = fully opaque)
     */
    virtual void set_alpha_mod(uint8_t alpha) const noexcept = 0;

    /**
     * Retrieves the current RGB color modulation of the texture.
     *
     * \returns the color mod as an fcolor (alpha is always 1.0)
     */
    virtual sdl::fcolor get_color_mod() const noexcept = 0;

    /**
     * Sets the RGB color modulation applied when this texture is rendered.
     *
     * \param r red multiplier (0.0 = none, 1.0 = full)
     * \param g green multiplier
     * \param b blue multiplier
     */
    virtual void set_color_mod(float r, float g, float b) const noexcept = 0;
};

/**
 * Concrete implementation of an SDL texture.
 */
class texture : public itexture
{
  public:
    explicit texture(std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture);
    ~texture();

    SDL_Texture *get() const noexcept override;
    blend_mode get_blend_mode() const noexcept override;
    void set_blend_mode(const blend_mode mode) const noexcept override;
    scale_mode get_scale_mode() const noexcept override;
    void set_scale_mode(const scale_mode mode) const noexcept override;
    uint8_t get_alpha_mod() const noexcept override;
    void set_alpha_mod(uint8_t alpha) const noexcept override;
    sdl::fcolor get_color_mod() const noexcept override;
    void set_color_mod(float r, float g, float b) const noexcept override;

  private:
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> m_texture;
};

} // namespace sdl
