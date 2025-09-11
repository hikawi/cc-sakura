#include "engine/engine.h"

#include "app.h"
#include "engine/text.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"

#include <algorithm>
#include <format>
#include <stdexcept>

namespace ccsakura
{

engine::engine(std::unique_ptr<iapp> app) : m_app(std::move(app))
{
    if (!m_app)
    {
        throw std::runtime_error("Unable to initialize application");
    }

    sdl::log_trace("ccsakura::engine constructed");
}

frame_data engine::get_frame_data() const
{
    return m_frame_data;
}

bool engine::iterate(const uint64_t tick) noexcept
{
    // Calculate delta time
    const double dt = std::min(s_max_dt, static_cast<double>(tick - m_frame_data.last_tick) / 1000);
    m_frame_data.last_tick = tick;
    m_frame_data.accumulator += dt;

    // TODO:
    // Add signals handling here

    // Call fixed update if and only if frame_accum has passed
    // enough for 1 frame time (1 / FPS), for 60FPS this is about 16ms.
    while (m_frame_data.accumulator >= s_time_step)
    {
        sdl::log_trace("Running a physical tick at {}", tick);
        m_frame_data.accumulator -= s_time_step;

        // TODO:
        // Add signals handling collisions and physical ticking here
    }

    // TODO:
    // Add tick handling here

    // Handle FPS
    m_frame_data.cur_frames++;
    m_frame_data.frame_time += dt;
    if (m_frame_data.frame_time >= 1.0)
    {
        m_frame_data.frame_time -= 1;
        m_frame_data.fps = m_frame_data.cur_frames;
        sdl::log_debug("FPS clocked at {}: {} FPS", tick, m_frame_data.fps);
        m_frame_data.cur_frames = 0;
    }

    return true;
}

void engine::render() const noexcept
{
    const sdl::irenderer &renderer = m_app->get_renderer();

    renderer.set_color(static_cast<uint8_t>(255), 255, 255, 255);
    renderer.clear();

    std::string str = std::format("{} FPS", m_frame_data.fps);
    ccsakura::text fps_text({ccsakura::typeface::rainy_hearts, 16}, str);
    std::unique_ptr<sdl::itexture> fps_texture = fps_text.render(renderer);
    renderer.render_texture(sdl::texture_render_options(*fps_texture).dst({0, 0}));

    renderer.present();
}

engine::~engine() noexcept
{
    sdl::log_trace("engine::engine destroyed");
}

} // namespace ccsakura
