#include "engine/engine.h"

#include "engine/scene.h"
#include "scenes/scene_dbg_colliders.h"
#include "scenes/scene_dbg_empty.h"
#include "scenes/scene_dbg_fps.h"
#include "sdl/sdl_events.h"
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

    // Let engine inject text dependencies also.
    sdl::log_debug("Injecting Font dependencies");
    text::use_cache(*m_deps.m_font_cache);

    // TODO: Remove
    m_scene_mgr.push_front(std::make_unique<scenes::dbg_empty>(sdl::fcolor(1, 1, 1, 1)));
    m_scene_mgr.push_front(std::make_unique<scenes::dbg_colliders>());
    m_scene_mgr.push_front(std::make_unique<scenes::dbg_fps>());
}

frame_data engine::get_frame_data() const
{
    return m_frame_data;
}

// Iterate should be this flow:
// 1. Process SDL events. Wrap all SDL events into signals and queue them to handle AFTER last tick's emitted signals.
// 2. Physical tick if the time step allows it.
// 3. Process all signals, including game signals and SDL's signals.
// 4. Tick animations.
// 5. Render.
bool engine::iterate(const uint64_t tick) noexcept
{
    // Calculate delta time
    const double dt = std::min(s_max_dt, static_cast<double>(tick - m_frame_data.last_tick) / 1000);
    m_frame_data.last_tick = tick;
    m_frame_data.accumulator += dt;

    // Pump all events from the scene manager and SDL's.
    process_events();

    // Process all the requests for this frame.
    m_scene_mgr.process_requests();

    // Process all signals from SDL and scenes.
    process_signals();

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

// Convert all SDL events to signals and queue in the scene manager.
void engine::process_events()
{
    std::deque<sdl::event> frame_events;
    frame_events.swap(m_events_queue);

    while (!frame_events.empty())
    {
        const sdl::event event = std::move(frame_events.front());
        frame_events.pop_front();

        std::unique_ptr<isignal> signal = isignal::wrap(event);
        m_scene_mgr.emit_signal(std::move(signal));
    }
}

void engine::process_signals()
{
    std::deque<std::unique_ptr<isignal>> frame_signals;
    m_scene_mgr.drain_signals(frame_signals);

    // Signals should be passed back to the engine.
    while (!frame_signals.empty())
    {
        std::unique_ptr<isignal> signal = std::move(frame_signals.front());
        frame_signals.pop_front();

        // Let scenes handle signals first. Some scenes might need cleaning up by themselves.
        m_scene_mgr.propagate_signals(*signal);

        // Now the engine handle the signal, as some may be destructive signals.
        // Only quit is implemented for now.
        if (signal->type() == std::type_index(typeid(signals::quit)))
        {
            m_running = false;
        }
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
