#include "engine/sprite.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "render/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Version 1 of the sprite decoder tool.
 */
void init_sprite_v1(SDL_IOStream *io, Sprite **spr)
{
    // First let's read the image first.
    // We start with the image length and then all the bytes of the image.
    Uint64 img_len;
    SDL_ReadU64LE(io, &img_len);

    char *img_data = SDL_malloc(img_len * sizeof(char));
    if (!SDL_ReadIO(io, img_data, img_len * sizeof(char)))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to load image data?");
        SDL_free(img_data);
        return;
    }

    // Read the template width and height.
    Uint32 width, height;
    SDL_ReadU32LE(io, &width);
    SDL_ReadU32LE(io, &height);

    // Load the frame tags.
    Uint32 num_tags;
    SDL_ReadU32LE(io, &num_tags);
    FrameTag *tags = NULL;
    if (num_tags > 0)
    {
        tags = SDL_malloc(sizeof(FrameTag) * num_tags);

        for (Uint32 i = 0; i < num_tags; i++)
        {
            // For each tag, we reach the name, and the "from" and "to".
            Uint32 name_len;
            SDL_ReadU32LE(io, &name_len);

            char *name = SDL_malloc(sizeof(char) * (name_len + 1));
            SDL_ReadIO(io, name, name_len);
            name[name_len] = '\0';
            tags[i].tag = name;

            SDL_ReadU32LE(io, &tags[i].from);
            SDL_ReadU32LE(io, &tags[i].to);
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "No frame tags are loaded.");
    }

    // Next. We read in the frames
    SpriteFrame *frames = NULL;
    Uint32 num_frames;
    SDL_ReadU32LE(io, &num_frames);

    // For each of them now.
    if (num_frames > 0)
    {
        frames = SDL_malloc(sizeof(SpriteFrame) * num_frames);

        for (Uint32 i = 0; i < num_frames; i++)
        {
            // For each frame, we read in the following:
            // source width, source height, relative x, relative y, relative
            // width, relative height, absolute x, absolute y, absolute width
            // and absolute height, duration.
            Uint32 srcw, srch, relx, rely, relw, relh, absx, absy, absw, absh;
            SDL_ReadU32LE(io, &srcw);
            SDL_ReadU32LE(io, &srch);
            SDL_ReadU32LE(io, &relx);
            SDL_ReadU32LE(io, &rely);
            SDL_ReadU32LE(io, &relw);
            SDL_ReadU32LE(io, &relh);
            SDL_ReadU32LE(io, &absx);
            SDL_ReadU32LE(io, &absy);
            SDL_ReadU32LE(io, &absw);
            SDL_ReadU32LE(io, &absh);

            frames[i].size.x = srcw;
            frames[i].size.y = srch;
            frames[i].offset.x = relx;
            frames[i].offset.y = rely;
            frames[i].offset.w = relw;
            frames[i].offset.h = relh;
            frames[i].frame.x = absx;
            frames[i].frame.y = absy;
            frames[i].frame.w = absw;
            frames[i].frame.h = absh;
            SDL_ReadU32LE(io, &frames[i].duration);
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "No frames are loaded.");
    }

    // Only allocate when we're sure everything is ready.
    Sprite *sprite = SDL_malloc(sizeof(Sprite));
    sprite->tags = tags;
    sprite->num_tags = num_tags;
    sprite->size.x = width;
    sprite->size.y = height;
    sprite->frames = frames;
    sprite->num_frames = num_frames;
    sprite->playing = false;
    sprite->frame_idx = 0;
    sprite->frame_accum = 0;
    sprite->sel_tag = -1;

    // Load the texture needed.
    SDL_IOStream *img_io = SDL_IOFromMem(img_data, img_len);
    SDL_Surface *surface = IMG_Load_IO(img_io, true);
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(get_current_renderer(), surface);
    sprite->texture = texture;

    SDL_DestroySurface(surface);
    *spr = sprite;
}

Sprite *init_sprite_from_sheet(const char *sprite)
{
    SDL_Log("Attempting to load sprite %s", sprite);

    SDL_IOStream *io = SDL_IOFromFile(sprite, "r");
    Sprite *spr = NULL;

    if (io == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load sprite %s",
                     sprite);
        return NULL;
    }

    // Ok we can now create the Sprite *. Hopefully it's not the wrong file
    // type!
    Uint32 version;
    SDL_ReadU32LE(io, &version);

    switch (version)
    {
    case 1:
        init_sprite_v1(io, &spr);
        break;
    default:
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Unknown sprite version. Can't decode");
        break;
    }

    SDL_CloseIO(io);
    return spr;
}

bool set_sprite_animation(Sprite *spr, const char *name)
{
    int idx = -1;
    for (Uint32 i = 0; i < spr->num_tags; i++)
    {
        if (SDL_strcmp(spr->tags[i].tag, name) == 0)
        {
            idx = (int)i;
            break;
        }
    }

    spr->sel_tag = idx;
    if (idx < 0)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "Trying to set sprite's animation to %s failed.", name);
    }
    else
    {
        reset_sprite_animation(spr);
    }

    return idx >= 0;
}

void reset_sprite_animation(Sprite *spr)
{
    spr->frame_idx = 0;
    spr->frame_accum = 0;
    spr->playing = false;
}

bool advance_sprite_animation(Sprite *spr, double dt)
{
    spr->frame_accum += dt;

    // Get the current frame and see if we passed the threshold.
    SpriteFrame *current = NULL;
    Uint32 total;

    if (spr->sel_tag >= 0)
    {
        FrameTag tag = spr->tags[spr->sel_tag];
        total = tag.to - tag.from + 1;
        current = &spr->frames[tag.from + spr->frame_idx];
    }
    else
    {
        total = spr->num_frames;
        current = &spr->frames[spr->frame_idx];
    }

    // Check if we passed the sprite frame's duration threshold.
    // If so, we advance the index by one.
    if (current && spr->frame_accum > (current->duration / 1000.0))
    {
        spr->frame_accum -= (current->duration / 1000.0);
        spr->frame_idx = (spr->frame_idx + 1) % total;
        return true;
    }

    return false;
}

void render_sprite(Sprite *spr, Vector2 pos)
{
    // Get the current texture.
    SpriteFrame *frame = NULL;
    if (spr->sel_tag < 0)
    {
        frame = &spr->frames[spr->frame_idx];
    }
    else
    {
        FrameTag tag = spr->tags[spr->sel_tag];
        frame = &spr->frames[spr->frame_idx + tag.from];
    }

    SDL_FRect srcrect, dstrect;
    srcrect = frame->frame;
    dstrect.x = (float)pos.x;
    dstrect.y = (float)pos.y;
    dstrect.h = srcrect.h;
    dstrect.w = srcrect.w;

    RenderingOptions opts = create_default_rendering_options();
    opts.align = RENDER_ORIGIN_MIDDLE_CENTER;
    opts.texture = spr->texture;
    opts.srcrect = &srcrect;
    opts.dstrect = &dstrect;
    render_aligned_texture(opts);
}

void destroy_sprite(Sprite *spr)
{
    if (!spr)
        return;

    if (spr->tags)
    {
        for (Uint32 i = 0; i < spr->num_tags; i++)
        {
            SDL_free(spr->tags[i].tag);
        }
        SDL_free(spr->tags);
    }

    SDL_free(spr->frames);
    SDL_DestroyTexture(spr->texture);
    SDL_free(spr);
}
