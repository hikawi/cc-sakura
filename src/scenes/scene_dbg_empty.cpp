#include "scenes/scene_dbg_empty.h"

#include "engine/scene.h"
#include "engine/signal.h"
#include "sdl/sdl_log.h"
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

void dbg_empty::on_attach(scene_context &ctx) noexcept
{
    m_mouse_event_callback = ctx.subscribe(listener_priority::normal, &dbg_empty::on_mouse_event, this);
}

void dbg_empty::on_detach(scene_context &ctx) noexcept
{
    ctx.unsubscribe(m_mouse_event_callback);
}

bool dbg_empty::on_tick(scene_context &, const double) noexcept
{
    return true;
}

void dbg_empty::on_render(const sdl::irenderer &renderer) const noexcept
{
    renderer.set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    renderer.clear();
}

void dbg_empty::on_mouse_event(signals::mouse &e)
{
    if (e.is_cancelled())
    {
        return;
    }
    sdl::log_info("Scene Debug Empty: {}, {}", e.x, e.y);
}

} // namespace ccsakura::scenes
