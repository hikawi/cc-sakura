/**
 * \file engine/render_helper.h
 *
 * Provides a simple way to construct a texture for passing into the SDL renderer.
 */

#pragma once

#include "engine/render_origin.h"
#include "sdl/sdl_render.h"

#include <optional>

namespace ccsakura
{

/**
 * Represents options for rendering a piece of texture using a renderer.
 *
 * This provides method chaining options for constructing a render. Lives in the engine layer
 * because it carries engine-level concerns (render_origin, color modulation).
 */
struct render_texture_options
{
    const sdl::irenderer &m_renderer;
    sdl::itexture &m_texture;                                     ///< the texture to be rendered
    std::optional<sdl::frect> m_srcrect = std::nullopt;           ///< where to render from the texture
    std::optional<sdl::frect> m_dstrect = std::nullopt;           ///< where to render to on the renderer
    std::optional<sdl::fpoint> m_origin = std::nullopt;           ///< the rotation center point
    enum render_origin m_render_origin = render_origin::top_left; ///< the rendering origin to shift the destination by
    sdl::flip m_flip_mode = sdl::flip::none;                      ///< whether to flip the texture
    double m_rotation = 0;                                        ///< the rotation angle of the texture
    std::optional<sdl::fcolor> m_color_mod = std::nullopt;        ///< RGB color modulation to apply during render

    /**
     * Constructs a simple texture rendering options.
     *
     * \param renderer the renderer to use
     * \param texture the texture to render
     */
    render_texture_options(const sdl::irenderer &renderer, sdl::itexture &texture);

    /**
     * Sets the source rectangle for rendering.
     *
     * \param rect The source rectangle within the texture.
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &srcrect(const sdl::frect rect) noexcept;

    /**
     * Sets the destination position for rendering.
     *
     * The destination rectangle will be sized to the source rectangle.
     *
     * \param pos The position of the top-left corner of the destination.
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &dst(const sdl::fpoint pos) noexcept;

    /**
     * Sets the destination rectangle for rendering.
     *
     * \param rect The destination rectangle on the renderer.
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &dstrect(const sdl::frect rect) noexcept;

    /**
     * Sets the rotation center point.
     *
     * \param pos The position of the rotation origin.
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &origin(const sdl::fpoint pos) noexcept;

    /**
     * Sets the rendering origin used to shift the destination rectangle.
     *
     * \param origin The predefined render origin.
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &render_origin(const ccsakura::render_origin origin) noexcept;

    /**
     * Sets the flip mode for the texture.
     *
     * \param flipmode The flip mode to apply (e.g., horizontal or vertical).
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &flip(const sdl::flip flipmode) noexcept;

    /**
     * Sets the rotation angle for the texture.
     *
     * \param angle The rotation angle in degrees.
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &rotate(const double angle) noexcept;

    /**
     * Sets the RGB color modulation applied for this render call.
     *
     * The texture's color mod is restored to white (1, 1, 1) after rendering.
     *
     * \param c the color to modulate with (alpha is ignored)
     * \returns A reference to the current object for method chaining.
     */
    render_texture_options &color_mod(sdl::fcolor c) noexcept;

    /**
     * Renders the texture using the provided options and renderer.
     */
    void render() const noexcept;
};

} // namespace ccsakura
