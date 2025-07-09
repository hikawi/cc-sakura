// engine/renderer.h
//
// The rendering portion of the engine.

#pragma once

#include "SDL3/SDL_render.h"
#include "app.h"
#include "engine/map.h"
#include "engine/sprite.h"

typedef enum
{
    RENDER_ORIGIN_TOP_LEFT,
    RENDER_ORIGIN_TOP_CENTER,
    RENDER_ORIGIN_TOP_RIGHT,
    RENDER_ORIGIN_MIDDLE_LEFT,
    RENDER_ORIGIN_MIDDLE_CENTER,
    RENDER_ORIGIN_MIDDLE_RIGHT,
    RENDER_ORIGIN_BOTTOM_LEFT,
    RENDER_ORIGIN_BOTTOM_CENTER,
    RENDER_ORIGIN_BOTTOM_RIGHT,
} RenderingOriginType;

typedef struct
{
    RenderingOriginType origin;
    SDL_Texture *texture;
    SDL_FRect *srcrect;
    SDL_FRect *dstrect;
} RenderingOptions;

/**
 * Shifts the position into the provided origin.
 */
void shift_position_to_origin(RenderingOriginType type, double *x, double *y,
                              double w, double h);

/**
 * Renders a texture that is aligned with its origin.
 */
void render_aligned_texture(RenderingOptions options);

/**
 * Renders a map with the provided sprite sheet.
 */
void render_map(Map *map, Sprite *spr);

/**
 * Renders the current app state.
 */
void render_state(AppState *app);
