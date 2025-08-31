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
#include <SDL3/SDL_render.h>

namespace sdl
{

/**
 * Abstract interface for \ref sdl::renderer for mocking purposes.
 */
class irenderer
{
  public:
    virtual ~irenderer() = default;

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

    void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const override;
    void set_color(const float r, const float g, const float b, const float a) const override;
    void clear() const override;
    void present() const override;

  private:
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> m_renderer;
};

} // namespace sdl
