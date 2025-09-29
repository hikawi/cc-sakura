/**
 * \file engine/vec3d.h
 *
 * Represents a tuple of 3 coordinates in 3D space. The X axis is horizontal (left to right), the Y axis is vertical
 * (down to up), and the Z axis is depth (back to front).
 */

#pragma once

#include <cassert>
#include <format>

namespace ccsakura
{

/**
 * Represents a set of coordinates in 3D space.
 */
struct vec3d
{
    double x; ///< the x component, -x is left and +x is right
    double y; ///< the y component, -y is back and +y is front
    double z; ///< the z component, -z is down and +z is up

    /**
     * Creates an all zero 3D vector.
     */
    constexpr vec3d() noexcept : x(0), y(0), z(0)
    {
    }

    /**
     * Creates a 3D vector with specified components.
     *
     * \param x the x component
     * \param y the y component
     * \param z the z component
     */
    constexpr vec3d(const double x, const double y, const double z) noexcept : x(x), y(y), z(z)
    {
    }

    /**
     * Performs vector addition (A + B).
     *
     * \param other The vector to add.
     * \return A new vec3d object representing the sum.
     */
    constexpr vec3d operator+(const vec3d &other) const noexcept
    {
        return vec3d(x + other.x, y + other.y, z + other.z);
    }

