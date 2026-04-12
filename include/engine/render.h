/**
 * \file engine/render.h
 *
 * Simple rendering options as utility for objects rendering themselves.
 */

#pragma once

#include "engine/vec2d.h"
#include "sdl/sdl_rect.h"

namespace sdl
{
class irenderer;
} // namespace sdl

namespace ccsakura
{

class entity;

namespace components
{
struct sprite;
struct text;
} // namespace components

/**
 * Specifies the location to start rendering.
 *
 * This is to deal with the fact that SDL rendering only starts at top left, this origin would shift all pixels back a
 * certain amount to simulate the process of drawing from a different origin.
 */
enum class render_origin
{
    top_left,      ///< the top left corner of the rectangle
    top_center,    ///< the top center of the rectangle
    top_right,     ///< the top right corner of the rectangle
    middle_left,   ///< the middle left of the rectangle
    middle_center, ///< the middle center of the rectangle
    middle_right,  ///< the middle right of the rectangle
    bottom_left,   ///< the bottom left corner of the rectangle
    bottom_center, ///< the bottom center of the rectangle
    bottom_right,  ///< the bottom right corner of the rectangle
};

/**
 * Represents a camera in 2D space.
 */
struct camera2d
{
    vec2d position;      ///< the camera's world coordinates
    double rotation = 0; ///< the camera's tilt or rotation, based on the x axis
    double zoom = 1;     ///< the camera's zoom level
    vec2d viewport;      ///< the camera's viewport to render
};

/**
 * Shifts the provided rectangle to the correct origin for SDL to render from the top left.
 *
 * \param frect the rectangle to shift
 * \param origin the new render origin
 */
void shift_origin(sdl::frect &frect, const render_origin origin);

/**
 * Converts a world-space position to screen-space using the given camera.
 *
 * \param world the world position to convert
 * \param cam the active camera
 * \returns the screen-space position
 */
sdl::fpoint world_to_screen(const vec2d &world, const camera2d &cam) noexcept;

/**
 * Converts a world-space position to screen-space using the given camera.
 *
 * \param world the world rectangle
 * \param cam the active camera
 * \returns the screen-space position
 */
sdl::frect world_to_screen(const sdl::frect &world, const camera2d &cam) noexcept;

void render_hitbox();

/**
 * Renders a sprite component at the given screen position.
 *
 * \param renderer the renderer to use
 * \param spr the sprite component
 * \param screen_pos screen-space position
 * \param rotation rotation in radians
 */
void render_sprite(const sdl::irenderer &renderer, const components::sprite &spr,
                   const sdl::fpoint &screen_pos, double rotation) noexcept;

/**
 * Renders a text component at the given screen position.
 * Uses the "font" sprite sheet; ASCII 32–126 are supported.
 *
 * \param renderer the renderer to use
 * \param txt the text component
 * \param screen_pos screen-space position
 */
void render_text(const sdl::irenderer &renderer, const components::text &txt,
                 const sdl::fpoint &screen_pos) noexcept;

/**
 * Renders all renderable components of an entity (sprite, hitbox, text).
 *
 * \param renderer the renderer to use
 * \param ent the entity to render
 * \param cam the active camera
 */
void render_entity(const sdl::irenderer &renderer, const entity &ent, const camera2d &cam) noexcept;

} // namespace ccsakura
