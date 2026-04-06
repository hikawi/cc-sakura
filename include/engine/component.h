/**
 * \file engine/component.h
 *
 * Provides the component abstraction for the engine.
 */

#pragma once

#include "render.h"
#include "vec2d.h"

#include <cstdint>
#include <string>

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
    vec2d position;
    double rotation;
    vec2d scale;

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
 * Represents the velocity of an entity.
 */
struct velocity : public component
{
    vec2d value;

    /**
     * Constructs a new velocity component.
     *
     * \param val the velocity value
     */
    velocity(vec2d val = {0, 0});
};

/**
 * Represents a sprite component.
 */
struct sprite : public component
{
    isprite *spr;
    render_origin origin;
    uint32_t frame_index = 0;   ///< Current frame being displayed
    double frame_elapsed = 0.0; ///< Seconds elapsed in the current frame
    std::string tag = "";       ///< Active animation tag name; empty means play all frames

    /**
     * Constructs a new sprite component.
     *
     * \param s the sprite to use
     * \param o the render origin
     */
    sprite(isprite *s = nullptr, render_origin o = render_origin::top_left);
};

} // namespace components

} // namespace ccsakura