    /**
     * Performs compound assignment addition (A += B).
     *
     * \param other The vector to add.
     * \return A reference to the modified current vec3d object.
     */
    constexpr vec3d &operator+=(const vec3d &other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    /**
     * Performs vector subtraction (A - B).
     *
     * \param other The vector to subtract.
     * \return A new vec3d object representing the difference.
     */
    constexpr vec3d operator-(const vec3d &other) const noexcept
    {
        return vec3d(x - other.x, y - other.y, z - other.z);
    }

    /**
     * Performs compound assignment subtraction (A -= B).
     *
     * \param other The vector to subtract.
     * \return A reference to the modified current vec3d object.
     */
    constexpr vec3d &operator-=(const vec3d &other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    /**
     * Performs vector-scalar multiplication (Vec * scalar).
     *
     * \param scalar The scalar value to multiply by.
     * \return A new vec3d object scaled by the scalar.
     */
    constexpr vec3d operator*(const double scalar) const noexcept
    {
        return vec3d(x * scalar, y * scalar, z * scalar);
    }

    /**
     * Performs compound assignment scalar multiplication (Vec *= scalar).
     *
     * \param scalar The scalar value to multiply by.
     * \return A reference to the modified current vec3d object.
     */
    constexpr vec3d &operator*=(const double scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    /**
     * Performs scalar-vector multiplication (scalar * Vec).
     *
     * This free function allows for multiplication with the scalar on the left side.
     * \param scalar The scalar value to multiply by.
     * \param vec The vector to be multiplied.
     * \return A new vec3d object scaled by the scalar.
     */
    constexpr friend vec3d operator*(const double scalar, const vec3d &vec)
    {
        return vec * scalar;
    }

    /**
     * Performs vector-scalar division (Vec / scalar).
     *
     * \param scalar The scalar value to divide by.
     * \return A new vec3d object resulting from the division.
     * \note Asserts if the scalar is zero to prevent division by zero.
     */
    constexpr vec3d operator/(const double scalar) const noexcept
    {
        assert(scalar != 0);
        return vec3d(x / scalar, y / scalar, z / scalar);
    }

    /**
     * Performs compound assignment vector-scalar division (Vec /= scalar).
     *
     * \param scalar The scalar value to divide by.
     * \return A reference to the modified current vec3d object.
     * \note Asserts if the scalar is zero to prevent division by zero.
     */
    constexpr vec3d &operator/=(const double scalar) noexcept
    {
        assert(scalar != 0);
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    /**
     * Performs scalar-vector division (scalar / Vec.component) element-wise.
     *
     * \param scalar The scalar dividend.
     * \param vec The vector divisor.
     * \return A new vec3d object with components equal to (scalar / x, scalar / y, scalar / z).
     * \note Asserts if any vector component is zero.
     */
    constexpr friend vec3d operator/(const double scalar, const vec3d &vec) noexcept
    {
        assert(vec.x != 0);
        assert(vec.y != 0);
        assert(vec.z != 0);
        return vec3d(scalar / vec.x, scalar / vec.y, scalar / vec.z);
    }

    /**
     * The unary plus operator (+Vec).
     *
     * \return An identical new vec3d object.
     */
    constexpr vec3d operator+() const noexcept
    {
        return *this;
    }

    /**
     * The unary minus operator (-Vec), negates all components.
     *
     * \return A new vec3d object with all components negated.
     */
    constexpr vec3d operator-() const noexcept
    {
        return vec3d(-x, -y, -z);
    }

    /**
     * Computes the dot product (scalar product) with another vector (A . B).
     *
     * \param other The other vector.
     * \return The resulting scalar value of the dot product.
     */
    constexpr double dot(const vec3d &other) const noexcept
    {
        return x * other.x + y * other.y + z * other.z;
    }

    /**
     * Computes the cross product (vector product) with another vector (A x B).
     *
     * \param other The other vector.
     * \return A new vec3d object representing the cross product result.
     */
    constexpr vec3d cross(const vec3d &other) const noexcept
    {
        return vec3d(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    /**
     * Projects the current vector onto a specified base vector.
     *
     * The formula used is: ( (A . B) / |B|^2 ) * B.
     * \param base The vector to project onto.
     * \return A new vec3d object representing the projection.
     */
    constexpr vec3d proj(const vec3d &base) const noexcept
    {
        // Formula is ((A . B) / |B|^2) * B
        return (dot(base) / base.length_squared()) * base;
    }

    /**
     * Calculates the squared magnitude (length squared) of the vector.
     * This avoids a square root operation, which is faster.
     *
     * \return The scalar value of the squared length (x*x + y*y + z*z).
     */
    constexpr double length_squared() const noexcept
    {
        return dot(*this);
    }

    /**
     * Calculates a version of the vector that has all of its components positive.
     *
     * \returns a new vector, with each component passed through abs()
     */
    vec3d abs() const noexcept;

    /**
     * Calculates another orthogonal vector that has a 0 dot product with this vector.
     *
     * \returns an orthogonal vector
     */
    vec3d orthogonal() const noexcept;

    /**
     * Calculates the magnitude (length or L2 norm) of the vector.
     *
     * \return The scalar length of the vector (sqrt(x*x + y*y + z*z)).
     */
    double length() const noexcept;

    /**
     * Returns a new vector that has the same direction but a magnitude of 1.
     *
     * \return The normalized (unit) vector.
     */
    vec3d normalized() const noexcept;

    /**
     * Checks if two vectors are equal by comparing all components.
     *
     * \param other The vector to compare against.
     * \return true if all components are equal, false otherwise.
     */
    bool operator==(const vec3d &other) const noexcept;

    /**
     * Checks if two vectors are not equal.
     *
     * \param other The vector to compare against.
     * \return true if at least one component is different, false otherwise.
     */
    bool operator!=(const vec3d &other) const noexcept;
};

/**
 * Finds the point Q that is the closest to point P on the segment made by A and B.
 *
 * \param a first point of the segment
 * \param b second point of the segment
 * \param p the point to look towards
 * \return the point q
 */
vec3d closest_point_on_segment(const vec3d a, const vec3d b, const vec3d p) noexcept;

/**
 * Finds two points P, Q approximately on two segments made by AB and CD respectively that are the closest to each
 * other.
 *
 * \param a first point of first segment
 * \param b second point of first segment
 * \param c third point of second segment
 * \param d final point of second segment
 * \return a pair of {P, Q} where P is on AB and Q is on CD
 */
std::pair<vec3d, vec3d> closest_points_between_segments(const vec3d a, const vec3d b, const vec3d c, const vec3d d);

} // namespace ccsakura

template <> struct std::formatter<ccsakura::vec3d>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return ctx.begin();
    }

    auto format(const ccsakura::vec3d &vec, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "vec3d({},{},{})", vec.x, vec.y, vec.z);
    }
};
