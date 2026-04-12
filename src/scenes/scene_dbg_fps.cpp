#include "scenes/scene_dbg_fps.h"

#include "engine/component/text.h"
#include "engine/component/transform.h"

#include <format>

namespace ccsakura::scenes
{

dbg_fps::dbg_fps()
{
    auto *fps = construct_entity(ENTITY_FPS)
                    .with_component<components::transform>(vec2d(476, 4), 0.0, true)
                    .with_component<components::text>()
                    .build();
    fps->get_component<components::text>()->origin = render_origin::top_right;
    fps->get_component<components::text>()->set("0 FPS");
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
        get_entity_component<components::text>(ENTITY_FPS)->set(std::format("{} FPS", m_fps));
        m_frame_count = 0;
        m_accumulator -= 1.0;
    }
    return true;
}

} // namespace ccsakura::scenes
