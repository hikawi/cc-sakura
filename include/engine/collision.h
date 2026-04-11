/**
 * \file engine/collision.h
 *
 * Provides a framework for handling collisions and making collider checks and how to resolve such collisions with
 * normal vectors.
 */

#pragma once

#include "engine/render.h"
#include "engine/vec2d.h"
#include "sdl/sdl_pixels.h"
#include "sdl/sdl_render.h"

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

class aabb_collider;
class obb_collider;
class circle_collider;
class capsule_collider;

/**
 * Base interface for a collider.
 *
 * All colliders should inherit from this virtual interface.
 */
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
     * Retrieves the current color of the collider.
     *
     * \returns the collider color
     */
    sdl::fcolor get_color() const noexcept;

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

    /**
     * Renders the collider for debug purposes.
     *
     * \param renderer the renderer to render with
     * \param camera the active camera for world-to-screen transformation
     */
    virtual void render(const sdl::irenderer &renderer, const camera2d &camera) const noexcept = 0;

  protected:
    sdl::fcolor m_color = {0, 0, 0.5f, 0.3f}; ///< the color to render the collider with
};

} // namespace ccsakura

#include "engine/collision/aabb_collider.h"
#include "engine/collision/capsule_collider.h"
#include "engine/collision/circle_collider.h"
#include "engine/collision/obb_collider.h"
