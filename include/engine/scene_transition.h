/**
 * \file engine/scene_transition.h
 *
 * Defines scene transition types, configuration, and the rendering dispatch function.
 */

#pragma once

#include "sdl/sdl_render.h"

namespace ccsakura
{

/**
 * The type of the scene transition to animate.
 */
enum class scene_transition_type
{
    none,             ///< Do nothing, similar to pushing.
    fade,             ///< Fade into the destination scene.
    slide_left,       ///< Current scene slides left as the destination scene slides in.
    slide_right,      ///< Current scene slides right as the destination scene slides in.
    slide_up,         ///< Current scene slides up as the destination scene slides in.
    slide_down,       ///< Current scene slides down as the destination scene slides in.
    split_horizontal, ///< Current scene splits horizontally revealing the destination scene underneath.
    split_vertical,   ///< Current scene splits vertically revealing the destination scene underneath.
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
