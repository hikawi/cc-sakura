#include "scenes/scene_dbg_sprite.h"

#include "engine/component.h"
#include "engine/entity.h"
#include "engine/scene.h"
#include "engine/sprite.h"
#include "sdl/sdl_render.h"
#include "sdl/sdl_surface.h"

#include <algorithm>
#include <numbers>
#include <random>

static constexpr double SPEED = 2.0;

namespace ccsakura::scenes
{

static vec2d random_key_pos()
{
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<double> dist_x(0.0, APPLICATION_LOGICAL_WIDTH - 32);
    static std::uniform_real_distribution<double> dist_y(0.0, APPLICATION_LOGICAL_HEIGHT - 32);
    return {dist_x(rng), dist_y(rng)};
}

dbg_sprite::dbg_sprite()
    : m_ball_collider({240, 135}, 1.0), m_key_collider({0, 0}, {11.0, 4.5}), m_score_text(typeface::rainy_hearts, 12),
      m_score(0), m_keys{false, false, false, false}
{
    m_entity = std::make_unique<entity>(1);
    m_entity->add_component<components::transform>(vec2d(240, 135));
    m_entity->add_component<components::sprite>(&sprite::named("dbg_ball"));

    const auto &bf = m_entity->get_component<components::sprite>().spr->frame(0);
    m_ball_collider.set_radius(std::min(bf.source_w, bf.source_h) / 2.0);

    m_velocity = {0, 0};

    m_key_entity = std::make_unique<entity>(2);
    const vec2d key_pos = random_key_pos();
    m_key_entity->add_component<components::transform>(key_pos);
    m_key_entity->add_component<components::sprite>(&sprite::named("dbg_key"));

    m_key_collider.set_center(key_pos);

    m_score_text.value = "Score: 0";
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
        m_velocity = wish_dir * SPEED;
    }
    else
    {
        m_velocity = {0, 0};
    }

    auto &tcomp = m_entity->get_component<components::transform>();
    tcomp.position += m_velocity;

    tcomp.position.x = std::clamp(tcomp.position.x, 0.0, static_cast<double>(APPLICATION_LOGICAL_WIDTH));
    tcomp.position.y = std::clamp(tcomp.position.y, 0.0, static_cast<double>(APPLICATION_LOGICAL_HEIGHT));

    m_ball_collider.set_center(tcomp.position);

    auto &kcomp = m_key_entity->get_component<components::transform>();
    if (m_ball_collider.collides(m_key_collider).is_colliding)
    {
        const vec2d new_pos = random_key_pos();
        kcomp.position = new_pos;
        m_key_collider.set_center(new_pos);
        m_score++;
        m_score_text.value = std::format("Score: {}", m_score);
    }

    return true;
}

bool dbg_sprite::on_tick(scene_context &, const double dt) noexcept
{
    auto &scomp = m_entity->get_component<components::sprite>();
    if (m_velocity.length_squared() > 0)
        scomp.tick(dt);

    m_key_entity->get_component<components::sprite>().tick(dt);

    return true;
}

void dbg_sprite::on_render(const sdl::irenderer &renderer) const noexcept
{
    m_ball_collider.render(renderer);
    m_key_collider.render(renderer);

    auto texture = m_score_text.render(renderer);
    texture->set_scale_mode(sdl::scale_mode::nearest);
    sdl::render_texture_options(renderer, *texture).dst({4, 4}).render();

    const auto &kcomp = m_key_entity->get_component<components::transform>();
    const auto &kscomp = m_key_entity->get_component<components::sprite>();
    if (kscomp.spr)
    {
        kscomp.spr->render(renderer,
                           sdl::fpoint{static_cast<float>(kcomp.position.x), static_cast<float>(kcomp.position.y)},
                           kscomp.origin, kscomp.frame_index);
    }

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
        break;
    }
}

} // namespace ccsakura::scenes
