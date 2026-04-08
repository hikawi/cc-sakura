#include "scenes/scene_dbg_physics.h"

#include "engine/collision.h"
#include "engine/component.h"
#include "engine/entity.h"
#include "engine/scene.h"
#include "engine/sprite.h"
#include "sdl/sdl_render.h"

#include <algorithm>
#include <format>

static constexpr double SPEED_X = 2.0;
static constexpr double JUMP_VELOCITY = -5.0;
static constexpr double GRAVITY = 0.2;
static constexpr double MAX_FALL_SPEED = 8.0;

static constexpr uint32_t ENTITY_PLAYER = 1;
static constexpr uint32_t ENTITY_FLOOR = 2;
static constexpr uint32_t ENTITY_BOX = 3;

namespace ccsakura::scenes
{

dbg_physics::dbg_physics() : m_info_text(typeface::rainy_hearts, 16)
{
    // Player
    auto *player = construct_entity(ENTITY_PLAYER)
                       .with_component<components::transform>(vec2d(240, 0))
                       .with_component<components::sprite>(&sprite::named("dbg_ball"))
                       .build();
    const auto &bf = player->get_component<components::sprite>().spr->frame(0);
    player->add_component<components::hitbox>(circle_collider({240, 0}, std::min(bf.source_w, bf.source_h) / 2.0));

    // Floor
    const vec2d floor_pos{240, 240};
    const vec2d floor_extents{200, 20};
    auto *floor = construct_entity(ENTITY_FLOOR)
                      .with_component<components::transform>(floor_pos)
                      .with_component<components::hitbox>(aabb_collider(floor_pos, floor_extents))
                      .build();
    floor->get_component<components::hitbox>().get().set_color(0.0f, 1.0f, 0.0f, 0.5f);

    // Box on top of the floor
    const vec2d box_pos{240, 200};
    const vec2d box_extents{30, 20};
    auto *box = construct_entity(ENTITY_BOX)
                    .with_component<components::transform>(box_pos)
                    .with_component<components::hitbox>(aabb_collider(box_pos, box_extents))
                    .build();
    box->get_component<components::hitbox>().get().set_color(1.0f, 0.5f, 0.0f, 0.5f);

    m_info_text.value = "Use A/D to move, W to jump";
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
        .map(sdl::keycode::w, intent::move_up) // Jump
        .bind();
}

void dbg_physics::on_detach(scene_context &ctx) noexcept
{
    unbind_intents(ctx);
}

bool dbg_physics::on_physical_tick(scene_context &) noexcept
{
    m_ticks++;

    // 1. Box movement
    auto bcomp = get_entity_component<components::transform>(ENTITY_BOX);
    auto &box_collider = get_entity_component<components::hitbox>(ENTITY_BOX)->as<aabb_collider>();

    // Simple back and forth movement
    if (m_ticks % 100 == 0)
        m_box_dir *= -1.0;
    bcomp->position.x += m_box_dir * 1.5;
    box_collider.set_center(bcomp->position);

    // 2. Player movement
    const double wish_x = static_cast<double>(is_intent_triggered(intent::move_right)) -
                          static_cast<double>(is_intent_triggered(intent::move_left));
    m_velocity.x = wish_x * SPEED_X;

    if (m_grounded && is_intent_triggered(intent::move_up))
    {
        m_velocity.y = JUMP_VELOCITY;
        m_grounded = false;
    }

    m_velocity.y += GRAVITY;
    if (m_velocity.y > MAX_FALL_SPEED)
        m_velocity.y = MAX_FALL_SPEED;

    auto tcomp = get_entity_component<components::transform>(ENTITY_PLAYER);
    tcomp->position += m_velocity;

    auto &player_collider = get_entity_component<components::hitbox>(ENTITY_PLAYER)->as<circle_collider>();
    player_collider.set_center(tcomp->position);

    // 3. Collision checks
    m_grounded = false;

    // Helper to resolve collision
    auto resolve = [&](const collider &other)
    {
        const auto collision = player_collider.collides(other);
        if (collision.is_colliding)
        {
            tcomp->position -= collision.normal * collision.depth;
            player_collider.set_center(tcomp->position);

            if (collision.normal.y > 0)
            {
                m_grounded = true;
                m_velocity.y = 0;
            }
            else if (collision.normal.y < 0)
            {
                m_velocity.y = 0;
            }
        }
    };

    auto &floor_collider = get_entity_component<components::hitbox>(ENTITY_FLOOR)->as<aabb_collider>();
    resolve(floor_collider);
    resolve(box_collider);

    m_info_text.value = std::format("Velocity: {:.2f}, {:.2f} | Grounded: {}", m_velocity.x, m_velocity.y, m_grounded);

    return true;
}

bool dbg_physics::on_tick(scene_context &, const double dt) noexcept
{
    if (m_velocity.length_squared() > 0)
    {
        auto player_sprite = get_entity_component<components::sprite>(ENTITY_PLAYER);
        if (player_sprite)
        {
            player_sprite->tick(dt);
        }
    }
    return true;
}

void dbg_physics::on_render(const sdl::irenderer &renderer) const noexcept
{
    // Render text
    auto texture = m_info_text.render(renderer);
    texture->set_scale_mode(sdl::scale_mode::nearest);
    sdl::render_texture_options(renderer, *texture).dst({4, 4}).render();

    // Render floor collider
    get_entity_component<components::hitbox>(ENTITY_FLOOR)->get().render(renderer);

    // Render box collider
    get_entity_component<components::hitbox>(ENTITY_BOX)->get().render(renderer);

    // Render player sprite and collider
    const auto &tcomp = get_entity_component<components::transform>(ENTITY_PLAYER);
    const auto &scomp = get_entity_component<components::sprite>(ENTITY_PLAYER);
    if (tcomp && scomp && scomp->spr)
    {
        scomp->spr->render(renderer, tcomp->position.to_fpoint(), scomp->origin, scomp->frame_index);
    }
    get_entity_component<components::hitbox>(ENTITY_PLAYER)->get().render(renderer);
}

} // namespace ccsakura::scenes
