/**
 * \file engine/renderer.h
 *
 * Helper utilities for the rendering part of the engine.
 */

#pragma once

#include "SDL3/SDL_render.h"

/**
 * Specifies where the origin of the rendered texture is.
 */
typedef enum
{
    RENDER_ORIGIN_TOP_LEFT,      ///< Puts the origin at the top left
    RENDER_ORIGIN_TOP_CENTER,    ///< Puts the origin at the top center
    RENDER_ORIGIN_TOP_RIGHT,     ///< Puts the origin at the top right
    RENDER_ORIGIN_MIDDLE_LEFT,   ///< Puts the origin at the middle left
    RENDER_ORIGIN_MIDDLE_CENTER, ///< Puts the origin at the middle center
    RENDER_ORIGIN_MIDDLE_RIGHT,  ///< Puts the origin at the middle right
    RENDER_ORIGIN_BOTTOM_LEFT,   ///< Puts the origin at the bottom left
    RENDER_ORIGIN_BOTTOM_CENTER, ///< Puts the origin at the bottom center
    RENDER_ORIGIN_BOTTOM_RIGHT,  ///< Puts the origin at the bottom right
} RenderingOriginType;

/**
 * Information struct for rendering a texture.
 */
typedef struct
{
    RenderingOriginType origin; ///< The origin of the texture to place at
    SDL_Renderer *renderer;     ///< The renderer to render to
    SDL_Texture *texture;       ///< The texture to render
    SDL_FRect *srcrect;         ///< The portion of the texture to render
    SDL_FRect *dstrect;         ///< The portion of the screen to render to
    double rotation;            ///< The rotation of the texture, in radians
    bool flip_hori;             ///< Whether to flip the texture horizontally
    bool flip_vert;             ///< Whether to flip the texture vertically
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
