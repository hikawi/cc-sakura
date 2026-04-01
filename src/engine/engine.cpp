#include "engine/engine.h"

#include "engine/scene.h"
#include "scenes/scene_dbg_empty.h"
#include "scenes/scene_dbg_fps.h"
#include "sdl/sdl_events.h"
#include "sdl/sdl_log.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_storage.h"

#include <algorithm>
#include <stdexcept>
#include <variant>

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

    // Let engine inject text dependencies also.
    sdl::log_debug("Injecting Font dependencies");
    text::use_cache(*m_deps.m_font_cache);

    // TODO: Remove
    m_scene_mgr.push_front(std::make_unique<scenes::dbg_empty>(sdl::fcolor(1, 1, 1, 1)));
    m_scene_mgr.push_front(std::make_unique<scenes::dbg_fps>());
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

    process_events();

    // Call fixed update if and only if frame_accum has passed
    // enough for 1 frame time (1 / FPS), for 60FPS this is about 16ms.
    while (m_frame_data.accumulator >= s_time_step)
    {
        sdl::log_trace("Running a physical tick at {}", tick);
        m_frame_data.accumulator -= s_time_step;

        // TODO:
        // Add signals handling collisions and physical ticking here
        m_scene_mgr.physical_tick();
    }

    m_scene_mgr.tick(dt);

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

void engine::process_events()
{
    // Convert necessary events to signals, or handle high level events?
    std::deque<sdl::event> frame_events;
    frame_events.swap(m_events_queue);

    while (!frame_events.empty())
    {
        auto event = frame_events.front();

        if (auto *_ = std::get_if<sdl::events::quit>(&event.data))
        {
            m_running = false;
            sdl::log_info("Quit event processed. Quitting...");
        }
        else if (auto *mouse_button = std::get_if<sdl::events::mouse_button>(&event.data))
        {
            sdl::log_info("Mouse button at {}, {}, DOWN={}", mouse_button->x, mouse_button->y, mouse_button->down);
        }

        frame_events.pop_front();
    }
}

void engine::queue_event(const sdl::event &&event) noexcept
{
    m_events_queue.emplace_back(std::move(event));
}

bool engine::is_running() const noexcept
{
    return m_running;
}

void engine::render() const noexcept
{
    const sdl::irenderer &renderer = *m_deps.m_renderer;
    renderer.set_blend_mode(sdl::blend_mode::blend);

    renderer.set_color(static_cast<uint8_t>(255), 255, 255, 255);
    renderer.clear();

    m_scene_mgr.render(renderer);

    renderer.present();
}

engine::~engine() noexcept
{
    sdl::log_trace("engine::engine destroyed");
}

} // namespace ccsakura
