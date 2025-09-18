/**
 * \file engine/vec2d.h
 *
 * Provides a 2D vector for math calculations.
 */

#pragma once

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
    double x; ///< the x component
    double y; ///< the y component

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
     * Constructs a vector2d from a copy.
     *
     * \param other the other to copy from
     */
    constexpr vec2d(const vec2d &other) noexcept
    {
        x = other.x;
        y = other.y;
    }

    constexpr vec2d &operator=(const vec2d &other) noexcept
    {
        x = other.x;
        y = other.y;
        return *this;
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
     * Computes the linear interpretation between two vectors.
     *
     * \param rhs the other point to get to
     * \param t the progress time
     * \returns a new interpolated vector
     */
    [[nodiscard]] constexpr vec2d lerp(const vec2d &rhs, const double t) const noexcept
    {
        return vec2d(std::lerp(x, rhs.x, t), std::lerp(y, rhs.y, t));
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
    [[nodiscard]] constexpr vec2d perpendicular() const noexcept
    {
        return rotated(std::numbers::pi / 2);
    }

    /**
     * Creates a simple zero vector.
     *
     * \returns a zero vector.
     */
    static constexpr vec2d zero() noexcept
    {
        return vec2d(0, 0);
    }

    /**
     * Computes a one vector in both directions
     *
     * \returns a one vector
     */
    static constexpr vec2d one() noexcept
    {
        return vec2d(1, 1);
    }

    /**
     * Creates a unit vector in the X direction.
     *
     * \returns a unit x vector
     */
    static constexpr vec2d unit_x() noexcept
    {
        return vec2d(1, 0);
    }

    /**
     * Creates a unit vector in the Y direction.
     *
     * \returns a unit y vector
     */
    static constexpr vec2d unit_y() noexcept
    {
        return vec2d(0, 1);
    }
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
