#include "engine/collision.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace ccsakura
{

circle_collider::circle_collider(const vec2d center, const double radius) : m_center(center), m_radius(radius)
{
    if (radius <= 0)
    {
        sdl::log_critical("Circle collider can't have a negative radius: {}", radius);
        throw std::invalid_argument("Circle collider can't have a negative value");
    }
    sdl::log_verbose("ccsakura::circle_collider constructed with {} with r = {}", center, radius);
}

collision circle_collider::collides(const collider &other) const noexcept
{
    collision info = other.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision circle_collider::collides_with(const aabb_collider &aabb) const noexcept
{
    collision info = aabb.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision circle_collider::collides_with(const obb_collider &obb) const noexcept
{
    collision info = obb.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision circle_collider::collides_with(const circle_collider &circle) const noexcept
{
    // Two circles collide when the distance between their centers are
    // lower than the sum of their radii.
    // But we calculate it as a vector that points outwards from c1, we take c2 - c1.
    vec2d d = circle.m_center - m_center;
    if (d.length_squared() > std::pow(m_radius + circle.m_radius, 2))
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // If the distance is 0, somehow, then we just push randomly up, then they overlap at center.
    // We just push in an arbitrary direction (up)
    if (double_equal(d.length_squared(), 0))
    {
        return {.is_colliding = true, .depth = m_radius + circle.m_radius, .normal = {0, 1}};
    }

    // Now it's colliding.

    // I think this is a lot simpler, the depth is how much it's collided by
    // (distance - r1 - r2 = -depth), and the normal vector is already
    // calculated.
    double dist = d.length();
    return {.is_colliding = true, .depth = circle.m_radius + m_radius - dist, .normal = d / dist};
}

collision circle_collider::collides_with(const capsule_collider &capsule) const noexcept
{
    // Special case: degenerate capsule, treat like a sphere
    if (capsule.get_p1() == capsule.get_p2())
    {
        vec2d center = (capsule.get_p1() + capsule.get_p2()) / 2;
        circle_collider circle(center, capsule.get_radius());
        return collides_with(circle);
    }

    // Step 1. Find the closest point on the capsule closest to the circle's
    // center.
    vec2d p = capsule.closest_point_to(m_center);

    // Step 2. Calculate the distance between the capsule's segment and the center.
    vec2d d = p - m_center;
    double dist = d.length();
    if (dist > m_radius + capsule.get_radius())
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // Step 3. Calculate collision normal.
    if (double_equal(d.length_squared(), 0))
    {
        // If length is close enough to 0, the center of the circle is on
        // the capsule's segment.
        vec2d normal = capsule.get_p2() - capsule.get_p1();
        return {
            .is_colliding = true, .depth = m_radius + capsule.get_radius(), .normal = normal.orthogonal().normalized()};
    }

    return {.is_colliding = true, .depth = m_radius + capsule.get_radius() - dist, .normal = d / dist};
}

void circle_collider::shift(const vec2d dir) noexcept
{
    m_center += dir;
}

void circle_collider::set_center(const vec2d center) noexcept
{
    m_center = center;
}

vec2d circle_collider::get_center() const noexcept
{
    return m_center;
}

void circle_collider::set_radius(const double radius) noexcept
{
    m_radius = radius;
}

double circle_collider::get_radius() const noexcept
{
    return m_radius;
}

void circle_collider::render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept
{
    renderer.set_color(m_color.r, m_color.g, m_color.b, m_color.a);

    const sdl::fpoint sc = world_to_screen(m_center, camera);
    const double sr = m_radius * camera.zoom;

    sdl::render_geometry_options opts(renderer);
    opts.add_vertex(sdl::vertex(sc, m_color));

    constexpr int segments = 64;
    for (int i = 0; i <= segments; ++i)
    {
        double angle = 2.0 * std::numbers::pi * i / segments;
        sdl::fpoint p(sc.x + static_cast<float>(std::cos(angle) * sr), sc.y + static_cast<float>(std::sin(angle) * sr));
        opts.add_vertex(sdl::vertex(p, m_color));
    }

    for (int i = 1; i <= segments; ++i)
    {
        opts.connect(0, i, i + 1);
    }

    opts.render();
}

circle_collider::~circle_collider()
{
    sdl::log_verbose("ccsakura::circle_collider destroyed");
}

} // namespace ccsakura
