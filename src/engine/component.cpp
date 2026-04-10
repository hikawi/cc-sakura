/**
 * \file engine/component.cpp
 *
 * Provides implementation for the engine components.
 */

#include "engine/component.h"

#include "engine/sprite.h"

namespace ccsakura::components
{

transform::transform(vec2d pos, double rot) : position(pos), rotation(rot)
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
        const uint32_t count = spr->frame_count();
        if (reverse)
        {
            if (frame_index == 0)
                frame_index = count - 1;
            else
                frame_index--;
        }
        else
        {
            frame_index = (frame_index + 1) % count;
        }
    }
    else
    {
        const auto &t = spr->frame_tag(tag);
        if (reverse)
        {
            if (frame_index <= t.from)
                frame_index = t.to;
            else
                frame_index--;
        }
        else
        {
            frame_index++;
            if (frame_index > t.to)
                frame_index = t.from;
        }
    }
}

} // namespace ccsakura::components
