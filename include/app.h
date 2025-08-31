/**
 * \file app.h
 *
 * Represents the final application state as a state machine.
 */

#pragma once

#include "sdl/sdl_render.h"
#include "sdl/sdl_video.h"

#include <memory>
namespace ccsakura
{

/**
 * Represents the application with a state.
 */
class app
{
  public:
    app();
    ~app();

    /**
     * Retrieves the wrapped window.
     *
     * \returns the wrapped window
     */
    const sdl::window &get_window() const;

    /**
     * Retrieves the wrapped renderer.
     *
     * \returns the wrapped renderer
     */
    const sdl::renderer &get_renderer() const;

  private:
    std::unique_ptr<sdl::window> m_window;
    std::unique_ptr<sdl::renderer> m_renderer;
};

}; // namespace ccsakura
