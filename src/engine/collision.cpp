#include "engine/collision.h"

#include "engine/vec2d.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
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
    vec2d closest_p = closest_point_to(circle.m_center);

    // The collision happens when D < R
    vec2d d = circle.m_center - closest_p;
    if (d.length_squared() > circle.m_radius * circle.m_radius)
    {
        return info;
    }

    info.is_colliding = true;

    // Check for normal case. The normal is just the direction to that center already calculated,
    // and the depth we just subtract the length from the radius.
    if (d.length_squared() > 0)
    {
        info.depth = circle.m_radius - d.length();
        info.normal = d.normalized();
        return info;
    }

    // Length is 0 case. Which means the circle center is inside the AABB.
    // We need to find the closest surface of the AABB to throw the circle in that direction.
    // First, we write out all of the unit vectors in all direction.
    std::array<std::tuple<double, vec2d>, 4> arr;
    arr[0] = {circle.m_center.x - (m_center.x - m_extents.x), vec2d(-1, 0)}; // -x
    arr[1] = {(m_center.x + m_extents.x) - circle.m_center.x, vec2d(1, 0)};  // +x
    arr[2] = {circle.m_center.y - (m_center.y - m_extents.y), vec2d(0, -1)}; // -y
    arr[3] = {(m_center.y + m_extents.y) - circle.m_center.y, vec2d(0, 1)};  // +y

    // Then, we look for the minimum.
    auto min_val = std::min_element(arr.begin(), arr.end(),
                                    [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });
    info.depth = std::get<0>(*min_val) + circle.m_radius;
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
    if (d.length_squared() > capsule.m_radius * capsule.m_radius)
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // Step 4. Calculate normal and penetration depth. Two cases.
    // dist = 0, the capsule's point segment is fully inside AABB.
    // dist > 0, the capsule kinda just hits the AABB on the side.
    if (double_equal(d.length_squared(), 0))
    {
        // この場合なら丸型とAABBの衝突と同じ
        circle_collider other(p, capsule.m_radius);
        return this->collides_with(other);
    }

    // Normal case.
    return {.is_colliding = true, .depth = capsule.m_radius - d.length(), .normal = d.normalized()};
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

// ==============================================

obb_collider::obb_collider(const vec2d center, const vec2d extents, const double angle)
    : m_center(center), m_extents(extents), m_angle(angle)
{
    if (extents.x <= 0 || extents.y <= 0)
    {
        sdl::log_critical("Half-extents for an OBB Collider are not positive: {}", extents);
        throw std::invalid_argument("Half-extents for an OBB Collider must be positive");
    }

    sdl::log_verbose("ccsakura::obb_collider constructed at {} with extents {} angled {}", m_center, m_extents,
                     m_angle);
}

