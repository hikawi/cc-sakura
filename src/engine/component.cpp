/**
 * \file engine/component.cpp
 *
 * Provides implementation for the engine components.
 */

#include "engine/component.h"

#include "engine/sprite.h"

namespace ccsakura::components
{

transform::transform(vec2d pos, double rot, vec2d scl) : position(pos), rotation(rot), scale(scl)
{
}

sprite::sprite(isprite *s, render_origin o) : spr(s), origin(o)
{
}

void sprite::tick(double dt) noexcept
{
    if (!spr)
        return;

    frame_elapsed += dt;
    const double frame_dur = spr->frame(frame_index).duration / 1000.0;
    if (frame_elapsed < frame_dur)
        return;

    frame_elapsed -= frame_dur;

    if (tag.empty())
    {
        frame_index = (frame_index + 1) % spr->frame_count();
    }
    else
    {
        const auto &t = spr->frame_tag(tag);
        frame_index++;
        if (frame_index > t.to)
            frame_index = t.from;
    }
}

} // namespace ccsakura::components
