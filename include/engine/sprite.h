/**
 * \file engine/sprite.h
 *
 * Provides a cache of sprite sheets and configurable and extensible sprite system.
 */

#pragma once

#include "engine/renderer.h"
#include "misc/genhashmap.h"
#include "misc/vector.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

#include <stdint.h>

/**
 * Represents a single frame of a sprite.
 */
typedef struct
{
    SDL_Rect frame;           ///< The frame where the sprite is in the main texture.
    SDL_Rect spr_source_size; ///< The sprite's frame in the original source box.
    struct
    {
        uint32_t w;    ///< Width of the original source box.
        uint32_t h;    ///< Height of the original source box.
    } source_size;     ///< The dimensions of the original source box.
    uint32_t duration; ///< The duration that an animation can stay for.
} SpriteFrame;

/**
 * Represents a tag that tags a sequence of frames with a name.
 */
typedef struct
{
    char *name;    ///< The name of the tag
    uint32_t from; ///< The index where the tag sequence starts, inclusive
    uint32_t to;   ///< The index where the tag sequence ends, inclusive
} SpriteFrameTag;

/**
 * Represents a sprite that maps to a sprite sheet.
 */
typedef struct
{
    SDL_Texture *texture; ///< The GPU-accelerated texture for the whole sheet
    char *name;           ///< The name of the sprite
    uint32_t w;           ///< The width of the original texture
    uint32_t h;           ///< The height of the original texture

    SpriteFrameTag *tags;     ///< The array of frame tags
    uint32_t tags_length;     ///< The length of the frame tags array
    uint32_t sel_tag;         ///< The currently selected frame tag index
    GenericHashMap *tags_map; ///< Hashmap for quickly querying frame tags by string

    SpriteFrame *frames;    ///< The array of frames
    uint32_t frames_length; ///< The length of the frames array
    uint32_t frame_idx;     ///< The current index into the frame array

    bool playing;       ///< Whether the animation should be playing
    double frame_accum; ///< The accumulator in seconds for animation calculation
    double scale;       ///< The sprite's local scaling
    double rotation;    ///< The sprite's rotation
    bool flip_hori;     ///< Whether to flip the sprite horizontally
    bool flip_vert;     ///< Whether to flip the sprite vertically
} Sprite;

/**
 * Represents a struct for passing properties into the \ref sprite_render function.
 */
typedef struct
{
    const Sprite *spr;          ///< The sprite to render.
    SDL_Renderer *renderer;     ///< The renderer to render with.
    Vector2 pos;                ///< The position to render at.
    RenderingOriginType origin; ///< The origin of the rendered sprite.
} SpriteRenderProperties;

/**
 * Loads a sprite from a texture and cache the sprite's sheet.
 *
 * The sprite sheet will stay available for as long as there are multiple sprites pointing to it.
 *
 * \param name the name of the sprite to load
 * \returns the sprite with a cached texture
 */
Sprite *sprite_load(const char *name);

/**
 * Selects a tag in the sprite and resets the animation.
 *
 * \param spr the sprite to modify
 * \param tag the tag to set to
 * \returns true if the tag was set correctly, false otherwise
 */
bool sprite_select_tag(Sprite *spr, const char *tag);

/**
 * Advances a sprite's animation by a deltatime amount.
 *
 * This does nothing if the sprite is currently paused.
 *
 * \param spr the sprite to advance
 * \param dt the deltatime to advance
 */
void sprite_advance_animation(Sprite *spr, const double dt);

/**
 * Renders a sprite.
 *
 * \pzram props the properties for rendering a sprite
 */
void sprite_render(SpriteRenderProperties props);

/**
 * Destroys a sprite.
 *
 * \param spr the sprite to destroy
 */
void sprite_destroy(Sprite *spr);
