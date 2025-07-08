// engine/sprite.h
//
// Handles the loading, creating and destroying sprites.

#pragma once

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "misc/vector.h"
#include <stdbool.h>

/**
 * Represents a sprite frame. It holds a certain rectangle that covers around
 * the sprite for pixel-perfect rendering, and for collision calculations.
 */
typedef struct
{
    SDL_FRect frame;  // The absolute srcrect of the frame to the main texture.
    Vector2 size;     // The source size of the frame.
    SDL_FRect offset; // The offset rect based on the original source size.
    Uint32 duration;  // The duration of the frame.
} SpriteFrame;

/**
 * Represents a frame tag. Provided by Aseprite, denoted from which index to
 * which index is a tag.
 */
typedef struct
{
    char *tag;
    Uint32 from;
    Uint32 to;
} FrameTag;

/**
 * Represents a Sprite. A sprite can be animated or not (determined by the `fps`
 * field). A sprite also has frames, where each frame determines the source
 * rectangle WITHIN the frame.
 */
typedef struct
{
    SDL_Texture *texture; // The GPU accelerated texture to paint, it can be a
                          // whole sprite sheet.
    SpriteFrame *frames;  // The sprite's frames.
    Uint32 num_frames;    // The number of frames.
    Vector2 size;         // The size of the sprite sheet as a whole.
    FrameTag *tags;       // The sprite's frame tags.
    Uint32 num_tags;      // The number of tags.

    bool playing;       // Whether the sprite is playing.
    int sel_tag;        // The frame tag that is currently being chosen.
    Uint32 frame_idx;   // The current frame index of the sprite.
    double frame_accum; // The accumulator for frames to decide when we should
                        // move on to next frame.
} Sprite;

/**
 * Loads a sprite from a sprite asset.
 */
Sprite *init_sprite_from_sheet(const char *sprite);

/**
 * Sets the sprite animation's currently selected tag.
 */
bool set_sprite_animation(Sprite *spr, const char *name);

/**
 * Resets a sprite's animation back to the default state.
 */
void reset_sprite_animation(Sprite *spr);

/**
 * Advances a sprite's animation status by a deltatime amount.
 * This does nothing if the sprite is not playing.
 *
 * Returns true if the sprite was advanced a step.
 */
bool advance_sprite_animation(Sprite *spr, double dt);

/**
 * Renders a sprite into the screen, at the exact center of the provided
 * position.
 */
void render_sprite(Sprite *spr, Vector2 pos);

/**
 * Destroys a sprite.
 */
void destroy_sprite(Sprite *spr);
