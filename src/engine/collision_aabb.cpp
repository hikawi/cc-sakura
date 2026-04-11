#include "engine/collision.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace ccsakura
{

void collider::set_color(const float r, const float g, const float b, const float a) noexcept
{
    m_color = {r, g, b, a};
}

sdl::fcolor collider::get_color() const noexcept
{
    return m_color;
}

// ========================================

aabb_collider::aabb_collider(const vec2d center, const vec2d extents) : m_center(center), m_extents(extents)
{
    if (extents.x <= 0 || extents.y <= 0)
    {
        sdl::log_critical("Half-extents for an AABB Collider are not positive: {}", extents);
        throw std::invalid_argument("Half-extents for an AABB Collider must be positive");
    }
    sdl::log_verbose("ccsakura::aabb_collider constructed at {} half-extents {}", center, extents);
}

collision aabb_collider::collides(const collider &other) const noexcept
{
    collision info = other.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision aabb_collider::collides_with(const aabb_collider &aabb) const noexcept
{
    // 1. Find the distance between two centers.
    vec2d d = aabb.m_center - m_center;
    vec2d abs_d(std::abs(d.x), std::abs(d.y));

    // 2. Find the tolerated distance before collision.
    vec2d e = m_extents + aabb.m_extents;

    // 3. It only collides if all of the extents are in the range.
    if (abs_d.x > e.x || abs_d.y > e.y)
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    collision info = {.is_colliding = true, .depth = 0, .normal = {0, 0}};
    vec2d s = e - abs_d;

    // 4. The penetration depth is the minimum of s's components.
    info.depth = std::min(s.x, s.y);

    // 5. The normal depends on which of the minimum it was, and which way to flip.
    if (s.x < s.y)
    {
        info.normal = d.x > 0 ? vec2d(1, 0) : vec2d(-1, 0);
    }
    else
    {
        info.normal = d.y > 0 ? vec2d(0, 1) : vec2d(0, -1);
    }

    return info;
}

collision aabb_collider::collides_with(const obb_collider &obb) const noexcept
{
    obb_collider self(m_center, m_extents, 0);
    return self.collides_with(obb);
}

collision aabb_collider::collides_with(const circle_collider &circle) const noexcept
{
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // First, we clamp to find the closest point on the AABB to the circle.
    vec2d closest_p = closest_point_to(circle.get_center());

    // The collision happens when D < R
    vec2d d = circle.get_center() - closest_p;
    if (d.length_squared() > circle.get_radius() * circle.get_radius())
    {
        return info;
    }

    info.is_colliding = true;

    // Check for normal case. The normal is just the direction to that center already calculated,
    // and the depth we just subtract the length from the radius.
    if (d.length_squared() > 0)
    {
        info.depth = circle.get_radius() - d.length();
        info.normal = d.normalized();
        return info;
    }

    // Length is 0 case. Which means the circle center is inside the AABB.
    // We need to find the closest surface of the AABB to throw the circle in that direction.
    // First, we write out all of the unit vectors in all direction.
    std::array<std::tuple<double, vec2d>, 4> arr;
    arr[0] = {circle.get_center().x - (m_center.x - m_extents.x), vec2d(-1, 0)}; // -x
    arr[1] = {(m_center.x + m_extents.x) - circle.get_center().x, vec2d(1, 0)};  // +x
    arr[2] = {circle.get_center().y - (m_center.y - m_extents.y), vec2d(0, -1)}; // -y
    arr[3] = {(m_center.y + m_extents.y) - circle.get_center().y, vec2d(0, 1)};  // +y

    // Then, we look for the minimum.
    auto min_val = std::min_element(arr.begin(), arr.end(),
                                    [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });
    info.depth = std::get<0>(*min_val) + circle.get_radius();
    info.normal = std::get<1>(*min_val);
    return info;
}

collision aabb_collider::collides_with(const capsule_collider &capsule) const noexcept
{
    // Step 1. Find the closest point on the capsule to the AABB center.
    vec2d p = capsule.closest_point_to(m_center);

    // Step 2. Find the closest point on the AABB that is the closest to
    // that representative point.
    vec2d q = closest_point_to(p);

    // Step 3. Collision happens when the distance between p and q <= c2.r
    // P is on the capsule, Q is on the AABB. To direct away from the AABB, we
    // take p - q (end - start).
    vec2d d = p - q;
    if (d.length_squared() > capsule.get_radius() * capsule.get_radius())
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // Step 4. Calculate normal and penetration depth. Two cases.
    // dist = 0, the capsule's point segment is fully inside AABB.
    // dist > 0, the capsule kinda just hits the AABB on the side.
    if (double_equal(d.length_squared(), 0))
    {
        // この場合なら丸型とAABBの衝突と同じ
        circle_collider other(p, capsule.get_radius());
        return this->collides_with(other);
    }

    // Normal case.
    return {.is_colliding = true, .depth = capsule.get_radius() - d.length(), .normal = d.normalized()};
}

void aabb_collider::shift(const vec2d dir) noexcept
{
    m_center += dir;
}

void aabb_collider::set_center(const vec2d center) noexcept
{
    m_center = center;
}

vec2d aabb_collider::get_center() const noexcept
{
    return m_center;
}

void aabb_collider::set_extents(const vec2d extents) noexcept
{
    m_extents = extents;
}

vec2d aabb_collider::get_extents() const noexcept
{
    return m_extents;
}

void aabb_collider::render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept
{
    renderer.set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    const sdl::fpoint sc = world_to_screen(m_center, camera);
    const float sw = static_cast<float>(m_extents.x * 2.0 * camera.zoom);
    const float sh = static_cast<float>(m_extents.y * 2.0 * camera.zoom);
    renderer.render_fill_rect(sdl::frect(sc.x - sw / 2, sc.y - sh / 2, sw, sh));
}

vec2d aabb_collider::closest_point_to(const vec2d p) const noexcept
{
    ccsakura::vec2d q;
    q.x = std::clamp(p.x, m_center.x - m_extents.x, m_center.x + m_extents.x);
    q.y = std::clamp(p.y, m_center.y - m_extents.y, m_center.y + m_extents.y);
    return q;
}

aabb_collider::~aabb_collider()
{
    sdl::log_verbose("ccsakura::aabb_collider destroyed");
}

} // namespace ccsakura
