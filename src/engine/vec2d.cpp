#include "engine/vec2d.h"

#include "utils.h"

#include <algorithm>
#include <cmath>

namespace ccsakura
{

vec2d vec2d::abs() const noexcept
{
    return vec2d(std::abs(x), std::abs(y));
}

bool vec2d::operator==(const vec2d &rhs) const noexcept
{
    return double_equal(x, rhs.x) && double_equal(y, rhs.y);
}

bool vec2d::operator!=(const vec2d &rhs) const noexcept
{
    return !double_equal(x, rhs.x) || !double_equal(y, rhs.y);
}

[[nodiscard]] double vec2d::length() const noexcept
{
    return std::sqrt(length_squared());
}

[[nodiscard]] vec2d vec2d::normalized() const noexcept
{
    const double len = length();
    return len > 0 ? *this / len : *this;
}

[[nodiscard]] double vec2d::distance(const vec2d &rhs) const noexcept
{
    return (rhs - *this).length();
}

vec2d closest_point_on_segment(const vec2d a, const vec2d b, const vec2d p) noexcept
{
    vec2d ab = b - a;
    vec2d ap = p - a;

    double len_sqr = ab.length_squared();

    // Handle case where the segment is length 0
    if (double_equal(len_sqr, 0))
    {
        return a;
    }

    // Project P onto the AB
    // This the "alpha parameter" we multiply to the "to" vector after
    // projection to get the projected vector. But we only care about a single
    // point so
    double t = std::clamp(ap.dot(ab) / len_sqr, 0.0, 1.0);

    // Calculate the closest point on the segment
    return {a.x + t * ab.x, a.y + t * ab.y};
}

std::pair<vec2d, vec2d> closest_points_between_segments(const vec2d p1, const vec2d p2, const vec2d q1,
                                                        const vec2d q2) noexcept
{
    vec2d d1 = p2 - p1; // direction of segment P
    vec2d d2 = q2 - q1; // direction of segment Q
    vec2d r = p1 - q1;  // between origins

    // I have no fucking clue what these a, b, c, e, f are even.
    double a = d1.length_squared(); // always >= 0
    double e = d2.length_squared(); // always >= 0
    double f = d2.dot(r);

    // Handle degenerate cases
    if (double_equal(a, 0) && double_equal(e, 0))
    {
        return {p1, q1}; // both are points
    }
    if (double_equal(a, 0))
    {
        vec2d q = closest_point_on_segment(q1, q2, p1);
        return {p1, q};
    }
    if (double_equal(e, 0))
    {
        vec2d p = closest_point_on_segment(p1, p2, q1);
        return {p, q1};
    }

    // Please don't ask me where I got this.
    // Apparently all those three AIs get these from somewhere, but I don't know where.
    double b = d1.dot(d2);
    double c = d1.dot(r);
    double denom = a * e - b * b;

    double s, t;

    if (double_equal(denom, 0))
    {
        // Segments are parallel
        s = 0.0; // arbitrary
        t = f / e;
        t = std::clamp(t, 0.0, 1.0);
    }
    else
    {
        s = (b * f - c * e) / denom;
        t = (a * f - b * c) / denom;

        s = std::clamp(s, 0.0, 1.0);
        t = std::clamp(t, 0.0, 1.0);
    }

    // Recompute to enforce symmetry: if clamped, recompute the other
    vec2d c1 = p1 + d1 * s;
    vec2d c2 = q1 + d2 * t;

    // If we clamped s, recompute t as projection of c1 onto segment q
    if (s == 0.0 || s == 1.0)
    {
        t = ((c1 - q1).dot(d2)) / e;
        t = std::clamp(t, 0.0, 1.0);
        c2 = q1 + d2 * t;
    }
    // If we clamped t, recompute s as projection of c2 onto segment p
    else if (t == 0.0 || t == 1.0)
    {
        s = ((c2 - p1).dot(d1)) / a;
        s = std::clamp(s, 0.0, 1.0);
        c1 = p1 + d1 * s;
    }

    return {c1, c2};
}

} // namespace ccsakura
