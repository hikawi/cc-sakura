/**
 * \file sdl/sdl_render.h
 *
 * SDL's version of hardware-accelerated rendering for 2D Graphics. To render 3D graphics, using SDL's GPU module is
 * more advised, but I did not port that.
 */

#pragma once

#include "engine/render.h"
#include "engine/vec2d.h"
#include "sdl/sdl_rect.h"
#include "sdl/sdl_surface.h"
#include "sdl/sdl_video.h"

#include <cstdint>
#include <memory>
#include <optional>
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
 * Virtual interface for a renderer.
 */
class irenderer;

/**
 * Represents options for rendering a piece of texture using a renderer.
 *
 * This provides method chaining options for constructing a render.
 */
struct texture_render_options;

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

  private:
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> m_texture;
};

class irenderer
{
  public:
    virtual ~irenderer() = default;

    /**
     * Retrieves the underlying SDL_Renderer.
     *
     * \returns the sdl renderer
     */
    virtual SDL_Renderer *get() const noexcept = 0;

    /**
     * Creates a new texture for this renderer.
     *
     * \param format the texture's pixel format
     * \param access the texture's access protection
     * \param w the texture's width
     * \param h the texture's height
     * \returns a new texture
     */
    virtual std::unique_ptr<itexture> create_texture(const pixel_format format, const texture_access access,
                                                     const int w, const int h) const = 0;

    /**
     * Creates a new texture for this renderer, using a surface.
     *
     * \param surface the surface to create from
     * \returns a new texture
     */
    virtual std::unique_ptr<itexture> create_texture(const sdl::isurface &surface) const = 0;

    /**
     * Renders a texture based on the provided options.
     *
     * \param options the options for rendering
     */
    virtual void render_texture(const texture_render_options &options) const noexcept = 0;

    /**
     * Sets the renderer's draw color to an RGBA set.
     *
     * \param r the red value
     * \param g the green value
     * \param b the blue value
     * \param a the alpha value
     */
    virtual void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const noexcept = 0;

    /**
     * Sets the renderer's draw color to an RGBA set.
     *
     * \param r the red value
     * \param g the green value
     * \param b the blue value
     * \param a the alpha value
     */
    virtual void set_color(const float r, const float g, const float b, const float a) const noexcept = 0;

    /**
     * Clears the rendering context.
     */
    virtual void clear() const noexcept = 0;

    /**
     * Updates the window with newly drawn context.
     */
    virtual void present() const noexcept = 0;
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
    explicit renderer(const iwindow &window, const char *name);
    ~renderer();

    SDL_Renderer *get() const noexcept override;
    std::unique_ptr<itexture> create_texture(const pixel_format format, const texture_access access, const int w,
                                             const int h) const override;
    std::unique_ptr<itexture> create_texture(const sdl::isurface &surface) const override;
    void render_texture(const texture_render_options &options) const noexcept override;
    void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const noexcept override;
    void set_color(const float r, const float g, const float b, const float a) const noexcept override;
    void clear() const noexcept override;
    void present() const noexcept override;

  private:
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> m_renderer;
    std::string m_name;
};

struct texture_render_options
{
    sdl::itexture &m_texture;                               ///< the texture to be rendered
    std::optional<sdl::frect> m_srcrect = std::nullopt;     ///< where to render from the texture
    std::optional<sdl::frect> m_dstrect = std::nullopt;     ///< where to render to on the renderer
    std::optional<ccsakura::vec2d> m_origin = std::nullopt; ///< the origin to rotate the texture by
    ccsakura::render_origin m_render_origin =
        ccsakura::render_origin::top_left;   ///< the rendering origin to shift the destination by
    sdl::flip m_flip_mode = sdl::flip::none; ///< whether to flip the texture
    double m_rotation = 0;                   ///< the rotation angle of the texture

    /**
     * Constructs a simple texture rendering options.
     *
     * \param texture the texture to render
     * \param renderer the renderer to use
     */
    texture_render_options(sdl::itexture &texture);

    /**
     * Sets the source rectangle for rendering.
     *
     * \param rect The source rectangle within the texture.
     * \returns A reference to the current object for method chaining.
     */
    texture_render_options &srcrect(const sdl::frect rect) noexcept;

    /**
     * Sets the destination position for rendering.
     *
     * The destination rectangle will be sized to the source rectangle.
     *
     * \param pos The position of the top-left corner of the destination.
     * \returns A reference to the current object for method chaining.
     */
    texture_render_options &dst(const ccsakura::vec2d pos) noexcept;

    /**
     * Sets the destination rectangle for rendering.
     *
     * \param rect The destination rectangle on the renderer.
     * \returns A reference to the current object for method chaining.
     */
    texture_render_options &dstrect(const sdl::frect rect) noexcept;

    /**
     * Sets the rotation origin for the texture.
     *
     * \param pos The position of the rotation origin.
     * \returns A reference to the current object for method chaining.
     */
    texture_render_options &origin(const ccsakura::vec2d pos) noexcept;

    /**
     * Sets the rotation origin based on a predefined enumeration.
     *
     * \param origin The predefined render origin.
     * \returns A reference to the current object for method chaining.
     */
    texture_render_options &render_origin(const ccsakura::render_origin origin) noexcept;

    /**
     * Sets the flip mode for the texture.
     *
     * \param flipmode The flip mode to apply (e.g., horizontal or vertical).
     * \returns A reference to the current object for method chaining.
     */
    texture_render_options &flip(const sdl::flip flipmode) noexcept;

    /**
     * Sets the rotation angle for the texture.
     *
     * \param angle The rotation angle in degrees.
     * \returns A reference to the current object for method chaining.
     */
    texture_render_options &rotate(const double angle) noexcept;
};

} // namespace sdl
