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
 * Provides a 2D-accelerated rendering context.
 */
class renderer
{
  public:
    /**
     * Create a 2D rendering context for a window.
     *
     * \param window the window to create for
     * \param name the driver name to use. pass nullptr to let SDL automatically choose the best driver from the user's
     * machine
     */
    renderer(const window &window, const char *name);
    ~renderer();

    /**
     * Sets the renderer's draw color to an RGBA set.
     *
     * \param r the red value
     * \param g the green value
     * \param b the blue value
     * \param a the alpha value
     */
    void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const;

    /**
     * Sets the renderer's draw color to an RGBA set.
     *
     * \param r the red value
     * \param g the green value
     * \param b the blue value
     * \param a the alpha value
     */
    void set_color(const float r, const float g, const float b, const float a) const;

    /**
     * Clears the rendering context.
     */
    void clear() const;

    /**
     * Updates the window with newly drawn context.
     */
    void present() const;

  private:
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> m_renderer;
};

} // namespace sdl
