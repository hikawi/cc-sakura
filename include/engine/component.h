/**
 * \file engine/component.h
 *
 * Provides the component abstraction for the engine.
 */

#pragma once

#include "collision.h"
#include "render.h"
#include "vec2d.h"

#include <cstdint>
#include <string>
#include <variant>

namespace ccsakura
{

class isprite;

/**
 * Represents a piece of abstraction that can be composed onto an entity.
 */
class component
{
  public:
    virtual ~component() = default;
};

namespace components
{

/**
 * Represents the position, rotation, and scale of an entity.
 */
struct transform : public component
{
    vec2d position;  ///< World position in logical pixels
    double rotation; ///< Rotation in radians
    vec2d scale;     ///< Scale multiplier on each axis

    /**
     * Constructs a new transform component.
     *
     * \param pos the position
     * \param rot the rotation
     * \param scl the scale
     */
    transform(vec2d pos = {0, 0}, double rot = 0, vec2d scl = {1, 1});
};

/**
 * Represents a sprite component.
 */
struct sprite : public component
{
    isprite *spr;               ///< Non-owning pointer to the sprite resource
    render_origin origin;       ///< Anchor point used for rendering alignment
    uint32_t frame_index = 0;   ///< Current frame being displayed
    double frame_elapsed = 0.0; ///< Seconds elapsed in the current frame
    std::string tag = "";       ///< Active animation tag name; empty means play all frames

    /**
     * Constructs a new sprite component.
     *
     * \param s the sprite to use
     * \param o the render origin
     */
    sprite(isprite *s = nullptr, render_origin o = render_origin::middle_center);

    /**
     * Advances the animation by the given delta time.
     *
     * \param dt delta time in seconds
     */
    void tick(double dt) noexcept;
};

/**
 * Represents a collider attached to an entity.
 */
struct hitbox : public component
{
    using shape_t = std::variant<aabb_collider, obb_collider, circle_collider, capsule_collider>;

    shape_t shape; ///< The concrete collider shape

    explicit hitbox(shape_t s) : shape(std::move(s))
    {
    }

    /**
     * Returns a reference to the base collider for collision detection and rendering.
     */
    collider &get()
    {
        return std::visit([](auto &c) -> collider & { return c; }, shape);
    }

    /**
     * Returns a const reference to the base collider for collision detection and rendering.
     */
    const collider &get() const
    {
        return std::visit([](const auto &c) -> const collider & { return c; }, shape);
    }

    /**
     * Returns whether the shape is of the given collider type.
     */
    template <typename T> bool is() const
    {
        return std::holds_alternative<T>(shape);
    }

    /**
     * Returns a reference to the shape as the given collider type.
     * Throws std::bad_variant_access if the shape is a different type.
     */
    template <typename T> T &as()
    {
        return std::get<T>(shape);
    }

    /**
     * Returns a const reference to the shape as the given collider type.
     * Throws std::bad_variant_access if the shape is a different type.
     */
    template <typename T> const T &as() const
    {
        return std::get<T>(shape);
    }
};

} // namespace components

} // namespace ccsakura
