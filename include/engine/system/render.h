/**
 * \file engine/system/render.h
 *
 * Provides the system for rendering in the ECS.
 */

#pragma once

#include "engine/camera.h"
#include "engine/component/collider.h"
#include "engine/component/sprite.h"
#include "engine/component/text.h"
#include "engine/entity.h"
#include "sdl/sdl_render.h"

namespace ccsakura::system
{

/**
 * Renders a collider component.
 *
 * \param collider the collider to render.
 */
void render_collider(const components::collider &collider, const sdl::irenderer &renderer, const camera2d &camera);

/**
 * Renders a sprite component at the given screen position.
 *
 * \param renderer the renderer to use
 * \param spr the sprite component
 * \param screen_pos screen-space position
 * \param rotation rotation in radians
 */
void render_sprite(const sdl::irenderer &renderer, const components::sprite &spr, const sdl::fpoint &screen_pos,
                   double rotation) noexcept;

/**
 * Renders a text component at the given screen position.
 * Uses the "font" sprite sheet; ASCII 32–126 are supported.
 *
 * \param renderer the renderer to use
 * \param txt the text component
 * \param screen_pos screen-space position
 */
void render_text(const sdl::irenderer &renderer, const components::text &txt, const sdl::fpoint &screen_pos) noexcept;

/**
 * Renders all renderable components of an entity (sprite, hitbox, text).
 *
 * \param renderer the renderer to use
 * \param ent the entity to render
 * \param cam the active camera
 */
void render_entity(const sdl::irenderer &renderer, const entity &ent, const camera2d &cam) noexcept;

} // namespace ccsakura::system
