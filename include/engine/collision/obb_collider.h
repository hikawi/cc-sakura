#pragma once

#include "engine/collision.h"

namespace ccsakura
{

/**
 * An oriented bounding box collider.
 *
 * This is the second most complicated capsule to use. Use this for moving or rotating platforms.
 */
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
    void render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept override;

    /**
     * Changes the center of the OBB.
     *
     * \param center the new center
     */
    void set_center(const vec2d center) noexcept;

    /**
     * Retrieves the center of the OBB.
     *
     * \returns the center
     */
    vec2d get_center() const noexcept;

    /**
     * Changes the extents of the OBB.
     *
     * \param extents the new extents
     */
    void set_extents(const vec2d extents) noexcept;

    /**
     * Retrieves the extents of the OBB.
     *
     * \returns the extents
     */
    vec2d get_extents() const noexcept;

    /**
     * Changes the angle of the OBB.
     *
     * \param angle the new angle
     */
    void set_angle(const double angle) noexcept;

    /**
     * Retrieves the current angle of the OBB.
     *
     * \returns the angle
     */
    double get_angle() const noexcept;

    /**
     * Rotates the OBB by an angle.
     *
     * \param angle the angle to rotate by
     */
    void rotate(const double angle) noexcept;

  private:
    vec2d m_center;
    vec2d m_extents;
    double m_angle;
};

} // namespace ccsakura
