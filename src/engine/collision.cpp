#include "engine/colllision.h"
#include "engine/vec2d.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <algorithm>
#include <limits>

namespace ccsakura
{

aabb_collider::aabb_collider(const double x, const double y, const double w, const double h) noexcept
    : m_x(x), m_y(y), m_w(w), m_h(h)
{
    sdl::log_trace("ccsakura::aabb_collider constructed at ({},{}) size {}x{}", m_x, m_y, m_w, m_h);
}

collision aabb_collider::collides(const collider &other) const noexcept
{
    collision info = other.collides_with(*this);
    info.normal = -info.normal;
    return info;
}

collision aabb_collider::collides_with(const aabb_collider &aabb) const noexcept
{
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // Calculate corner points of each collider.
    double min_x1 = this->m_x - this->m_w / 2, max_x1 = this->m_x + this->m_w / 2;
    double min_y1 = this->m_y - this->m_h / 2, max_y1 = this->m_y + this->m_h / 2;
    double min_x2 = aabb.m_x - aabb.m_w / 2, max_x2 = aabb.m_x + aabb.m_w / 2;
    double min_y2 = aabb.m_y - aabb.m_h / 2, max_y2 = aabb.m_y + aabb.m_h / 2;

    // Calculate overlap.
    double overlap_x = std::min(max_x1, max_x2) - std::max(min_x1, min_x2);
    double overlap_y = std::min(max_y1, max_y2) - std::max(min_y1, min_y2);

    // Calculate the area that they overlap.
    // A collision occurs when both overlaps are greater than 0.
    if (overlap_x > 0 && overlap_y > 0)
    {
        info.is_colliding = true;

        // After this, we check for the collision normal, which is the "vector"
        // that we can use to push one collider out with the least force
        // possible.
        //
        // For example, if Sakura jumps to the right and hit the wall, goes in
        // the wall a little, then her collision would be due to slamming her
        // face to the right, the normal would need to push her out of the wall,
        // meaning the wall's normal is to the left.
        //
        // Imagine overlap_x and overlay_y to be the sides of the overlapping
        // rectangle. The shorter side would decide the direction of the normal
        // vector.

        if (overlap_x < overlap_y)
        {
            info.depth = overlap_x;

            // Okay, we know to push c2 left or right, but where? Depends on
            // where c2 is. If c1 is to the left of c2, push right, and vice
            // versa.

            if (this->m_x < aabb.m_x)
            {
                info.normal.x = 1.0;
            }
            else
            {
                info.normal.x = -1.0;
            }

            info.normal.y = 0.0;
        }
        else
        {
            info.depth = overlap_y;

            // Okay, we know to push c2 up or down, but where? Depends on
            // where c2 is. If c1 is to the top of c2, push down, and vice
            // versa.

            if (this->m_y < aabb.m_y)
            {
                info.normal.y = -1.0;
            }
            else
            {
                info.normal.y = 1.0;
            }

            info.normal.x = 0.0;
        }
    }

    return info;
}

collision aabb_collider::collides_with(const obb_collider &obb) const noexcept
{
    obb_collider self(m_x, m_y, m_w, m_h, 0);
    return self.collides_with(obb);
}

collision aabb_collider::collides_with(const circle_collider &circle) const noexcept
{
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // First, we clamp to find the closest point on the AABB to the circle.
    vec2d closest = closest_point_to(circle.m_center);

    // The collision happens when dx^2 + dy^2 < r^2
    vec2d d = circle.m_center - closest;
    if (d.length_squared() >= circle.m_radius * circle.m_radius)
    {
        return info;
    }

    info.is_colliding = true;

    // We separate cases to calculate the normal.
    // First special case, the closest point is the circle center (distance 0).
    if (double_equal(d.length_squared(), 0))
    {
        // For this case, we calculate the distance between the center
        // to each of the AABB sides, the shortest one is the normal.
        double left = circle.m_center.x - (this->m_x - this->m_w / 2);
        double right = (this->m_x + this->m_w / 2) - circle.m_center.x;
        double top = (this->m_y + this->m_h / 2) - circle.m_center.y;
        double bottom = circle.m_center.y - (this->m_y - this->m_h / 2);

        // Find the minimum and its index.
        double vals[4] = {left, right, top, bottom};
        double min = left;
        int idx = 0;

        for (int i = 1; i < 4; i++)
        {
            if (vals[i] < min)
            {
                min = vals[i];
                idx = i;
            }
        }

        // The depth is the minimum + radius, and we find the normal based on
        // which index was chosen. Also like it's stupid head-scratching here,
        // but if an edge is the smallest distance, we push TOWARDS that edge.
        //
        // |-------------|
        // |             |
        // | O           |
        // |             |
        // |-------------|
        //
        // Imagine O is the center of the circle.
        //
        // It's the closest to the left edge, so naturally and intuitively, to
        // "resolve" the collision, we need to push the circle left.
        //
        // How much to push by? That is the depth needed to push it out. From
        // the left edge to the center, is the "min" distance. And the rest of
        // the other half circle is radius. To push it fully out, it is min +
        // radius.
        //
        // In what direction? We just check that with what edge is the closest.

        info.depth = min + circle.m_radius;
        switch (idx)
        {
        case 0:
            info.normal = {-1, 0};
            break;
        case 1:
            info.normal = {1, 0};
            break;
        case 2:
            info.normal = {0, 1};
            break;
        case 3:
            info.normal = {0, -1};
            break;
        default:
            break;
        }
    }
    else
    {
        // Normal case, the normal vector is the vector that points from
        // the circle center to the closest point.
        info.normal = d.normalized();
        info.depth = circle.m_radius - d.length();
    }

    return info;
}

collision aabb_collider::collides_with(const capsule_collider &capsule) const noexcept
{
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // Step 1. Find the closest point on the capsule to the AABB center.
    vec2d p = capsule.closest_point_to({m_x, m_y});

    // Step 2. Find the closest point on the AABB that is the closest to
    // that representative point.
    vec2d q = closest_point_to(p);

    // Step 3. Collision happens when the distance between p and q <= c2.r
    // P is on the capsule, Q is on the AABB. To direct away from the AABB, we
    // take p - q (end - start).
    vec2d d = p - q;
    if (d.length_squared() > capsule.m_radius * capsule.m_radius)
    {
        return info;
    }

    info.is_colliding = true;

    // Step 4. Calculate normal and penetration depth. Two cases.
    // dist = 0, the capsule's point segment is fully inside AABB.
    // dist > 0, the capsule kinda just hits the AABB on the side.
    if (double_equal(d.length_squared(), 0))
    {
        // この場合なら丸型とAABBの衝突と同じ
        circle_collider other(p, capsule.m_radius);
        return this->collides_with(other);
    }
    else
    {
        // Normal case.
        info.normal = d.normalized();
        info.depth = capsule.m_radius - d.length();
    }

    return info;
}

void aabb_collider::shift(const vec2d dir) noexcept
{
    m_x += dir.x;
    m_y += dir.y;
}

vec2d aabb_collider::closest_point_to(const vec2d p) const noexcept
{
    ccsakura::vec2d q;
    q.x = std::clamp(p.x, m_x - m_w / 2, m_x + m_w / 2);
    q.y = std::clamp(p.y, m_y - m_h / 2, m_y + m_h / 2);
    return q;
}

aabb_collider::~aabb_collider()
{
    sdl::log_trace("ccsakura::aabb_collider destroyed");
}

// ==============================================

obb_collider::obb_collider(const double x, const double y, const double w, const double h, const double angle) noexcept
    : m_x(x), m_y(y), m_w(w), m_h(h), m_angle(angle)
{
    sdl::log_trace("ccsakura::obb_collider constructed at ({}, {}) size ({}, {}) angled {}", m_x, m_y, m_w, m_h,
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
    obb_collider other(aabb.m_x, aabb.m_y, aabb.m_w, aabb.m_h, 0);
    return this->collides_with(other);
}

collision obb_collider::collides_with(const obb_collider &obb) const noexcept
{
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // Step 1. Get some data for the local axes in each OBB. We're using
    // the Separating Axis Theorem. Calculate the local axes for each OBB.
    vec2d local_x1(std::cos(m_angle), std::sin(m_angle));
    vec2d local_y1(-std::sin(m_angle), std::cos(m_angle));
    vec2d local_x2(std::cos(obb.m_angle), std::sin(obb.m_angle));
    vec2d local_y2(-std::sin(obb.m_angle), std::cos(obb.m_angle));

    // Step 2. Find the vector that crosses the two centers.
    vec2d center = vec2d(obb.m_x, obb.m_y) - vec2d(m_x, m_y);

    // Step 3. For each local axes, project all of them onto each other.
    vec2d axes[4] = {local_x1, local_y1, local_x2, local_y2};
    double overlap = std::numeric_limits<double>::infinity();
    int idx = 0;

    for (int i = 0; i < 4; i++)
    {
        // Project
        double u1 = std::abs(axes[i].dot(local_x1)) * m_w / 2;
        double v1 = std::abs(axes[i].dot(local_y1)) * m_h / 2;
        double r1 = u1 + v1;

        // Project obb.
        double u2 = std::abs(axes[i].dot(local_x2)) * obb.m_w / 2;
        double v2 = std::abs(axes[i].dot(local_y2)) * obb.m_h / 2;
        double r2 = u2 + v2;

        // Project center.
        double s = std::abs(center.dot(axes[i]));
        if (s > r1 + r2)
        {
            // Separated axis found.
            // No collision.
            return info;
        }

        // Track the axis with the minimum overlap.
        if (r1 + r2 - s < overlap)
        {
            overlap = r1 + r2 - s;
            idx = i;
        }
    }

    // Now we got the collision.
    // The dot product represents their directions relative to each other,
    // a negative product means opposite directions.
    // Our center vector already points us correctly (-> C2)
    if (center.dot(axes[idx]) < 0)
    {
        info.normal = -axes[idx].normalized();
    }
    else
    {
        info.normal = axes[idx].normalized();
    }

    // The depth is the overlap
    info.depth = overlap;
    info.is_colliding = true;
    return info;
}

collision obb_collider::collides_with(const circle_collider &circle) const noexcept
{
    // Step 1. Transform the circle into local coordinate space
    vec2d new_center(circle.m_center.x - m_x, circle.m_center.y - m_y);
    new_center = new_center.rotated(-m_angle);

    // Step 2. Treat the OBB as an AABB and check collisions.
    // The OBB is IN THE ORIGIN in its own coordinate space.
    aabb_collider self(0, 0, m_w, m_h);
    circle_collider new_circ(new_center, circle.m_radius);

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
    // Step 1. Undo the rotation.
    vec2d obb_center(m_x, m_y);
    vec2d p1_tl = (capsule.m_p1 - obb_center).rotated(-m_angle);
    vec2d p2_tl = (capsule.m_p2 - obb_center).rotated(-m_angle);

    // Step 2. Treat the OBB as an AABB.
    aabb_collider self(0, 0, m_w, m_h);
    capsule_collider cap_tl(p1_tl, p2_tl, capsule.m_radius);

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
    m_x += dir.x;
    m_y += dir.y;
}

obb_collider::~obb_collider()
{
    sdl::log_trace("ccsakura::obb_collider destroyed");
}

// ==============================================

circle_collider::circle_collider(const vec2d center, const double radius) noexcept : m_center(center), m_radius(radius)
{
    sdl::log_trace("ccsakura::circle_collider constructed with {} with r = {}", center, radius);
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
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // Two circles collide when the distance between their centers are
    // greater than the sum of their radii.
    // But we calculate it as a vector that points outwards from c1, we take c2 - c1.
    vec2d d = circle.m_center - m_center;
    if (d.length_squared() > std::pow(m_radius + circle.m_radius, 2))
    {
        return info;
    }

    // If the distance is 0, somehow, then we just push randomly up idk.
    if (d.length_squared() == 0)
    {
        info.normal = {0, -1};
        info.depth = m_radius + circle.m_radius;
        return info;
    }

    // Now it's colliding.
    info.is_colliding = true;

    // I think this is a lot simpler, the depth is how much it's collided by
    // (distance - r1 - r2 = -depth), and the normal vector is already
    // calculated.
    double dist = d.length();
    info.depth = -(dist - m_radius - circle.m_radius);
    info.normal = d.normalized();

    return info;
}

collision circle_collider::collides_with(const capsule_collider &capsule) const noexcept
{
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // Special case: degenerate capsule
    if (capsule.m_p1 == capsule.m_p2)
    {
        vec2d center = (capsule.m_p1 + capsule.m_p2) / 2;
        circle_collider circle(center, capsule.m_radius);
        return collides_with(circle);
    }

    // Step 1. Find the closest point on the capsule closest to the circle's
    // center.
    vec2d p = capsule.closest_point_to(m_center);

    // Step 2. Calculate the distance between the capsule's segment and the
    // center.
    vec2d d = p - m_center;
    if (d.length() > m_radius + capsule.m_radius)
    {
        return info;
    }
    info.is_colliding = true;

    // Step 3. Calculate collision normal.
    if (d.length_squared() == 0)
    {
        // If length is close enough to 0, the center of the circle is on
        // the capsule's segment.
        info.normal = (capsule.m_p2 - capsule.m_p1).perpendicular().normalized();
        info.depth = m_radius + capsule.m_radius;
    }
    else
    {
        info.normal = d.normalized();
        info.depth = m_radius + capsule.m_radius - d.length();
    }

    return info;
}

void circle_collider::shift(const vec2d dir) noexcept
{
    m_center += dir;
}

circle_collider::~circle_collider()
{
    sdl::log_trace("ccsakura::circle_collider destroyed");
}

// ==============================================

capsule_collider::capsule_collider(const vec2d p1, const vec2d p2, const double radius) noexcept
    : m_p1(p1), m_p2(p2), m_radius(radius)
{
    sdl::log_trace("ccsakura::capsule_collider with segment {}-{} with r = {}", p1, p2, radius);
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
    auto [self_best, other_best] = closest_points_between_segments(m_p1, m_p2, capsule.m_p1, capsule.m_p2);
    double dist = other_best.distance(self_best);
    collision info;

    if (dist > m_radius + capsule.m_radius)
    {
        return info;
    }

    info.is_colliding = true;
    info.normal = (other_best - self_best).normalized();
    info.depth = m_radius + capsule.m_radius - dist;
    return info;
}

void capsule_collider::shift(const vec2d dir) noexcept
{
    m_p1 += dir;
    m_p2 += dir;
}

vec2d capsule_collider::closest_point_to(const vec2d p) const noexcept
{
    return closest_point_on_segment(m_p1, m_p2, p);
}

capsule_collider::~capsule_collider()
{
    sdl::log_trace("ccsakura::capsule_collider destroyed");
}

} // namespace ccsakura
