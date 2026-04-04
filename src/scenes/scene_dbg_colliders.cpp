#include "scenes/scene_dbg_colliders.h"

#include "engine/collision.h"
#include "engine/scene.h"
#include "sdl/sdl_render.h"

#include <cmath>
#include <numbers>

namespace ccsakura::scenes
{

dbg_colliders::dbg_colliders()
{
    // AABB at top-left
    auto aabb = std::make_unique<aabb_collider>(vec2d(100, 70), vec2d(40, 25));
    aabb->set_color(1.0f, 0.0f, 0.0f, 0.5f);
    m_aabb = aabb.get();
    m_colliders.push_back(std::move(aabb));

    // OBB at top-right
    auto obb = std::make_unique<obb_collider>(vec2d(350, 70), vec2d(40, 25), std::numbers::pi / 6.0);
    obb->set_color(0.0f, 1.0f, 0.0f, 0.5f);
    m_obb = obb.get();
    m_colliders.push_back(std::move(obb));

    // Circle at bottom-left
    auto circle = std::make_unique<circle_collider>(vec2d(100, 200), 40.0);
    circle->set_color(0.0f, 0.0f, 1.0f, 0.5f);
    m_circle = circle.get();
    m_colliders.push_back(std::move(circle));

    // Capsule at bottom-right
    auto capsule = std::make_unique<capsule_collider>(vec2d(300, 180), vec2d(400, 220), 25.0);
    capsule->set_color(1.0f, 1.0f, 0.0f, 0.5f);
    m_capsule = capsule.get();
    m_colliders.push_back(std::move(capsule));
}

scene_type dbg_colliders::type() const noexcept
{
    return scene_type::dbg_colliders;
}

void dbg_colliders::on_attach(scene_context &) noexcept
{
}

void dbg_colliders::on_detach(scene_context &) noexcept
{
}

bool dbg_colliders::on_tick(scene_context &, const double dt) noexcept
{
    m_time += dt;

    // AABB: Move and Scale
    vec2d aabb_center(100 + std::cos(m_time) * 20.0, 70 + std::sin(m_time * 0.5) * 10.0);
    vec2d aabb_extents(40 + std::sin(m_time * 2.0) * 10.0, 25 + std::cos(m_time * 2.0) * 5.0);
    m_aabb->set_center(aabb_center);
    m_aabb->set_extents(aabb_extents);

    // OBB: Move and Scale
    vec2d obb_center(350 + std::sin(m_time) * 20.0, 70 + std::cos(m_time * 0.5) * 10.0);
    vec2d obb_extents(40 + std::cos(m_time * 2.0) * 10.0, 25 + std::sin(m_time * 2.0) * 5.0);
    m_obb->set_center(obb_center);
    m_obb->set_extents(obb_extents);
    m_obb->rotate(dt);

    // Circle: Move and Scale
    vec2d circle_center(100 + std::sin(m_time * 0.7) * 15.0, 200 + std::cos(m_time * 0.7) * 15.0);
    double circle_radius = 40.0 + std::sin(m_time * 1.5) * 10.0;
    m_circle->set_center(circle_center);
    m_circle->set_radius(circle_radius);

    // Capsule: Move and Rotate
    vec2d cap_center(350 + std::cos(m_time * 0.3) * 20.0, 200 + std::sin(m_time * 0.3) * 10.0);
    vec2d cap_dir = vec2d(std::cos(m_time), std::sin(m_time)) * 40.0;
    m_capsule->set_p1(cap_center - cap_dir);
    m_capsule->set_p2(cap_center + cap_dir);

    return true;
}

void dbg_colliders::on_render(const sdl::irenderer &renderer) const noexcept
{
    for (const auto &c : m_colliders)
    {
        c->render(renderer);
    }
}

} // namespace ccsakura::scenes
