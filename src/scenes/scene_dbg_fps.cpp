#include "scenes/scene_dbg_fps.h"

#include "engine/render.h"
#include "engine/text.h"
#include "sdl/sdl_render.h"

#include <format>

namespace ccsakura::scenes
{

dbg_fps::dbg_fps() : m_fps_text(text(typeface::daydream, 24))
{
    m_fps_text.color = sdl::color(0, 0, 255, 255);
    m_fps_text.value = "0 FPS";
}

scene_type dbg_fps::type() const noexcept
{
    return scene_type::dbg_fps;
}

void dbg_fps::on_tick(const double dt) noexcept
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
}

void dbg_fps::on_render(const sdl::irenderer &renderer) const noexcept
{
    auto texture = m_fps_text.render(renderer);
    sdl::render_texture_options(renderer, *texture)
        .render_origin(render_origin::top_right)
        .dst(sdl::fpoint(1200, 20))
        .render();
}

} // namespace ccsakura::scenes
