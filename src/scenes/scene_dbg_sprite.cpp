#include "scenes/scene_dbg_sprite.h"

#include "engine/collision.h"
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

static constexpr uint32_t ENTITY_BALL = 1;
static constexpr uint32_t ENTITY_KEY = 2;

namespace ccsakura::scenes
{

static vec2d random_key_pos()
{
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<double> dist_x(0.0, APPLICATION_LOGICAL_WIDTH - 32);
    static std::uniform_real_distribution<double> dist_y(0.0, APPLICATION_LOGICAL_HEIGHT - 32);
    return {dist_x(rng), dist_y(rng)};
}

dbg_sprite::dbg_sprite() : m_score_text(typeface::rainy_hearts, 12), m_score(0)
{
    auto ball = std::make_unique<entity>(ENTITY_BALL);
    ball->add_component<components::transform>(vec2d(240, 135));
    ball->add_component<components::sprite>(&sprite::named("dbg_ball"));
    const auto &bf = ball->get_component<components::sprite>().spr->frame(0);
    ball->add_component<components::hitbox>(circle_collider({240, 135}, std::min(bf.source_w, bf.source_h) / 2.0));

    auto key = std::make_unique<entity>(ENTITY_KEY);
    const vec2d key_pos = random_key_pos();
    key->add_component<components::transform>(key_pos);
    key->add_component<components::sprite>(&sprite::named("dbg_key"));
    key->add_component<components::hitbox>(aabb_collider({key_pos, {11, 4.5}}));

    add_entity(std::move(ball));
    add_entity(std::move(key));

    m_score_text.value = "Score: 0";
}

scene_type dbg_sprite::type() const noexcept
{
    return scene_type::dbg_sprite;
}

void dbg_sprite::on_attach(scene_context &ctx) noexcept
{
    bind_intents(ctx, {
                          {sdl::keycode::a, intent::move_left},
                          {sdl::keycode::d, intent::move_right},
                          {sdl::keycode::w, intent::move_up},
                          {sdl::keycode::s, intent::move_down},
                      });
}

void dbg_sprite::on_detach(scene_context &ctx) noexcept
{
    unbind_intents(ctx);
}

bool dbg_sprite::on_physical_tick(scene_context &) noexcept
{
    const double wish_x = static_cast<double>(is_intent_triggered(intent::move_right)) -
                          static_cast<double>(is_intent_triggered(intent::move_left));
    const double wish_y = static_cast<double>(is_intent_triggered(intent::move_down)) -
                          static_cast<double>(is_intent_triggered(intent::move_up));

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

    auto tcomp = get_entity_component<components::transform>(ENTITY_BALL);
    tcomp->position += m_velocity;
    tcomp->position.x = std::clamp(tcomp->position.x, 0.0, static_cast<double>(APPLICATION_LOGICAL_WIDTH));
    tcomp->position.y = std::clamp(tcomp->position.y, 0.0, static_cast<double>(APPLICATION_LOGICAL_HEIGHT));

    auto &ball_collider = get_entity_component<components::hitbox>(ENTITY_BALL)->as<circle_collider>();
    auto &key_collider = get_entity_component<components::hitbox>(ENTITY_KEY)->as<aabb_collider>();

    ball_collider.set_center(tcomp->position);

    auto kcomp = get_entity_component<components::transform>(ENTITY_KEY);
    const auto collision = ball_collider.collides(key_collider);
    if (collision.is_colliding && collision.depth >= 1)
    {
        const vec2d new_pos = random_key_pos();
        kcomp->position = new_pos;
        key_collider.set_center(new_pos);
        m_score++;
        m_score_text.value = std::format("Score: {}", m_score);
    }

    return true;
}

bool dbg_sprite::on_tick(scene_context &, const double dt) noexcept
{
    if (m_velocity.length_squared() > 0)
    {
        auto ball = get_entity_component<components::sprite>(ENTITY_BALL);
        if (ball)
        {
            ball->tick(dt);
        }
    }

    auto key = get_entity_component<components::sprite>(ENTITY_KEY);
    if (key)
    {
        key->tick(dt);
    }
    return true;
}

void dbg_sprite::on_render(const sdl::irenderer &renderer) const noexcept
{
    get_entity_component<components::hitbox>(ENTITY_BALL)->get().render(renderer);
    get_entity_component<components::hitbox>(ENTITY_KEY)->get().render(renderer);

    auto texture = m_score_text.render(renderer);
    texture->set_scale_mode(sdl::scale_mode::nearest);
    sdl::render_texture_options(renderer, *texture).dst({4, 4}).render();

    const auto kcomp = get_entity_component<components::transform>(ENTITY_KEY);
    const auto kscomp = get_entity_component<components::sprite>(ENTITY_KEY);
    if (kscomp && kcomp && kscomp->spr)
    {
        kscomp->spr->render(renderer, kcomp->position.to_fpoint(), kscomp->origin, kscomp->frame_index);
    }

    const auto &tcomp = get_entity_component<components::transform>(ENTITY_BALL);
    const auto &scomp = get_entity_component<components::sprite>(ENTITY_BALL);
    if (tcomp && scomp && scomp->spr)
    {
        scomp->spr->render(renderer, tcomp->position.to_fpoint(), scomp->origin, scomp->frame_index);
    }
}

} // namespace ccsakura::scenes
