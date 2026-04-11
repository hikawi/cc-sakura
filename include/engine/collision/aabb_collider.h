#pragma once

#include "engine/collision.h"

namespace ccsakura
{

/**
 * An axis-aligned bounding box collider.
 *
 * This is the simplest collider to use, and should be used in a lot of situations, just because it's much easier to
 * deal with.
 */
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
    void render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept override;

    /**
     * Changes the center of the AABB.
     *
     * \param center the new center
     */
    void set_center(const vec2d center) noexcept;

    /**
     * Retrieves the center of the AABB.
     *
     * \returns the center
     */
    vec2d get_center() const noexcept;

    /**
     * Changes the extents of the AABB.
     *
     * \param extents the new extents
     */
    void set_extents(const vec2d extents) noexcept;

    /**
     * Retrieves the extents of the AABB.
     *
     * \returns the extents
     */
    vec2d get_extents() const noexcept;

    /**
     * Retrieves the closest point on the AABB that is the closest to the provided point P.
     *
     * \param p the pointer to check against
     * \returns the point on the aabb closest to p
     */
    vec2d closest_point_to(const vec2d p) const noexcept;

  private:
    vec2d m_center;
    vec2d m_extents;
};

} // namespace ccsakura
