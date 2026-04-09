#include "scenes/scene_dbg_fps.h"

#include "engine/text.h"

#include <format>

namespace ccsakura::scenes
{

dbg_fps::dbg_fps() : m_fps_text(text(typeface::rainy_hearts, 16))
{
    m_fps_text.color = sdl::color(0, 0, 255, 255);
    m_fps_text.value = "0 FPS";
}

scene_type dbg_fps::type() const noexcept
{
    return scene_type::dbg_fps;
}

void dbg_fps::on_attach(scene_context &) noexcept
{
}

void dbg_fps::on_detach(scene_context &) noexcept
{
}

bool dbg_fps::on_tick(scene_context &, const double dt) noexcept
{
    m_accumulator += dt;
    m_frame_count++;
    if (m_accumulator >= 1.0)
    {
        m_fps = m_frame_count;
        m_fps_text.value = std::format("{} FPS", m_fps);
        m_frame_count = 0;
        m_accumulator -= 1.0;
    }
    return true;
}

} // namespace ccsakura::scenes
