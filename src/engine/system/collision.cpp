#include "engine/system/collision.h"

#include "engine/collision.h"
#include "engine/vec2d.h"
#include "sdl/sdl_log.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <variant>

namespace ccsakura::system
{

// ==================================
// HELPER UTILS
// ==================================

vec2d closest_point_aabb_to(const components::aabb_collider &self, const vec2d p)
{
    ccsakura::vec2d q;
    q.x = std::clamp(p.x, self.center.x - self.half_extents.x, self.center.x + self.half_extents.x);
    q.y = std::clamp(p.y, self.center.y - self.half_extents.y, self.center.y + self.half_extents.y);
    return q;
}

// ==================================
// IMPLEMENTATION PAIRS
// ==================================

collision collides_aabb_aabb(const components::aabb_collider &self, const components::aabb_collider &other)
{
    // 1. Find the distance between two centers.
    vec2d d = other.center - self.center;
    vec2d abs_d(std::abs(d.x), std::abs(d.y));

    // 2. Find the tolerated distance before collision.
    vec2d e = self.half_extents + other.half_extents;

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

collision collides_aabb_circle(const components::aabb_collider &self, const components::circle_collider &other)
{
    collision info = {.is_colliding = false, .depth = 0, .normal = {0, 0}};

    // First, we clamp to find the closest point on the AABB to the circle.
    vec2d closest_p = closest_point_aabb_to(self, other.center);

    // The collision happens when D < R
    vec2d d = other.center - closest_p;
    if (d.length_squared() > other.radius * other.radius)
    {
        return info;
    }

    info.is_colliding = true;

    // Check for normal case. The normal is just the direction to that center already calculated,
    // and the depth we just subtract the length from the radius.
    if (d.length_squared() > 0)
    {
        info.depth = other.radius - d.length();
        info.normal = d.normalized();
        return info;
    }

    // Length is 0 case. Which means the circle center is inside the AABB.
    // We need to find the closest surface of the AABB to throw the circle in that direction.
    // First, we write out all of the unit vectors in all direction.
    std::array<std::pair<double, vec2d>, 4> arr;
    arr[0] = {other.center.x - (self.center.x - self.half_extents.x), vec2d(-1, 0)}; // -x
    arr[1] = {(self.center.x + self.half_extents.x) - other.center.x, vec2d(1, 0)};  // +x
    arr[2] = {other.center.y - (self.center.y - self.half_extents.y), vec2d(0, -1)}; // -y
    arr[3] = {(self.center.y + self.half_extents.y) - other.center.y, vec2d(0, 1)};  // +y

    // Then, we look for the minimum.
    auto min_val = std::min_element(arr.begin(), arr.end(),
                                    [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });
    info.depth = min_val->first + other.radius;
    info.normal = min_val->second;
    return info;
}

collision collides_aabb_capsule(const components::aabb_collider &self, const components::capsule_collider &other)
{
    // Step 1. Find the closest point on the capsule to the AABB center.
    vec2d p = closest_point_on_segment(other.p1, other.p2, self.center);

    // Step 2. Find the closest point on the AABB that is the closest to
    // that representative point.
    vec2d q = closest_point_aabb_to(self, p);

    // Step 3. Collision happens when the distance between p and q <= c2.r
    // P is on the capsule, Q is on the AABB. To direct away from the AABB, we
    // take p - q (end - start).
    vec2d d = p - q;
    if (d.length_squared() > other.radius * other.radius)
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // Step 4. Calculate normal and penetration depth. Two cases.
    // dist = 0, the capsule's point segment is fully inside AABB.
    // dist > 0, the capsule kinda just hits the AABB on the side.
    if (double_equal(d.length_squared(), 0))
    {
        // この場合なら丸型とAABBの衝突と同じ
        components::circle_collider circle(p, other.radius);
        return collides_aabb_circle(self, circle);
    }

    // Normal case.
    return {.is_colliding = true, .depth = other.radius - d.length(), .normal = d.normalized()};
}

collision collides_obb_obb(const components::obb_collider &self, const components::obb_collider &other)
{
    // Step 1. Get some data for the local axes in each OBB. We're using
    // the Separating Axis Theorem. Calculate the local axes for each OBB.
    std::array<vec2d, 4> axes;
    axes[0] = vec2d(std::cos(self.angle), std::sin(self.angle));
    axes[1] = vec2d(-std::sin(self.angle), std::cos(self.angle));
    axes[2] = vec2d(std::cos(other.angle), std::sin(other.angle));
    axes[3] = vec2d(-std::sin(other.angle), std::cos(other.angle));

    // Step 2. Find the vector that crosses the two centers.
    vec2d d = other.center - self.center;

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
        double ca = self.center.dot(l);
        double cb = other.center.dot(l);

        // Project the OBB's radius or extents on the local axis
        double ra = vec2d(axes[0].dot(l), axes[1].dot(l)).abs().dot(self.half_extents);
        double rb = vec2d(axes[2].dot(l), axes[3].dot(l)).abs().dot(other.half_extents);

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

collision collides_obb_circle(const components::obb_collider &self, const components::circle_collider &other)
{
    // Step 1. Transform the circle into local coordinate space
    vec2d d = other.center - self.center;
    vec2d local_center = d.rotated(-self.angle);

    // Step 2. Treat the OBB as an AABB and check collisions.
    // The OBB is IN THE ORIGIN in its own coordinate space.
    components::aabb_collider new_self{{0, 0}, self.half_extents};
    components::circle_collider new_other{local_center, other.radius};

    // Step 3. Translate back.
    collision info = collides_aabb_circle(new_self, new_other);
    if (info.is_colliding)
    {
        info.normal = info.normal.rotated(self.angle);
    }

    return info;
}

collision collides_obb_capsule(const components::obb_collider &self, const components::capsule_collider &other)
{
    // Step 1. Rotate the capsule into OBB's local coordinate space.
    double angle_sin = std::sin(-self.angle);
    double angle_cos = std::cos(-self.angle);
    vec2d local_p1 = (other.p1 - self.center).rotated_sincos(angle_sin, angle_cos);
    vec2d local_p2 = (other.p2 - self.center).rotated_sincos(angle_sin, angle_cos);

    // Step 2. Treat the OBB as an AABB and the new rotated capsule.
    components::aabb_collider new_self{{0, 0}, self.half_extents};
    components::capsule_collider new_other{local_p1, local_p2, other.radius};

    // Step 3. Check collision, and transform back if needed.
    collision info = collides_aabb_capsule(new_self, new_other);
    if (info.is_colliding)
    {
        info.normal = info.normal.rotated(self.angle);
    }

    return info;
}

collision collides_circle_circle(const components::circle_collider &self, const components::circle_collider &other)
{
    // Two circles collide when the distance between their centers are
    // lower than the sum of their radii.
    // But we calculate it as a vector that points outwards from c1, we take c2 - c1.
    vec2d d = other.center - self.center;
    if (d.length_squared() > std::pow(self.radius + other.radius, 2))
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // If the distance is 0, somehow, then we just push randomly up, then they overlap at center.
    // We just push in an arbitrary direction (up)
    if (double_equal(d.length_squared(), 0))
    {
        return {.is_colliding = true, .depth = self.radius + other.radius, .normal = {0, 1}};
    }

    // Now it's colliding.

    // I think this is a lot simpler, the depth is how much it's collided by
    // (distance - r1 - r2 = -depth), and the normal vector is already
    // calculated.
    double dist = d.length();
    return {.is_colliding = true, .depth = other.radius + self.radius - dist, .normal = d / dist};
}

collision collides_circle_capsule(const components::circle_collider &self, const components::capsule_collider &other)
{
    // Special case: degenerate capsule, treat like a sphere
    if (other.p1 == other.p2)
    {
        vec2d center = (other.p1 + other.p2) / 2;
        components::circle_collider circle{center, other.radius};
        return collides_circle_circle(self, circle);
    }

    // Step 1. Find the closest point on the capsule closest to the circle's
    // center.
    vec2d p = closest_point_on_segment(other.p1, other.p2, self.center);

    // Step 2. Calculate the distance between the capsule's segment and the center.
    vec2d d = p - self.center;
    double dist = d.length();
    if (dist > self.radius + other.radius)
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // Step 3. Calculate collision normal.
    if (double_equal(d.length_squared(), 0))
    {
        // If length is close enough to 0, the center of the circle is on
        // the capsule's segment.
        vec2d normal = other.p2 - other.p1;
        return {.is_colliding = true, .depth = self.radius + other.radius, .normal = normal.orthogonal().normalized()};
    }

    return {.is_colliding = true, .depth = self.radius + other.radius - dist, .normal = d / dist};
}

collision collides_capsule_capsule(const components::capsule_collider &self, const components::capsule_collider &other)
{
    if (other.p1 == other.p2)
    {
        components::circle_collider new_other{other.p1, other.radius};
        collision info = collides_circle_capsule(new_other, self);
        info.normal = -info.normal;
        return info;
    }

    // 1. Find a set of points that are the closest between two segments.
    auto [self_best, other_best] = closest_points_between_segments(self.p1, self.p2, other.p1, other.p2);
    vec2d d = other_best - self_best;
    double dist = d.length();

    // 2. If distance is greater, no collision happens.
    if (dist > self.radius + other.radius)
    {
        return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
    }

    // 3. Collision.
    if (double_equal(dist, 0))
    {
        vec2d norm = (other.p2 - other.p1).orthogonal().normalized();
        return {.is_colliding = true, .depth = self.radius + other.radius - dist, .normal = norm};
    }

    return {.is_colliding = true, .depth = self.radius + other.radius - dist, .normal = d / dist};
}

// ==================================
// The main handle stuff
// ==================================

collision check_collisions(const components::collider &self, const components::collider &other)
{
    return std::visit(
        overloaded{[](const components::aabb_collider &self, const components::aabb_collider &other) -> collision
                   { return collides_aabb_aabb(self, other); },
                   [](const components::aabb_collider &self, const components::obb_collider &other) -> collision
                   {
                       components::obb_collider new_self{self.center, self.half_extents, 0};
                       return collides_obb_obb(new_self, other);
                   },
                   [](const components::aabb_collider &self, const components::circle_collider &other) -> collision
                   { return collides_aabb_circle(self, other); },
                   [](const components::aabb_collider &self, const components::capsule_collider &other) -> collision
                   { return collides_aabb_capsule(self, other); },
                   [](const components::obb_collider &self, const components::aabb_collider &other) -> collision
                   {
                       components::obb_collider new_other{other.center, other.half_extents, 0};
                       return collides_obb_obb(self, new_other);
                   },
                   [](const components::obb_collider &self, const components::obb_collider &other) -> collision
                   { return collides_obb_obb(self, other); },
                   [](const components::obb_collider &self, const components::circle_collider &other) -> collision
                   { return collides_obb_circle(self, other); },
                   [](const components::obb_collider &self, const components::capsule_collider &other) -> collision
                   { return collides_obb_capsule(self, other); },
                   [](const components::circle_collider &self, const components::aabb_collider &other) -> collision
                   {
                       collision collision = collides_aabb_circle(other, self);
                       collision.normal = -collision.normal;
                       return collision;
                   },
                   [](const components::circle_collider &self, const components::obb_collider &other) -> collision
                   {
                       collision collision = collides_obb_circle(other, self);
                       collision.normal = -collision.normal;
                       return collision;
                   },
                   [](const components::circle_collider &self, const components::circle_collider &other) -> collision
                   { return collides_circle_circle(self, other); },
                   [](const components::circle_collider &self, const components::capsule_collider &other) -> collision
                   { return collides_circle_capsule(self, other); },
                   [](const components::capsule_collider &self, const components::aabb_collider &other) -> collision
                   {
                       collision collision = collides_aabb_capsule(other, self);
                       collision.normal = -collision.normal;
                       return collision;
                   },
                   [](const components::capsule_collider &self, const components::obb_collider &other) -> collision
                   {
                       collision collision = collides_obb_capsule(other, self);
                       collision.normal = -collision.normal;
                       return collision;
                   },
                   [](const components::capsule_collider &self, const components::circle_collider &other) -> collision
                   {
                       collision collision = collides_circle_capsule(other, self);
                       collision.normal = -collision.normal;
                       return collision;
                   },
                   [](const components::capsule_collider &self, const components::capsule_collider &other) -> collision
                   { return collides_capsule_capsule(self, other); },
                   [](const auto &, const auto &) -> collision
                   {
                       sdl::log_critical("Unimplemented collider pair");
                       return {.is_colliding = false, .depth = 0, .normal = {0, 0}};
                   }},

        self.shape, other.shape);
}

} // namespace ccsakura::system
