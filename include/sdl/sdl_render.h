/**
 * \file sdl/sdl_render.h
 *
 * SDL's version of hardware-accelerated rendering for 2D Graphics. To render 3D graphics, using SDL's GPU module is
 * more advised, but I did not port that.
 */

#pragma once

#include "sdl/sdl_video.h"

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
};

class texture : public itexture
{
  public:
    explicit texture(std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture);

    blend_mode get_blend_mode() const noexcept override;
    void set_blend_mode(const blend_mode mode) const noexcept override;

  private:
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> m_texture;
};

/**
 * Abstract interface for \ref sdl::renderer for mocking purposes.
 */
class irenderer
{
  public:
    virtual ~irenderer() = default;

    /**
     * Creates a new texture for this renderer.
     *
     * \param format the texture's pixel format
     * \param access the texture's access protection
     * \param w the texture's width
     * \param h the texture's height
     * \returns a new texture
     */
    virtual std::unique_ptr<itexture> create_texture(pixel_format format, texture_access access, int w,
                                                     int h) const = 0;

    /**
     * Sets the renderer's draw color to an RGBA set.
     *
     * \param r the red value
     * \param g the green value
     * \param b the blue value
     * \param a the alpha value
     */
    virtual void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const = 0;

    /**
     * Sets the renderer's draw color to an RGBA set.
     *
     * \param r the red value
     * \param g the green value
     * \param b the blue value
     * \param a the alpha value
     */
    virtual void set_color(const float r, const float g, const float b, const float a) const = 0;

    /**
     * Clears the rendering context.
     */
    virtual void clear() const = 0;

    /**
     * Updates the window with newly drawn context.
     */
    virtual void present() const = 0;
};

/**
 * Provides a 2D-accelerated rendering context.
 */
class renderer : public irenderer
{
  public:
    /**
     * Create a 2D rendering context for a window.
     *
     * \param window the window to create for
     * \param name the driver name to use. pass nullptr to let SDL automatically choose the best driver from the user's
     * machine
     */
    renderer(const iwindow &window, const char *name);
    ~renderer();

    std::unique_ptr<itexture> create_texture(pixel_format format, texture_access access, int w, int h) const override;
    void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const override;
    void set_color(const float r, const float g, const float b, const float a) const override;
    void clear() const override;
    void present() const override;

  private:
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> m_renderer;
};

} // namespace sdl
