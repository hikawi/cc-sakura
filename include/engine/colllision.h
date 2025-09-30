/**
 * \file engine/collision.h
 *
 * Provides a framework for handling collisions and making collider checks and how to resolve such collisions with
 * normal vectors.
 */

#pragma once

#include "engine/vec2d.h"
#include "sdl/sdl_pixels.h"

#include <array>

namespace ccsakura
{

/**
 * PODO for a collision.
 */
struct collision
{
    bool is_colliding = false; ///< if the collision is happening
    double depth = 0;          ///< how far are the colliders in each other
    vec2d normal;              ///< the normal vector pointing towards the shortest direction to resolve the collision
};

/**
 * Base interface for a collider.
 *
 * All colliders should inherit from this virtual interface.
 */
class collider;

/**
 * An axis-aligned bounding box collider.
 *
 * This is the simplest collider to use, and should be used in a lot of situations, just because it's much easier to
 * deal with.
 */
class aabb_collider;

/**
 * An oriented bounding box collider.
 *
 * This is the second most complicated capsule to use. Use this for moving or rotating platforms.
 */
class obb_collider;

/**
 * A circle collider.
 *
 * This is one of the primitive types of colliders, and should be used for round objects.
 */
class circle_collider;

/**
 * A capsule collider.
 *
 * This is the most complicated type of collider, it should not be overused. This is mainly only used for some main
 * characters.
 */
class capsule_collider;

class collider
{
  public:
    virtual ~collider() = default;

    /**
     * Marks the collider a color to render with.
     *
     * \param r the red value
     * \param g the green value
     * \param b the blue value
     * \param a the alpha value
     */
    void set_color(const float r, const float g, const float b, const float a) noexcept;

    /**
     * Checks for the collision against another arbitrary collider.
     *
     * \param other the other collider to check against
     * \returns the collision information
     */
    virtual collision collides(const collider &other) const noexcept = 0;

    /**
     * Checks for collisions against an AABB collider.
     *
     * \param aabb the other aabb collider
     * \returns the collision information
     */
    virtual collision collides_with(const aabb_collider &aabb) const noexcept = 0;

    /**
     * Checks for collisions against an OBB collider.
     *
     * \param obb the other obb collider
     * \returns the collision information
     */
    virtual collision collides_with(const obb_collider &obb) const noexcept = 0;

    /**
     * Checks for collisions against a Circle collider.
     *
     * \param circle the other circle collider
     * \returns the collision information
     */
    virtual collision collides_with(const circle_collider &circle) const noexcept = 0;

    /**
     * Checks for collisions against a Capsule collider.
     *
     * \param capsule the capsule collider
     * \returns the collision information
     */
    virtual collision collides_with(const capsule_collider &capsule) const noexcept = 0;

    /**
     * Moves this collider a certain amount in a direction.
     *
     * \param dir the direction to move in
     */
    virtual void shift(const vec2d dir) noexcept = 0;

  protected:
    sdl::fcolor m_color = {0, 0, 0.5f, 0.3f}; ///< the color to render the collider with
};

class aabb_collider : public collider
{
  public:
    /**
     * Constructs an AABB collider.
     *
     * \param center the center point of the AABB collider
     * \param extents the half-widths of the axes of the collider
     */
    aabb_collider(const vec2d center, const vec2d extents);
    ~aabb_collider();

    collision collides(const collider &other) const noexcept override;
    collision collides_with(const aabb_collider &aabb) const noexcept override;
    collision collides_with(const obb_collider &obb) const noexcept override;
    collision collides_with(const circle_collider &circle) const noexcept override;
    collision collides_with(const capsule_collider &capsule) const noexcept override;
    void shift(const vec2d dir) noexcept override;

    /**
     * Retrieves the closest point on the AABB that is the closest to the provided point P.
     *
     * \param p the pointer to check against
     * \returns the point on the aabb closest to p
     */
    vec2d closest_point_to(const vec2d p) const noexcept;

    friend class obb_collider;
    friend class circle_collider;
    friend class capsule_collider;

  private:
    vec2d m_center;
    vec2d m_extents;
};

class obb_collider : public collider
{
  public:
    /**
     * Constructs an OBB collider.
     *
     * \param center the center point of the OBB collider
     * \param extents the half extents of the OBB
     * \param angle the angle of the collider along with the x axis
     */
    obb_collider(const vec2d center, const vec2d extents, double angle);
    ~obb_collider();

    collision collides(const collider &other) const noexcept override;
    collision collides_with(const aabb_collider &aabb) const noexcept override;
    collision collides_with(const obb_collider &obb) const noexcept override;
    collision collides_with(const circle_collider &circle) const noexcept override;
    collision collides_with(const capsule_collider &capsule) const noexcept override;
    void shift(const vec2d dir) noexcept override;

    friend class aabb_collider;
    friend class circle_collider;
    friend class capsule_collider;

  private:
    vec2d m_center;
    vec2d m_extents;
    double m_angle;
};

class circle_collider : public collider
{
  public:
    /**
     * Constructs a circle collider.
     *
     * \param center the center point of the collider
     * \param radius the radius of the circle
     */
    circle_collider(const vec2d center, const double radius);
    ~circle_collider();

    collision collides(const collider &other) const noexcept override;
    collision collides_with(const aabb_collider &aabb) const noexcept override;
    collision collides_with(const obb_collider &obb) const noexcept override;
    collision collides_with(const circle_collider &circle) const noexcept override;
    collision collides_with(const capsule_collider &capsule) const noexcept override;
    void shift(const vec2d dir) noexcept override;

    friend class aabb_collider;
    friend class obb_collider;
    friend class capsule_collider;

  private:
    vec2d m_center;
    double m_radius;
};

class capsule_collider : public collider
{
  public:
    /**
     * Constructs a Capsule collider.
     *
     * \param p1 an endpoint of the collider shaft
     * \param p2 another endpoint of the collider shaft
     * \param radius the radius of the collider
     */
    capsule_collider(const vec2d p1, const vec2d p2, const double radius);
    ~capsule_collider();

    collision collides(const collider &other) const noexcept override;
    collision collides_with(const aabb_collider &aabb) const noexcept override;
    collision collides_with(const obb_collider &obb) const noexcept override;
    collision collides_with(const circle_collider &circle) const noexcept override;
    collision collides_with(const capsule_collider &capsule) const noexcept override;
    void shift(const vec2d dir) noexcept override;

    /**
     * Calculates the closest point on this capsule's segment to the provided point.
     *
     * \param p the point to calculate towards
     * \returns the closest point on the segment to the point p
     */
    vec2d closest_point_to(const vec2d p) const noexcept;

    friend class aabb_collider;
    friend class obb_collider;
    friend class circle_collider;

  private:
    vec2d m_p1;
    vec2d m_p2;
    double m_radius;
};

} // namespace ccsakura
