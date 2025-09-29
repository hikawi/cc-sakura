#include "engine/vec3d.h"

#include "utils.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace ccsakura
{

vec3d vec3d::abs() const noexcept
{
    return vec3d(std::abs(x), std::abs(y), std::abs(z));
}

vec3d vec3d::orthogonal() const noexcept
{
    vec3d ret;
    if (std::abs(x) > std::abs(y))
    {
        ret = {-z, 0, x};
    }
    else
    {
        ret = {0, z, -y};
    }
    return ret.z > 0 ? ret : -ret;
}

double vec3d::length() const noexcept
{
    return std::sqrt(length_squared());
}

bool vec3d::operator==(const vec3d &other) const noexcept
{
    return double_equal(x, other.x) && double_equal(y, other.y) && double_equal(z, other.z);
}

bool vec3d::operator!=(const vec3d &other) const noexcept
{
    return !double_equal(x, other.x) || !double_equal(y, other.y) || !double_equal(z, other.z);
}

vec3d vec3d::normalized() const noexcept
{
    if (double_equal(length_squared(), 0))
    {
        return *this;
    }
    return (*this) / length();
}

vec3d closest_point_on_segment(const vec3d a, const vec3d b, const vec3d p) noexcept
{
    // 1. ベクトル AB を計算
    vec3d ab = b - a;

    // 2. 特殊ケース: 線分の長さがゼロ（A = B の場合）
    // この場合、最近点は点 A (または B)
    if (double_equal(ab.length_squared(), 0))
    {
        return a;
    }

    // 3. パラメータ t を計算: t = (AP ・ AB) / ||AB||^2
    vec3d ap = p - a;
    double t = ap.dot(ab) / ab.length_squared();

    // 4. 最近点 Q を線形補間 (Q = A + t_clipped * AB) で計算して返す
    // Q = A * (1 - t_clipped) + B * t_clipped とも書けますが、この形式が最も直接的です。
    return a + ab * std::clamp(t, 0.0, 1.0);
}

/**
 * Solve the 2-function linear equation and clamp both results to [0, 1] range.
 *
 * at + bs = c,
 * dt + es = f.
 *
 * This keeps calculating until both s and t are clamped and work with the provided parameters as best as possible.
 */
std::pair<double, double> solve_lineq_clamp(const double a, const double b, const double c, const double d,
                                            const double e, const double f)
{
    const double det = a * e - b * d;

    // t = (ce - bf) / det, s = (cd - af) / det
    double s = -1, t = -1;
    if (!double_equal(det, 0))
    {
        t = (c * e - b * f) / det;
        s = (a * f - c * d) / det;
    }
    else
    {
        t = 0;
        s = f / e;
    }

    // 運動クランプ（２パス）
    // Pass 1: Clamp S, then calculate T based on the clamped S
    s = std::clamp(s, 0.0, 1.0);
    t = std::clamp((c - b * s) / a, 0.0, 1.0);

    // Pass 2: Use clamped T to calculate S the final time.
    s = std::clamp((f - d * t) / e, 0.0, 1.0);

    return {t, s};
}

std::pair<vec3d, vec3d> closest_points_between_segments(const vec3d a, const vec3d b, const vec3d c, const vec3d d)
{
    // Define d1 = AB, d2 = CD, r = AC
    vec3d ab = b - a;
    vec3d cd = d - c;
    vec3d ca = a - c;

    // Special cases, if both AB and CD are denegerate: then it's distance between two points
    // If AB or CD is degenrate, it's closest_point_on_segment.
    if (double_equal(ab.length_squared(), 0) && double_equal(cd.length_squared(), 0))
    {
        return {a, c};
    }
    else if (double_equal(ab.length_squared(), 0))
    {
        return {a, closest_point_on_segment(c, d, a)};
    }
    else if (double_equal(cd.length_squared(), 0))
    {
        return {closest_point_on_segment(a, b, c), c};
    }

    // We solve the set of linear equation as followed:
    // t(d1 . d1) - s(d1 . d2) = -(r . d1)
    // t(d1 . d2) - s(d2 . d2) = -(r . d2)
    double val_a = ab.dot(ab);
    double val_b = -ab.dot(cd);
    double val_c = -ca.dot(ab);
    double val_d = ab.dot(cd);
    double val_e = -cd.dot(cd);
    double val_f = -ca.dot(cd);

    auto [t, s] = solve_lineq_clamp(val_a, val_b, val_c, val_d, val_e, val_f);
    return {a + t * ab, c + s * cd};
}

} // namespace ccsakura
