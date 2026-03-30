/**
 * \file engine/component.cpp
 *
 * Provides implementation for the engine components.
 */

#include "engine/component.h"

namespace ccsakura::components
{

transform::transform(vec2d pos, double rot, vec2d scl) : position(pos), rotation(rot), scale(scl)
{
}

velocity::velocity(vec2d val) : value(val)
{
}

sprite::sprite(isprite *s, render_origin o) : spr(s), origin(o)
{
}

} // namespace ccsakura::components
