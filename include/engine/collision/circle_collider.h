#pragma once

#include "engine/collision.h"

namespace ccsakura
{

/**
 * A circle collider.
 *
 * This is one of the primitive types of colliders, and should be used for round objects.
 */
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
    void render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept override;

    /**
     * Changes the center of the circle.
     *
     * \param center the new center
     */
    void set_center(const vec2d center) noexcept;

    /**
     * Retrieves the center of the circle.
     *
     * \returns the center
     */
    vec2d get_center() const noexcept;

    /**
     * Changes the radius of the circle.
     *
     * \param radius the new radius
     */
    void set_radius(const double radius) noexcept;

    /**
     * Retrieves the radius of the circle.
     *
     * \returns the radius
     */
    double get_radius() const noexcept;

  private:
    vec2d m_center;
    double m_radius;
};

} // namespace ccsakura
