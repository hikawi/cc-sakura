/**
 * \file engine/renderer.h
 *
 * Helper utilities for the rendering part of the engine.
 */

#pragma once

#include "engine/map.h"
#include "engine/sprite.h"
#include "SDL3/SDL_render.h"

/**
 * Specifies where the origin of the rendered texture is.
 */
typedef enum
{
    /**
     * Puts the origin at the top left.
     */
    RENDER_ORIGIN_TOP_LEFT,

    /**
     * Puts the origin at the top center.
     */
    RENDER_ORIGIN_TOP_CENTER,

    /**
     * Puts the origin at the top right.
     */
    RENDER_ORIGIN_TOP_RIGHT,

    /**
     * Puts the origin at the middle left.
     */
    RENDER_ORIGIN_MIDDLE_LEFT,

    /**
     * Puts the origin at the true center.
     */
    RENDER_ORIGIN_MIDDLE_CENTER,

    /**
     * Puts the origin at the middle right.
     */
    RENDER_ORIGIN_MIDDLE_RIGHT,

    /**
     * Puts the origin at the bottom left.
     */
    RENDER_ORIGIN_BOTTOM_LEFT,

    /**
     * Puts the origin at the bottom center.
     */
    RENDER_ORIGIN_BOTTOM_CENTER,

    /**
     * Puts the origin at the bottom right.
     */
    RENDER_ORIGIN_BOTTOM_RIGHT,
} RenderingOriginType;

/**
 * Information struct for rendering a texture.
 */
typedef struct
{
    /**
     * The origin's placement in the texture.
     */
    RenderingOriginType origin;

    /**
     * The texture to render.
     */
    SDL_Texture *texture;

    /**
     * The portion of the texture to render.
     */
    SDL_FRect *srcrect;

    /**
     * The portion of the screen to render to.
     */
    SDL_FRect *dstrect;

    /**
     * The texture's rotation.
     */
    double rotation;

    /**
     * Whether to flip the texture horizontally.
     */
    bool flip_hori;

    /**
     * Whether to flip the texture vertically.
     */
    bool flip_vert;
} RenderingOptions;

/**
 * Shifts the position into the provided origin.
 *
 * \param type the origin type
 * \param x the x pointer to shift
 * \param y the y pointer to shift
 * \param w the texture's width
 * \param h the texture's height
 */
void shift_position_to_origin(RenderingOriginType type, double *x, double *y, double w, double h);

/**
 * Renders a texture that is aligned with its origin.
 *
 * \param options the rendering options
 */
void render_aligned_texture(RenderingOptions options);

/**
 * \internal
 *
 * Renders a map with the provided sprite sheet.
 */
void render_map(Map *map, Sprite *spr);

/**
 * Renders a sprite into the screen, at the exact center of the provided
 * position.
 *
 * \param spr the sprite to render
 * \param pos the position to render
 * \deprecated This function does not respect the app's logical scaling.
 */
void render_sprite(Sprite *spr, Vector2 pos);
