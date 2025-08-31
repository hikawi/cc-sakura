#include "app.h"

#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_video.h"

#include <memory>

namespace ccsakura
{

app::app()
    : m_window(std::make_unique<sdl::window>(APPLICATION_NAME, APPLICATION_ORIGINAL_WIDTH, APPLICATION_ORIGINAL_HEIGHT,
                                             sdl::window_flags::resizable | sdl::window_flags::always_on_top)),
      m_renderer(std::make_unique<sdl::renderer>(*m_window, nullptr))
{
    sdl::log_trace("ccsakura::app constructed");
}

const sdl::window &app::get_window() const
{
    return *m_window.get();
}

const sdl::renderer &app::get_renderer() const
{
    return *m_renderer.get();
}

app::~app()
{
    sdl::log_trace("ccsakura::app destructed");
}

} // namespace ccsakura
