/**
 * \file engine/component/transform.h
 *
 * Provides a simple data struct object for positions and rotations.
 */

#pragma once

#include "engine/component.h"
#include "engine/vec2d.h"

namespace ccsakura::components
{

/**
 * Represents the position, rotation, and scale of an entity.
 */
struct transform : public component
{
    vec2d position;     ///< World position in logical pixels
    double rotation;    ///< Rotation in radians
    bool fixed = false; ///< If true, position is in screen space and skips camera mapping

    /**
     * Constructs a new transform component.
     *
     * \param pos the position
     * \param rot the rotation
     * \param fixed whether the transform is in screen space
     */
    transform(vec2d pos = {0, 0}, double rot = 0, bool fixed = false);
};

} // namespace ccsakura::components
