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
 * Virtual interface for \ref ccsakura::app for mocking purposes.
 */
class iapp
{
  public:
    virtual ~iapp() = default;

    /**
     * Retrieves the wrapped window.
     *
     * \returns the wrapped window
     */
    virtual const sdl::iwindow &get_window() const noexcept = 0;

    /**
     * Retrieves the wrapped renderer.
     *
     * \returns the wrapped renderer
     */
    virtual const sdl::irenderer &get_renderer() const noexcept = 0;
};

/**
 * Represents the application with a state.
 */
class app : public iapp
{
  public:
    app(std::unique_ptr<sdl::iwindow> window, std::unique_ptr<sdl::irenderer> renderer);
    ~app() override;

    const sdl::iwindow &get_window() const noexcept override;
    const sdl::irenderer &get_renderer() const noexcept override;

  private:
    std::unique_ptr<sdl::iwindow> m_window;
    std::unique_ptr<sdl::irenderer> m_renderer;
};

}; // namespace ccsakura
