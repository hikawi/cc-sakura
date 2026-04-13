#include "scenes/scene_dbg_empty.h"

#include "engine/scene.h"
#include "engine/signal.h"
#include "sdl/sdl_log.h"

namespace ccsakura::scenes
{

dbg_empty::dbg_empty(sdl::fcolor color)
{
    m_background_color = color;
}

scene_type dbg_empty::type() const noexcept
{
    return scene_type::debug;
}

void dbg_empty::on_attach(scene_context &ctx) noexcept
{
    bind_signals(ctx).on<signals::mouse>(listener_priority::normal, &dbg_empty::on_mouse_event, this).bind();
}

void dbg_empty::on_detach(scene_context &ctx) noexcept
{
    unbind_signals(ctx);
}

bool dbg_empty::on_tick(scene_context &, const double) noexcept
{
    return true;
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
