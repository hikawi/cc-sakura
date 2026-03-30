#include "engine/engine.h"

#include "engine/component.h"
#include "engine/entity.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_storage.h"

#include <algorithm>
#include <stdexcept>

namespace ccsakura
{

bool engine_deps::is_valid() const noexcept
{
    return m_renderer && m_window && m_app && m_font_cache && m_sprite_cache;
}

engine::engine(engine_deps &&deps) : m_deps(std::move(deps))
{
    if (!m_deps.is_valid())
    {
        sdl::log_error("Failed to initialize engine: some dependencies are missing");
        throw std::runtime_error("Unable to initialize engine");
    }

    sdl::log_trace("ccsakura::engine constructed");

    // Let engine inject dependencies
    sdl::log_debug("Injecting Sprite dependencies");
    sprite::use_cache(*m_deps.m_sprite_cache);
    sprite::use_renderer(*m_deps.m_renderer);
    sprite::use_storage_opener(sdl::open_title_storage);
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
    const sdl::irenderer &renderer = *m_deps.m_renderer;
    renderer.set_blend_mode(sdl::blend_mode::blend);

    renderer.set_color(static_cast<uint8_t>(255), 255, 255, 255);
    renderer.clear();

    static entity test_entity(1);
    if (!test_entity.has_component<components::transform>())
    {
        test_entity.add_component<components::transform>(vec2d(100, 100));
    }

    components::transform &comp = test_entity.get_component<components::transform>();

    // Update position for testing
    comp.position.x += 0.001;
    if (comp.position.x > APPLICATION_ORIGINAL_WIDTH)
    {
        comp.position.x = 0;
    }

    // Render a 64x64 square
    const float size = 64.0f;
    const float half_size = size / 2.0f;
    const sdl::fcolor color(1.0f, 0.0f, 0.0f, 1.0f); // Red square

    sdl::render_geometry_options opts(renderer);
    opts.add_vertex(sdl::vertex(sdl::fpoint(static_cast<float>(comp.position.x) - half_size,
                                            static_cast<float>(comp.position.y) - half_size),
                                color))
        .add_vertex(sdl::vertex(sdl::fpoint(static_cast<float>(comp.position.x) + half_size,
                                            static_cast<float>(comp.position.y) - half_size),
                                color))
        .add_vertex(sdl::vertex(sdl::fpoint(static_cast<float>(comp.position.x) + half_size,
                                            static_cast<float>(comp.position.y) + half_size),
                                color))
        .add_vertex(sdl::vertex(sdl::fpoint(static_cast<float>(comp.position.x) - half_size,
                                            static_cast<float>(comp.position.y) + half_size),
                                color));

    opts.connect(0, 1, 2);
    opts.connect(0, 2, 3);
    opts.render();

    renderer.present();
}

engine::~engine() noexcept
{
    sdl::log_trace("engine::engine destroyed");
}

} // namespace ccsakura
