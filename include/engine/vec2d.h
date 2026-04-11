/**
 * \file engine/vec2d.h
 *
 * Provides a 2D vector for math calculations.
 */

#pragma once

#include "sdl/sdl_rect.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <numbers>
#include <stdexcept>

namespace ccsakura
{

/**
 * Represents a two-component vector.
 */
struct vec2d
{
    double x; ///< the x component, left -x and right +x
    double y; ///< the y component, down -y and up +y

    /**
     * Constructs a default zero vector.
     */
    constexpr vec2d() noexcept : x(0), y(0)
    {
    }

    /**
     * Constructs a vector with specified components.
     *
     * \param x the x component
     * \param y the y component
     */
    constexpr vec2d(const double x, const double y) noexcept : x(x), y(y)
    {
    }

    /**
     * Computes a vector that is the sum of two vectors.
     *
     * \param rhs the vector to add to this
     * \returns a new result vector
     */
    constexpr vec2d operator+(const vec2d &rhs) const noexcept
    {
        return vec2d(x + rhs.x, y + rhs.y);
    }

    /**
     * Computes a vector that is the difference of two vectors.
     *
     * \param rhs the vector to subtract from this
     * \returns a new result vector
     */
    constexpr vec2d operator-(const vec2d &rhs) const noexcept
    {
        return vec2d(x - rhs.x, y - rhs.y);
    }

    /**
     * Computes a vector that is the scalar multiplication of a vector and a scalar.
     *
     * \param scalar the scalar to multiply with
     * \returns a new scaled vector
     */
    constexpr vec2d operator*(double scalar) const noexcept
    {
        return vec2d(scalar * x, scalar * y);
    }

    /**
     * Computes a vector that is the scalar multiplication of a vector and a scalar.
     *
     * \param scalar the scalar to multiply with
     * \param v the vector to scale
     * \returns a new result vector
     */
    constexpr friend vec2d operator*(double scalar, const vec2d &v) noexcept
    {
        return vec2d(scalar * v.x, scalar * v.y);
    }

    /**
     * Computes a vector that is the inverse scale of a vector.
     *
     * \param scalar the scalar to scale down
     * \returns a new result vector scaled
     */
    constexpr vec2d operator/(double scalar) const
    {
        if (scalar == 0)
        {
            throw std::invalid_argument("divisor can not be zero");
        }
        return vec2d(x / scalar, y / scalar);
    }

