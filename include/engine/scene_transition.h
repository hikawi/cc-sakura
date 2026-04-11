/**
 * \file engine/scene_transition.h
 *
 * Defines scene transition types, configuration, and the rendering dispatch function.
 */

#pragma once

#include "sdl/sdl_render.h"

namespace ccsakura
{

enum class scene_transition_type
{
    none,
    fade,
    slide_left,
    slide_right,
    slide_up,
    slide_down,
    split_horizontal,
    split_vertical,
};

struct scene_transition
{
    scene_transition_type type = scene_transition_type::none;
    double duration = 0.0;

    static scene_transition fade(double d) noexcept
    {
        return {scene_transition_type::fade, d};
    }
    static scene_transition slide_left(double d) noexcept
    {
        return {scene_transition_type::slide_left, d};
    }
    static scene_transition slide_right(double d) noexcept
    {
        return {scene_transition_type::slide_right, d};
    }
    static scene_transition slide_up(double d) noexcept
    {
        return {scene_transition_type::slide_up, d};
    }
    static scene_transition slide_down(double d) noexcept
    {
        return {scene_transition_type::slide_down, d};
    }
    static scene_transition split_horizontal(double d) noexcept
    {
        return {scene_transition_type::split_horizontal, d};
    }
    static scene_transition split_vertical(double d) noexcept
    {
        return {scene_transition_type::split_vertical, d};
    }
};

/**
 * Renders a transition frame by compositing the from and to scene textures.
 *
 * Each effect type owns the rendering of both textures. Called once per frame
 * during an active transition, in place of the normal per-scene composite draw.
 *
 * \param renderer the renderer to draw with
 * \param from_tex texture of the outgoing scene
 * \param to_tex   texture of the incoming scene
 * \param type     the transition effect to apply
 * \param t        normalized progress (0.0 = start, 1.0 = end)
 * \param vw       viewport width
 * \param vh       viewport height
 */
void render_transition(const sdl::irenderer &renderer, sdl::itexture &from_tex, sdl::itexture &to_tex,
                       scene_transition_type type, double t, float vw, float vh) noexcept;

} // namespace ccsakura
