#include "engine/collision.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace ccsakura
{

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
    obb_collider other(aabb.get_center(), aabb.get_extents(), 0);
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
    vec2d d = circle.get_center() - m_center;
    vec2d local_center = d.rotated(-m_angle);

    // Step 2. Treat the OBB as an AABB and check collisions.
    // The OBB is IN THE ORIGIN in its own coordinate space.
    aabb_collider self({0, 0}, m_extents);
    circle_collider new_circ(local_center, circle.get_radius());

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
    vec2d local_p1 = (capsule.get_p1() - m_center).rotated_sincos(angle_sin, angle_cos);
    vec2d local_p2 = (capsule.get_p2() - m_center).rotated_sincos(angle_sin, angle_cos);

    // Step 2. Treat the OBB as an AABB and the new rotated capsule.
    aabb_collider self({0, 0}, m_extents);
    capsule_collider cap_tl(local_p1, local_p2, capsule.get_radius());

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

} // namespace ccsakura
