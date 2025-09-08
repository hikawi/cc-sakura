#include "app.h"

#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_video.h"

#include <memory>

namespace ccsakura
{

app::app(std::unique_ptr<sdl::iwindow> window, std::unique_ptr<sdl::irenderer> renderer)
    : m_window(std::move(window)), m_renderer(std::move(renderer))
{
    if (!m_window)
    {
        throw std::runtime_error("Unable to initialize application, window is not available");
    }
    if (!m_renderer)
    {
        throw std::runtime_error("Unable to initialize application, window is not available");
    }

    sdl::log_trace("ccsakura::app constructed");
}

const sdl::iwindow &app::get_window() const noexcept
{
    return *m_window.get();
}

const sdl::irenderer &app::get_renderer() const noexcept
{
    return *m_renderer.get();
}

app::~app()
{
    sdl::log_trace("ccsakura::app destructed");
}

} // namespace ccsakura