    /**
     * Computes the sum of two vectors and updates this.
     *
     * \param rhs the vector to add
     * \returns this reference
     */
    constexpr vec2d &operator+=(const vec2d &rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    /**
     * Computes the difference of two vectors and updates this.
     *
     * \param rhs the vector to subtract
     * \returns this reference
     */
    constexpr vec2d &operator-=(const vec2d &rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    /**
     * Computes the scaled vector and updates this.
     *
     * \param scalar the scaled multiplier
     * \returns this reference
     */
    constexpr vec2d &operator*=(double scalar) noexcept
    {
        *this = scalar * *this;
        return *this;
    }

    /**
     * Computes the inverse scaled vector and updates this.
     *
     * \param scalar the multiplier to scale down
     * \returns this reference
     */
    constexpr vec2d &operator/=(double scalar)
    {
        *this = *this / scalar;
        return *this;
    }

    /**
     * Retrieves the negated version of this vector.
     *
     * \returns a new vector that is negated
     */
    constexpr vec2d operator-() const noexcept
    {
        return vec2d(-x, -y);
    }

    /**
     * Computes the component-wise minimum of two vectors.
     *
     * \param other the other vector to min() with
     * \return the minimum vector
     */
    constexpr vec2d min(const vec2d &other) const noexcept
    {
        return vec2d(std::min(x, other.x), std::min(y, other.y));
    }

    /**
     * Computes the component-wise maximum of two vectors.
     *
     * \param other the other vector to max() with
     * \return the maximum vector
     */
    constexpr vec2d max(const vec2d &other) const noexcept
    {
        return vec2d(std::max(x, other.x), std::max(y, other.y));
    }

    /**
     * Retrieves the component-wise absolute value vector.
     *
     * \return an absolute value vector
     */
    vec2d abs() const noexcept;

    /**
     * Checks if two vectors are equal.
     *
     * \param rhs the vector to check against
     * \returns true if two vectors are meaningfully equal
     */
    bool operator==(const vec2d &rhs) const noexcept;

    /**
     * Checks if two vectors are not equal.
     *
     * \param rhs the vector to check against
     * \returns true if two vectors are not equal
     */
    bool operator!=(const vec2d &rhs) const noexcept;

    /**
     * Computes the length of the vector.
     *
     * \returns the vector length
     */
    [[nodiscard]] double length() const noexcept;

    /**
     * Computes the squared length of the vector.
     *
     * \returns the vector magnitude, squared
     */
    [[nodiscard]] constexpr double length_squared() const noexcept
    {
        return x * x + y * y;
    }

    /**
     * Computes the dot product of two vectors.
     *
     * \param rhs the vector to dot product with
     * \returns the dot product
     */
    [[nodiscard]] constexpr double dot(const vec2d &rhs) const noexcept
    {
        return x * rhs.x + y * rhs.y;
    }

    /**
     * Computes and returns the normalized vector of this vector.
     *
     * This does nothing when the vector magnitude is zero.
     *
     * \returns a new normalized vector
     */
    [[nodiscard]] vec2d normalized() const noexcept;

    /**
     * Calculates the rotation from the X axis to the vector.
     *
     * \return the rotation
     */
    double rotation() const noexcept;

    /**
     * Computes the distance between two points in space.
     *
     * \param rhs the other point
     * \returns the distance between this and the other point
     */
    [[nodiscard]] double distance(const vec2d &rhs) const noexcept;

    /**
     * Computes the squared distance between two points in space.
     *
     * \param rhs the other point
     * \returns the squared distance between this and the other point
     */
    [[nodiscard]] constexpr double distance_squared(const vec2d &rhs) const noexcept
    {
        return (rhs - *this).length_squared();
    }

    /**
     * Computes the rotated vector from this vector at a certain angle.
     *
     * \param theta the angle in radians
     * \returns a new rotated vector
     */
    [[nodiscard]] constexpr vec2d rotated(const double theta) const noexcept
    {
        return rotated_sincos(std::sin(theta), std::cos(theta));
    }

    /**
     * Computes a rotated vector from this vector, knowing the precomputed sin and cos values.
     *
     * \param sin the sine value of the angle
     * \param cos the cosine value of the angle
     * \returns a newly rotated vector
     */
    [[nodiscard]] constexpr vec2d rotated_sincos(const double sin, const double cos) const noexcept
    {
        return vec2d(x * cos - y * sin, x * sin + y * cos);
    }

    /**
     * Rotates a vector counter-clockwise a square angle.
     *
     * \returns a new rotated vector
     */
    [[nodiscard]] constexpr vec2d orthogonal() const noexcept
    {
        return rotated(std::numbers::pi / 2);
    }

    /**
     * Converts the vector to a normal SDL fpoint.
     *
     * \returns an sdl::fpoint of the vec2d
     */
    sdl::fpoint to_fpoint() const noexcept;
};

/**
 * Finds the closest point on the segment AB to the provided point P.
 *
 * \param a the first point of the segment
 * \param b the second point of the segment
 * \param p the point to check against
 * \returns the point on AB closest to P
 */
vec2d closest_point_on_segment(const vec2d a, const vec2d b, const vec2d p) noexcept;

/**
 * Calculates the closest points that belong on two segments made by p1-p2 and q1-q2,
 * so that the points' distance is also the shortest distance between P segment and Q segment.
 *
 * \param p1 first point of first segment
 * \param p2 second point of first segment
 * \param q1 first point of second segment
 * \param q2 second point of second segment
 * \returns a pair of points, where the first is on the segment P and the second is on the segment Q
 */
std::pair<vec2d, vec2d> closest_points_between_segments(const vec2d p1, const vec2d p2, const vec2d q1,
                                                        const vec2d q2) noexcept;

} // namespace ccsakura

template <> struct std::formatter<ccsakura::vec2d>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return ctx.begin();
    }

    auto format(const ccsakura::vec2d &vec, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "vec2d({},{})", vec.x, vec.y);
    }
};
