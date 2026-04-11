#pragma once

#include "engine/collision.h"

namespace ccsakura
{

/**
 * A capsule collider.
 *
 * This is the most complicated type of collider, it should not be overused. This is mainly only used for some main
 * characters.
 */
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
    void render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept override;

    /**
     * Changes the first endpoint of the capsule.
     *
     * \param p1 the new point
     */
    void set_p1(const vec2d p1) noexcept;

    /**
     * Retrieves the first endpoint of the capsule.
     *
     * \returns the point
     */
    vec2d get_p1() const noexcept;

    /**
     * Changes the second endpoint of the capsule.
     *
     * \param p2 the new point
     */
    void set_p2(const vec2d p2) noexcept;

    /**
     * Retrieves the second endpoint of the capsule.
     *
     * \returns the point
     */
    vec2d get_p2() const noexcept;

    /**
     * Changes the radius of the capsule.
     *
     * \param radius the new radius
     */
    void set_radius(const double radius) noexcept;

    /**
     * Retrieves the radius of the capsule.
     *
     * \returns the radius
     */
    double get_radius() const noexcept;

    /**
     * Calculates the closest point on this capsule's segment to the provided point.
     *
     * \param p the point to calculate towards
     * \returns the closest point on the segment to the point p
     */
    vec2d closest_point_to(const vec2d p) const noexcept;

  private:
    vec2d m_p1;
    vec2d m_p2;
    double m_radius;
};

} // namespace ccsakura
