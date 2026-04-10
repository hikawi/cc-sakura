#include "scenes/scene_dbg_physics.h"

#include "engine/collision.h"
#include "engine/component.h"
#include "engine/entity.h"
#include "engine/scene.h"
#include "engine/sprite.h"

#include <algorithm>
#include <format>

static constexpr double SPEED_X = 2.0;
static constexpr double JUMP_VELOCITY = -5.0;
static constexpr double GRAVITY = 0.2;
static constexpr double MAX_FALL_SPEED = 8.0;

static constexpr uint32_t ENTITY_PLAYER = 1;
static constexpr uint32_t ENTITY_FLOOR = 2;
static constexpr uint32_t ENTITY_BOX = 3;
static constexpr uint32_t ENTITY_INFO = 4;

namespace ccsakura::scenes
{

dbg_physics::dbg_physics()
{
    // Player
    auto *player = construct_entity(ENTITY_PLAYER)
                       .with_component<components::transform>(vec2d(240, 0))
                       .with_component<components::sprite>(&sprite::named("dbg_ball"))
                       .build();
    const auto &bf = player->get_component<components::sprite>()->spr->frame(0);
    player->add_component<components::hitbox>(circle_collider({240, 0}, std::min(bf.source_w, bf.source_h) / 2.0));

    // Floor
    const vec2d floor_pos{240, 240};
    const vec2d floor_extents{200, 20};
    auto *floor = construct_entity(ENTITY_FLOOR)
                      .with_component<components::transform>(floor_pos)
                      .with_component<components::hitbox>(aabb_collider(floor_pos, floor_extents))
                      .build();
    floor->get_component<components::hitbox>()->get().set_color(0.0f, 1.0f, 0.0f, 0.5f);

    // Box on top of the floor
    const vec2d box_pos{240, 200};
    const vec2d box_extents{30, 20};
    auto *box = construct_entity(ENTITY_BOX)
                    .with_component<components::transform>(box_pos)
                    .with_component<components::hitbox>(aabb_collider(box_pos, box_extents))
                    .build();
    box->get_component<components::hitbox>()->get().set_color(1.0f, 0.5f, 0.0f, 0.5f);

    auto *info = construct_entity(ENTITY_INFO)
                     .with_component<components::transform>(vec2d(4, 4), 0.0, true)
                     .with_component<components::text>()
                     .build();
    info->get_component<components::text>()->set("Use A/D to move, W to jump");

    ON_COLLISION(ENTITY_PLAYER, ENTITY_FLOOR, { resolve_player(a, b, col); });
    ON_COLLISION(ENTITY_PLAYER, ENTITY_BOX, { resolve_player(a, b, col); });
}

scene_type dbg_physics::type() const noexcept
{
    return scene_type::dbg_physics;
}

void dbg_physics::on_attach(scene_context &ctx) noexcept
{
    bind_intents(ctx)
        .map(sdl::keycode::a, intent::move_left)
        .map(sdl::keycode::d, intent::move_right)
        .map(sdl::keycode::space, intent::move_up)
        .bind();
}

void dbg_physics::on_detach(scene_context &ctx) noexcept
{
    unbind_intents(ctx);
}

bool dbg_physics::on_physical_tick(scene_context &ctx) noexcept
{
    m_ticks++;

    // 1. Box movement
    with_entity_components<components::transform, components::hitbox>(ENTITY_BOX,
                                                                      [&](auto &bcomp, auto &hbox)
                                                                      {
                                                                          // Simple back and forth movement
                                                                          if (m_ticks % 100 == 0)
                                                                              m_box_dir *= -1.0;
                                                                          bcomp.position.x += m_box_dir * 1.5;
                                                                          hbox.template as<aabb_collider>().set_center(bcomp.position);
                                                                      });

    // 2. Player movement
    const double wish_x = static_cast<double>(is_intent_triggered(intent::move_right)) -
                          static_cast<double>(is_intent_triggered(intent::move_left));
    m_velocity.x = wish_x * SPEED_X;

    if (m_grounded && is_intent_triggered(intent::move_up))
    {
        m_velocity.y = JUMP_VELOCITY;
        m_grounded = false;
    }

    m_velocity.y = std::min(MAX_FALL_SPEED, m_velocity.y + GRAVITY);

    with_entity_components<components::transform, components::hitbox>(ENTITY_PLAYER,
                                                                      [&](auto &tcomp, auto &hbox)
                                                                      {
                                                                          tcomp.position += m_velocity;
                                                                          hbox.template as<circle_collider>().set_center(tcomp.position);
                                                                          ctx.camera().position = tcomp.position;
                                                                      });

    m_grounded = false;
    return true;
}

bool dbg_physics::on_tick(scene_context &, const double dt) noexcept
{
    if (m_velocity.x * m_velocity.x > 0)
    {
        auto player_sprite = get_entity_component<components::sprite>(ENTITY_PLAYER);
        if (player_sprite)
        {
            player_sprite->tick(dt);
            player_sprite->reverse = m_velocity.x < 0;
        }
    }

    get_entity_component<components::text>(ENTITY_INFO)
        ->set(std::format("Velocity: {:.2f}, {:.2f} | Grounded: {}", m_velocity.x, m_velocity.y, m_grounded));

    return true;
}

void dbg_physics::resolve_player(uint32_t a, uint32_t /*b*/, const collision &col) noexcept
{
    const bool a_is_player = (a == ENTITY_PLAYER);
    const vec2d normal = a_is_player ? col.normal : -col.normal;

    with_entity_components<components::transform, components::hitbox>(ENTITY_PLAYER,
                                                                      [&](auto &tcomp, auto &hitbox)
                                                                      {
                                                                          tcomp.position += -normal * col.depth;
                                                                          hitbox.template as<circle_collider>().set_center(tcomp.position);

                                                                          if (normal.y > 0)
                                                                          {
                                                                              m_grounded = true;
                                                                              m_velocity.y = 0;
                                                                          }
                                                                          else if (normal.y < 0)
                                                                          {
                                                                              m_velocity.y = 0;
                                                                          }
                                                                      });
}

} // namespace ccsakura::scenes