collision obb_collider::collides(const collider &other) const noexcept
{
    collision info = other.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision obb_collider::collides_with(const aabb_collider &aabb) const noexcept
{
    obb_collider other(aabb.m_center, aabb.m_extents, 0);
    return this->collides_with(other);
}

collision obb_collider::collides_with(const obb_collider &obb) const noexcept
{
    // Step 1. Get some data for the local axes in each OBB. We're using
    // the Separating Axis Theorem. Calculate the local axes for each OBB.
    std::array<vec2d, 4> axes;
    axes[0] = vec2d(std::cos(m_angle), std::sin(m_angle));
    axes[1] = vec2d(-std::sin(m_angle), std::cos(m_angle));
    axes[2] = vec2d(std::cos(obb.m_angle), std::sin(obb.m_angle));
    axes[3] = vec2d(-std::sin(obb.m_angle), std::cos(obb.m_angle));

    // Step 2. Find the vector that crosses the two centers.
    vec2d d = obb.m_center - m_center;

    // Step 3. For each local axes, project all of them onto each other.
    double min_overlap = std::numeric_limits<double>::infinity();
    vec2d min_axis = {0, 0};

    for (size_t i = 0; i < axes.size(); i++)
    {
        vec2d l = axes[i];
        if (double_equal(l.length_squared(), 0))
        {
            continue; // skip on 0-length axes
        }

        // Project centers
        double ca = m_center.dot(l);
        double cb = obb.m_center.dot(l);

        // Project the OBB's radius or extents on the local axis
        double ra = vec2d(axes[0].dot(l), axes[1].dot(l)).abs().dot(m_extents);
        double rb = vec2d(axes[2].dot(l), axes[3].dot(l)).abs().dot(obb.m_extents);

        double dist = std::abs(cb - ca);
        double overlap = ra + rb - dist;

        // 分離軸みっけ
        if (overlap < 0)
        {
            return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
        }

        if (overlap < min_overlap)
        {
            min_overlap = overlap;
            min_axis = l.dot(d) < 0 ? -l : l;
        }
    }

    // The depth is the overlap
    return {.is_colliding = true, .depth = min_overlap, .normal = min_axis.normalized()};
}

collision obb_collider::collides_with(const circle_collider &circle) const noexcept
{
    // Step 1. Transform the circle into local coordinate space
    vec2d d = circle.m_center - m_center;
    vec2d local_center = d.rotated(-m_angle);

    // Step 2. Treat the OBB as an AABB and check collisions.
    // The OBB is IN THE ORIGIN in its own coordinate space.
    aabb_collider self({0, 0}, m_extents);
    circle_collider new_circ(local_center, circle.m_radius);

    // Step 3. Translate back.
    collision info = self.collides_with(new_circ);
    if (info.is_colliding)
    {
        info.normal = info.normal.rotated(m_angle);
    }

    return info;
}

collision obb_collider::collides_with(const capsule_collider &capsule) const noexcept
{
    // Step 1. Rotate the capsule into OBB's local coordinate space.
    double angle_sin = std::sin(-m_angle);
    double angle_cos = std::cos(-m_angle);
    vec2d local_p1 = (capsule.m_p1 - m_center).rotated_sincos(angle_sin, angle_cos);
    vec2d local_p2 = (capsule.m_p2 - m_center).rotated_sincos(angle_sin, angle_cos);

    // Step 2. Treat the OBB as an AABB and the new rotated capsule.
    aabb_collider self({0, 0}, m_extents);
    capsule_collider cap_tl(local_p1, local_p2, capsule.m_radius);

    // Step 3. Check collision, and transform back if needed.
    collision info = self.collides_with(cap_tl);
    if (info.is_colliding)
    {
        info.normal = info.normal.rotated(m_angle);
    }

    return info;
}

void obb_collider::shift(const vec2d dir) noexcept
{
    m_center += dir;
}

void obb_collider::set_center(const vec2d center) noexcept
{
    m_center = center;
}

vec2d obb_collider::get_center() const noexcept
{
    return m_center;
}

void obb_collider::set_extents(const vec2d extents) noexcept
{
    m_extents = extents;
}

vec2d obb_collider::get_extents() const noexcept
{
    return m_extents;
}

void obb_collider::set_angle(const double angle) noexcept
{
    m_angle = angle;
}

double obb_collider::get_angle() const noexcept
{
    return m_angle;
}

void obb_collider::rotate(const double angle) noexcept
{
    m_angle += angle;
}

void obb_collider::render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept
{
    renderer.set_color(m_color.r, m_color.g, m_color.b, m_color.a);

    double cos_a = std::cos(m_angle);
    double sin_a = std::sin(m_angle);
    vec2d ux(cos_a, sin_a);
    vec2d uy(-sin_a, cos_a);

    std::array<vec2d, 4> corners = {
        m_center - ux * m_extents.x - uy * m_extents.y, // bottom-left
        m_center + ux * m_extents.x - uy * m_extents.y, // bottom-right
        m_center + ux * m_extents.x + uy * m_extents.y, // top-right
        m_center - ux * m_extents.x + uy * m_extents.y  // top-left
    };

    sdl::render_geometry_options opts(renderer);
    for (const auto &c : corners)
    {
        opts.add_vertex(sdl::vertex(world_to_screen(c, camera), m_color));
    }

    opts.connect(0, 1, 2);
    opts.connect(0, 2, 3);
    opts.render();
}

obb_collider::~obb_collider()
{
    sdl::log_verbose("ccsakura::obb_collider destroyed");
}

// ==============================================

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
    if (capsule.m_p1 == capsule.m_p2)
    {
        vec2d center = (capsule.m_p1 + capsule.m_p2) / 2;
        circle_collider circle(center, capsule.m_radius);
        return collides_with(circle);
    }

    // Step 1. Find the closest point on the capsule closest to the circle's
    // center.
    vec2d p = capsule.closest_point_to(m_center);

    // Step 2. Calculate the distance between the capsule's segment and the center.
    vec2d d = p - m_center;
    double dist = d.length();
    if (dist > m_radius + capsule.m_radius)
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // Step 3. Calculate collision normal.
    if (double_equal(d.length_squared(), 0))
    {
        // If length is close enough to 0, the center of the circle is on
        // the capsule's segment.
        vec2d normal = capsule.m_p2 - capsule.m_p1;
        return {.is_colliding = true, .depth = m_radius + capsule.m_radius, .normal = normal.orthogonal().normalized()};
    }

    return {.is_colliding = true, .depth = m_radius + capsule.m_radius - dist, .normal = d / dist};
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
        sdl::fpoint p(sc.x + static_cast<float>(std::cos(angle) * sr),
                      sc.y + static_cast<float>(std::sin(angle) * sr));
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

// ==============================================

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
