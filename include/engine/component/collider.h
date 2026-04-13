/**
 * \file engine/component/collider.h
 *
 * Provides a simple data struct object for hitboxes and collisions.
 */

#pragma once

#include "engine/component.h"
#include "engine/vec2d.h"
#include "sdl/sdl_pixels.h"

#include <variant>

namespace ccsakura::components
{

/**
 * An axis-aligned bounding box collider.
 *
 * This is the simplest collider to use, and should be used in a lot of situations, just because it's much easier to
 * deal with.
 */
struct aabb_collider
{
    vec2d center = {0, 0};       ///< The center of the collider.
    vec2d half_extents = {0, 0}; ///< Half-extents of the collider.
};

/**
 * An oriented bounding box collider.
 *
 * This is the second most complicated capsule to use. Use this for moving or rotating platforms.
 */
struct obb_collider
{
    vec2d center = {0, 0};       ///< The center of the collider.
    vec2d half_extents = {0, 0}; ///< Half-extents of the collider.
    double angle = 0;            ///< Rotation of the collider in radians.
};

/**
 * A circle collider.
 *
 * This is one of the primitive types of colliders, and should be used for round objects.
 */
struct circle_collider
{
    vec2d center;  ///< The center of the collider.
    double radius; ///< The radius of the circle.
};

/**
 * A capsule collider.
 *
 * This is the most complicated type of collider, it should not be overused. This is mainly only used for some main
 * characters.
 */
struct capsule_collider
{
    vec2d p1;      ///< First point of the collider.
    vec2d p2;      ///< Second point of the collider.
    double radius; ///< What makes it a fat stick.
};

/**
 * Provides the collider component for handling physics.
 */
struct collider : component
{
    using shape_type = std::variant<aabb_collider, obb_collider, circle_collider, capsule_collider>;

    shape_type shape;                   ///< The shape of the collider, which is an std::variant.
    sdl::fcolor color = {0, 0, 0, 0.5}; ///< The color of the collider to render as, if enabled.

    collider(const shape_type shape);

    /**
     * Retrieves whether the shape is of the given collider type.
     *
     * \returns true if the collider is of that type
     */
    template <typename T> bool is() const
    {
        return std::holds_alternative<T>(shape);
    }

    /**
     * Retrieves a reference to the shape as the given collider type.
     * Throws std::bad_variant_access if the shape is a different type.
     *
     * \returns a collider of type T
     */
    template <typename T> T &as()
    {
        return std::get<T>(shape);
    }

    /**
     * Returns a const reference to the shape as the given collider type.
     * Throws std::bad_variant_access if the shape is a different type.
     *
     * \returns a const collider of type T
     */
    template <typename T> const T &as() const
    {
        return std::get<T>(shape);
    }
};

} // namespace ccsakura::components
