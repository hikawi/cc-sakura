/**
 * \file sdl/sdl_render.h
 *
 * SDL's version of hardware-accelerated rendering for 2D Graphics. To render 3D graphics, using SDL's GPU module is
 * more advised, but I did not port that.
 */

#pragma once

#include "sdl/sdl_pixels.h"
#include "sdl/sdl_rect.h"
#include "sdl/sdl_surface.h"
#include "sdl/sdl_texture.h"
#include "sdl/sdl_video.h"

#include <cstdint>
#include <memory>
#include <SDL3/SDL_render.h>
#include <vector>

namespace sdl
{

/**
 * Logical presentation mode for a renderer.
 */
enum class logical_presentation
{
    disabled = SDL_LOGICAL_PRESENTATION_DISABLED,           ///< disabled logical presentation
    stretch = SDL_LOGICAL_PRESENTATION_STRETCH,             ///< stretch to fill the window
    letterbox = SDL_LOGICAL_PRESENTATION_LETTERBOX,         ///< letterbox to fill the window
    overscan = SDL_LOGICAL_PRESENTATION_OVERSCAN,           ///< overscan to fill the window
    integer_scale = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE, ///< integer scale to fill the window
};

/**
 * Represents a vertex within 2D space.
 */
struct vertex
{
    fpoint pos;       ///< the position of the vertex
    fcolor color;     ///< the color to paint for the vertex
    fpoint tex_coord; ///< the coords on the texture to use, if available

    /**
     * Constructs a new vertex with a position and a color.
     *
     * \param pos the position of the vertex
     * \param color the color to use
     */
    vertex(const fpoint pos, const fcolor color);

    /**
     * Constructs a new vertex with all parameters.
     *
     * \param pos the position of the vertex
     * \param color the color to use
     * \param tex_coord the position on the texture to draw
     */
    vertex(const fpoint pos, const fcolor color, const fpoint tex_coord);

    /**
     * Checks if two vertices are equal.
     *
     * \param other the other vetex to check
     * \returns true if both vertices are equal
     */
    bool operator==(const vertex &other) const noexcept;

    /**
     * Checks if two vertices are not equal.
     *
     * \param other the other vetex to check
     * \returns true if both vertices are inequal.
     */
    bool operator!=(const vertex &other) const noexcept;

    /**
     * Converts the vertex into SDL's struct of vertex.
     *
     * \returns the SDL vertex
     */
    SDL_Vertex to_sdl() const noexcept;
};

/**
 * Virtual interface for a renderer.
 */
class irenderer;

/**
 * Plain old data object for a geometry rendering call.
 */
struct render_geometry_options
{
    const irenderer &renderer;    ///< the renderer to use
    itexture *texture = nullptr;  ///< the texture to use, optional
    std::vector<vertex> vertices; ///< the vertices of triangles
    std::vector<int> indices;     ///< the indices mapping of triangles in groups of 3

    /**
     * Constructs a new geometry rendering options.
     *
     * \param renderer the renderer to use
     */
    render_geometry_options(const irenderer &renderer);

    /**
     * Sets the texture to use to render with.
     *
     * \param txt the texture to use
     * \returns the render geometry builder
     */
    render_geometry_options &use_texture(sdl::itexture &txt) noexcept;

    /**
     * Adds a new vertex to the list of geometry to render
     *
     * \param vertex the vertex to add
     * \returns the render geometry builder
     */
    render_geometry_options &add_vertex(const sdl::vertex &vertex) noexcept;

    /**
     * Connects three vertices into a triangle.
     *
     * \param a the index to the first vertex
     * \param b the index to the second vertex
     * \param c the index to the third vertex
     * \returns the render geometry builder
     */
    render_geometry_options &connect(const int a, const int b, const int c) noexcept;

    /**
     * Pass all arguments set up to the renderer's genometry rendering options.
     */
    void render() const noexcept;
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
     * Sets the renderer's logical presentation.
     *
     * \param w the logical width
     * \param h the logical height
     * \param mode the presentation mode
     */
    virtual void set_logical_presentation(const int w, const int h, const logical_presentation mode) const noexcept = 0;

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
     * Renders a texture. Maps directly to SDL_RenderTexture.
     *
     * \param texture the texture to render
     * \param srcrect source rectangle, or nullptr for the full texture
     * \param dstrect destination rectangle, or nullptr for the full renderer
     */
    virtual void render_texture(sdl::itexture &texture, const sdl::frect *srcrect,
                                const sdl::frect *dstrect) const noexcept = 0;

