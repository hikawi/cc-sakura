#include "engine/collision.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace ccsakura
{

capsule_collider::capsule_collider(const vec2d p1, const vec2d p2, const double radius)
    : m_p1(p1), m_p2(p2), m_radius(radius)
{
    if (radius < 0)
    {
        sdl::log_critical("Capsule collider can't be created with negative radius: {}", radius);
        throw std::invalid_argument("Capsule collider can't be created with negative radius");
    }

    sdl::log_verbose("ccsakura::capsule_collider with segment {}-{} with r = {}", p1, p2, radius);
}

collision capsule_collider::collides(const collider &other) const noexcept
{
    collision col = other.collides_with(*this);
    col.normal = -col.normal;
    return col;
}

collision capsule_collider::collides_with(const aabb_collider &aabb) const noexcept
{
    collision info = aabb.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision capsule_collider::collides_with(const obb_collider &obb) const noexcept
{
    collision info = obb.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision capsule_collider::collides_with(const circle_collider &circle) const noexcept
{
    collision info = circle.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision capsule_collider::collides_with(const capsule_collider &capsule) const noexcept
{
    if (capsule.m_p1 == capsule.m_p2)
    {
        circle_collider other(capsule.m_p1, capsule.m_radius);
        return collides_with(other);
    }

    // 1. Find a set of points that are the closest between two segments.
    auto [self_best, other_best] = closest_points_between_segments(m_p1, m_p2, capsule.m_p1, capsule.m_p2);
    vec2d d = other_best - self_best;
    double dist = d.length();

    // 2. If distance is greater, no collision happens.
    if (dist > m_radius + capsule.m_radius)
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // 3. Collision.
    if (double_equal(dist, 0))
    {
        vec2d norm = (capsule.m_p2 - capsule.m_p1).orthogonal().normalized();
        return {.is_colliding = true, .depth = m_radius + capsule.m_radius - dist, .normal = norm};
    }

    return {.is_colliding = true, .depth = m_radius + capsule.m_radius - dist, .normal = d / dist};
}

void capsule_collider::shift(const vec2d dir) noexcept
{
    m_p1 += dir;
    m_p2 += dir;
}

void capsule_collider::set_p1(const vec2d p1) noexcept
{
    m_p1 = p1;
}

vec2d capsule_collider::get_p1() const noexcept
{
    return m_p1;
}

void capsule_collider::set_p2(const vec2d p2) noexcept
{
    m_p2 = p2;
}

vec2d capsule_collider::get_p2() const noexcept
{
    return m_p2;
}

void capsule_collider::set_radius(const double radius) noexcept
{
    m_radius = radius;
}

double capsule_collider::get_radius() const noexcept
{
    return m_radius;
}

void capsule_collider::render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept
{
    renderer.set_color(m_color.r, m_color.g, m_color.b, m_color.a);

    // If it's just a circle, render it as such.
    if (m_p1 == m_p2)
    {
        circle_collider(m_p1, m_radius).render(renderer, camera);
        return;
    }

    vec2d d = m_p2 - m_p1;
    vec2d norm = d.orthogonal().normalized() * m_radius;

    // Rectangle corners (world-space, then transformed to screen)
    const sdl::fpoint sr1 = world_to_screen(m_p1 + norm, camera);
    const sdl::fpoint sr2 = world_to_screen(m_p2 + norm, camera);
    const sdl::fpoint sr3 = world_to_screen(m_p2 - norm, camera);
    const sdl::fpoint sr4 = world_to_screen(m_p1 - norm, camera);

    const sdl::fpoint sp1 = world_to_screen(m_p1, camera);
    const sdl::fpoint sp2 = world_to_screen(m_p2, camera);
    const double sr = m_radius * camera.zoom;

    sdl::render_geometry_options opts(renderer);

    // Add rectangle vertices
    int rect_start = static_cast<int>(opts.vertices.size());
    opts.add_vertex(sdl::vertex(sr1, m_color));
    opts.add_vertex(sdl::vertex(sr2, m_color));
    opts.add_vertex(sdl::vertex(sr3, m_color));
    opts.add_vertex(sdl::vertex(sr4, m_color));

    opts.connect(rect_start + 0, rect_start + 1, rect_start + 2);
    opts.connect(rect_start + 0, rect_start + 2, rect_start + 3);

    // Semi-circles at endpoints
    constexpr int segments = 32;
    double base_angle = std::atan2(norm.y, norm.x);

    // Semi-circle at p1 (around p1, from r4 to r1, away from p2)
    int p1_center_idx = static_cast<int>(opts.vertices.size());
    opts.add_vertex(sdl::vertex(sp1, m_color));
    for (int i = 0; i <= segments; ++i)
    {
        double angle = base_angle + std::numbers::pi * i / segments;
        sdl::fpoint p(sp1.x + static_cast<float>(std::cos(angle) * sr),
                      sp1.y + static_cast<float>(std::sin(angle) * sr));
        opts.add_vertex(sdl::vertex(p, m_color));
    }
    for (int i = 1; i <= segments; ++i)
    {
        opts.connect(p1_center_idx, p1_center_idx + i, p1_center_idx + i + 1);
    }

    // Semi-circle at p2 (around p2, from r2 to r3, away from p1)
    int p2_center_idx = static_cast<int>(opts.vertices.size());
    opts.add_vertex(sdl::vertex(sp2, m_color));
    for (int i = 0; i <= segments; ++i)
    {
        double angle = base_angle + std::numbers::pi + std::numbers::pi * i / segments;
        sdl::fpoint p(sp2.x + static_cast<float>(std::cos(angle) * sr),
                      sp2.y + static_cast<float>(std::sin(angle) * sr));
        opts.add_vertex(sdl::vertex(p, m_color));
    }
    for (int i = 1; i <= segments; ++i)
    {
        opts.connect(p2_center_idx, p2_center_idx + i, p2_center_idx + i + 1);
    }

    opts.render();
}

vec2d capsule_collider::closest_point_to(const vec2d p) const noexcept
{
    return closest_point_on_segment(m_p1, m_p2, p);
}

capsule_collider::~capsule_collider()
{
    sdl::log_verbose("ccsakura::capsule_collider destroyed");
}

} // namespace ccsakura
