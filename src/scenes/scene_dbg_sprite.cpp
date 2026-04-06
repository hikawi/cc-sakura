#include "scenes/scene_dbg_sprite.h"

#include "engine/component.h"
#include "engine/entity.h"
#include "engine/scene.h"
#include "engine/sprite.h"
#include "sdl/sdl_render.h"

#include <algorithm>
#include <numbers>

static constexpr double ACCEL = 0.5;
static constexpr double FRICTION = 0.8;
static constexpr double MAX_SPEED = 5.0;
static constexpr double MAX_SPEED_SQ = MAX_SPEED * MAX_SPEED;
static constexpr double STOP_THRESHOLD_SQ = 0.01 * 0.01;

namespace ccsakura::scenes
{

dbg_sprite::dbg_sprite() : m_keys{false, false, false, false}
{
    m_entity = std::make_unique<entity>(1);
    m_entity->add_component<components::transform>(vec2d(240, 135));
    m_entity->add_component<components::sprite>(&sprite::named("dbg_ball"));
    m_velocity = {0, 0};
}

scene_type dbg_sprite::type() const noexcept
{
    return scene_type::dbg_sprite;
}

void dbg_sprite::on_attach(scene_context &ctx) noexcept
{
    m_key_callback = ctx.subscribe(listener_priority::normal, &dbg_sprite::on_key_event, this);
}

void dbg_sprite::on_detach(scene_context &ctx) noexcept
{
    ctx.unsubscribe(m_key_callback);
}

bool dbg_sprite::on_physical_tick(scene_context &) noexcept
{
    const double wish_x = static_cast<double>(m_keys.right) - static_cast<double>(m_keys.left);
    const double wish_y = static_cast<double>(m_keys.down) - static_cast<double>(m_keys.up);

    if (wish_x != 0 || wish_y != 0)
    {
        vec2d wish_dir{wish_x, wish_y};
        if (wish_x != 0 && wish_y != 0)
        {
            wish_dir *= (1.0 / std::numbers::sqrt2);
        }
        m_velocity += (wish_dir * ACCEL);
    }
    else
    {
        m_velocity *= FRICTION;
    }

    const double speed_sq = m_velocity.length_squared();
    if (speed_sq > MAX_SPEED_SQ)
    {
        m_velocity *= (MAX_SPEED / std::sqrt(speed_sq));
    }

    auto &tcomp = m_entity->get_component<components::transform>();
    tcomp.position += m_velocity;

    // Small optimization: stop tiny vibrations
    if (speed_sq < STOP_THRESHOLD_SQ)
        m_velocity = {0, 0};

    // Coords check (0,0) to (LOGICAL_WIDTH, LOGICAL_HEIGHT)
    tcomp.position.x = std::clamp(tcomp.position.x, 0.0, static_cast<double>(APPLICATION_LOGICAL_WIDTH));
    tcomp.position.y = std::clamp(tcomp.position.y, 0.0, static_cast<double>(APPLICATION_LOGICAL_HEIGHT));

    return true;
}

bool dbg_sprite::on_tick(scene_context &, const double dt) noexcept
{
    auto &scomp = m_entity->get_component<components::sprite>();

    if (m_velocity.length_squared() <= 0 || !scomp.spr)
        return true;

    scomp.frame_elapsed += dt;
    const double frame_dur = scomp.spr->frame(scomp.frame_index).duration / 1000.0;
    if (scomp.frame_elapsed >= frame_dur)
    {
        scomp.frame_elapsed -= frame_dur;

        if (scomp.tag.empty())
        {
            scomp.frame_index = (scomp.frame_index + 1) % scomp.spr->frame_count();
        }
        else
        {
            const auto &t = scomp.spr->frame_tag(scomp.tag);
            scomp.frame_index++;
            if (scomp.frame_index > t.to)
                scomp.frame_index = t.from;
        }
    }

    return true;
}

void dbg_sprite::on_render(const sdl::irenderer &renderer) const noexcept
{
    const auto &tcomp = m_entity->get_component<components::transform>();
    const auto &scomp = m_entity->get_component<components::sprite>();
    if (!scomp.spr)
        return;

    scomp.spr->render(renderer, sdl::fpoint{static_cast<float>(tcomp.position.x), static_cast<float>(tcomp.position.y)},
                      scomp.origin, scomp.frame_index);
}

void dbg_sprite::on_key_event(signals::key &e)
{
    if (e.is_cancelled())
        return;

    switch (e.keycode)
    {
    case sdl::keycode::w:
        m_keys.up = e.down;
        break;
    case sdl::keycode::s:
        m_keys.down = e.down;
        break;
    case sdl::keycode::a:
        m_keys.left = e.down;
        break;
    case sdl::keycode::d:
        m_keys.right = e.down;
        break;
    default:
        // Ignore.
    }
}

} // namespace ccsakura::scenes