    /**
     * Renders a texture with rotation and flip. Maps directly to SDL_RenderTextureRotated.
     *
     * \param texture the texture to render
     * \param srcrect source rectangle, or nullptr for the full texture
     * \param dstrect destination rectangle, or nullptr for the full renderer
     * \param angle rotation angle in degrees, clockwise
     * \param center rotation center point, or nullptr for dstrect center
     * \param flip_mode flip mode to apply
     */
    virtual void render_texture_rotated(sdl::itexture &texture, const sdl::frect *srcrect, const sdl::frect *dstrect,
                                        double angle, const sdl::fpoint *center,
                                        sdl::flip flip_mode) const noexcept = 0;

    /**
     * Renders a texture with an affine transform. Maps directly to SDL_RenderTextureAffine.
     *
     * \param texture the texture to render
     * \param srcrect source rectangle, or nullptr for the full texture
     * \param origin top-left corner of the destination, or nullptr for (0,0)
     * \param right direction and length of the destination's right edge
     * \param down direction and length of the destination's bottom edge
     */
    virtual void render_texture_affine(sdl::itexture &texture, const sdl::frect *srcrect, const sdl::fpoint *origin,
                                       const sdl::fpoint *right, const sdl::fpoint *down) const noexcept = 0;

    /**
     * Renders a rectangle on the screen.
     *
     * \param rect the rectangle to render
     */
    virtual void render_fill_rect(const sdl::frect &rect) const noexcept = 0;

    /**
     * Render triangles on the renderer.
     *
     * \param opts the options to use for rendering geometry
     */
    virtual void render_geometry(const render_geometry_options &opts) const noexcept = 0;

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
     * Converts window coordinates to renderer logical coordinates.
     *
     * \param x the window x coordinate
     * \param y the window y coordinate
     * \returns the logical coordinate as an fpoint
     */
    virtual sdl::fpoint coordinates_from_window(float x, float y) const noexcept = 0;

    /**
     * Clears the rendering context.
     */
    virtual void clear() const noexcept = 0;

    /**
     * Updates the window with newly drawn context.
     */
    virtual void present() const noexcept = 0;

    /**
     * Sets the render target for subsequent drawing operations.
     * Pass nullptr to restore rendering to the window.
     *
     * \param texture the target texture, or nullptr to restore window
     */
    virtual void set_render_target(sdl::itexture *texture) const noexcept = 0;
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
    blend_mode get_blend_mode() const noexcept override;
    void set_blend_mode(const blend_mode mode) const noexcept override;
    void set_logical_presentation(const int w, const int h, const logical_presentation mode) const noexcept override;
    std::unique_ptr<itexture> create_texture(const pixel_format format, const texture_access access, const int w,
                                             const int h) const override;
    std::unique_ptr<itexture> create_texture(const sdl::isurface &surface) const override;
    void render_texture(sdl::itexture &texture, const sdl::frect *srcrect,
                        const sdl::frect *dstrect) const noexcept override;
    void render_texture_rotated(sdl::itexture &texture, const sdl::frect *srcrect, const sdl::frect *dstrect,
                                double angle, const sdl::fpoint *center, sdl::flip flip_mode) const noexcept override;
    void render_texture_affine(sdl::itexture &texture, const sdl::frect *srcrect, const sdl::fpoint *origin,
                               const sdl::fpoint *right, const sdl::fpoint *down) const noexcept override;
    void render_fill_rect(const sdl::frect &rect) const noexcept override;
    void render_geometry(const render_geometry_options &opts) const noexcept override;
    void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const noexcept override;
    void set_color(const float r, const float g, const float b, const float a) const noexcept override;
    sdl::fpoint coordinates_from_window(float x, float y) const noexcept override;
    void clear() const noexcept override;
    void present() const noexcept override;
    void set_render_target(sdl::itexture *texture) const noexcept override;

  private:
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> m_renderer;
    std::string m_name;
};

} // namespace sdl
