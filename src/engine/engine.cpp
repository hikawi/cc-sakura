#include "engine/engine.h"

#include "app.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"

#include <algorithm>
#include <numbers>
#include <stdexcept>

namespace ccsakura
{

engine::engine(std::unique_ptr<iapp> app) : m_app(std::move(app))
{
    if (!m_app)
    {
        throw std::runtime_error("Unable to initialize application");
    }
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
    // Add variable tick handling here

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

    const float RAINBOW_SPEED = 0.001f;
    static float phase = 0.0f;

    phase += RAINBOW_SPEED;

    // Ensure the phase doesn't grow indefinitely.
    // Cycle it back to 0 when it exceeds 2 * PI to prevent precision issues.
    if (phase > 2.0f * std::numbers::pi_v<float>)
    {
        phase -= 2.0f * std::numbers::pi_v<float>;
    }

    // Calculate the value for each color channel using sine waves with different offsets
    float red = 0.5f * (1.0f + std::sin(phase));
    float green = 0.5f * (1.0f + std::sin(phase + (2.0f * std::numbers::pi_v<float> / 3.0f)));
    float blue = 0.5f * (1.0f + std::sin(phase + (4.0f * std::numbers::pi_v<float> / 3.0f)));

    // Apply the new color to the renderer
    renderer.set_color(red, green, blue, 1.0f);
    renderer.clear();
    renderer.present();
}

engine::~engine() noexcept
{
    sdl::log_trace("engine::engine destroyed");
}

} // namespace ccsakura
