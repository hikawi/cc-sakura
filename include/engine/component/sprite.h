/**
 * \file engine/component/sprite.h
 *
 * Provides a data struct for an entity holding a sprite.
 */

#pragma once

#include "engine/component.h"
#include "engine/render_origin.h"
#include "engine/sprite.h"

namespace ccsakura::components
{

/**
 * Represents a sprite component.
 */
struct sprite : public component
{
    isprite *spr;                     ///< Non-owning pointer to the sprite resource
    render_origin origin;             ///< Anchor point used for rendering alignment
    uint32_t frame_index = 0;         ///< Current frame being displayed
    double frame_elapsed = 0.0;       ///< Seconds elapsed in the current frame
    std::string tag = "";             ///< Active animation tag name; empty means play all frames
    sdl::flip flip = sdl::flip::none; ///< Flip mode
    bool reverse = false;             ///< Whether to play the animation in reverse

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

} // namespace ccsakura::components
