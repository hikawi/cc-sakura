#include "scenes/scene_dbg_empty.h"

#include "sdl/sdl_render.h"

namespace ccsakura::scenes
{

dbg_empty::dbg_empty(sdl::fcolor color) : m_color(color)
{
}

scene_type dbg_empty::type() const noexcept
{
    return scene_type::dbg_empty;
}

void dbg_empty::on_render(const sdl::irenderer &renderer) const noexcept
{
    renderer.set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    renderer.clear();
}

} // namespace ccsakura::scenes
