#include "engine/sprite.h"

#include "app.h"
#include "common.h"
#include "engine/renderer.h"
#include "misc/genhashmap.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_storage.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

/**
 * Version 1 of the Sprite File Format loader.
 */
bool sprite_load_v1(Sprite *spr, SDL_IOStream *fp, SDL_Renderer *renderer)
{
    uint64_t img_size;
    SDL_ReadU64LE(fp, &img_size);

    uint8_t *img_buf = SDL_malloc(sizeof(uint8_t) * img_size);
    if (!img_buf)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Can't allocate enough memory for image buffer");
        return false;
    }

    // Read the image by chunks until it is finished.
    uint64_t img_read = 0;
    while (img_read < img_size)
    {
        uint64_t to_read = SDL_min(4096, img_size - img_read);
        uint64_t buf_read = SDL_ReadIO(fp, img_buf + img_read, to_read);

        if (buf_read == 0)
        {
            // Unexpected EOF!
            if (SDL_GetIOStatus(fp) == SDL_IO_STATUS_EOF)
            {
                SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unexpected EOF from reading sprite. Sprite file is corrupted?");
                SDL_free(img_buf);
                return false;
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Read data sprite error: %s", SDL_GetError());
                SDL_free(img_buf);
                return false;
            }
        }

        img_read += buf_read;
    }

    // Convert from byte array to a workable Surface by SDL_image.
    SDL_IOStream *img_io = SDL_IOFromMem(img_buf, img_size);
    if (!img_io)
    {
        SDL_free(img_buf);
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open IO stream for sprite file: %s", SDL_GetError());
        return false;
    }

    SDL_Surface *img_surface = IMG_Load_IO(img_io, true);
    if (!img_surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to create surface from IO: %s", SDL_GetError());
        return false;
    }

    // GPU-accelerated texture, we don't need the surface anymore!
    SDL_free(img_buf); // We can free the image buffer now.
    SDL_Texture *img_texture = SDL_CreateTextureFromSurface(renderer, img_surface);
    SDL_DestroySurface(img_surface); // We can destroy the surface now.
    if (!img_texture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to create texture from surface: %s", SDL_GetError());
        return false;
    }

    spr->texture = img_texture;
    SDL_SetTextureScaleMode(img_texture, SDL_SCALEMODE_NEAREST);
    SDL_ReadU32LE(fp, &spr->w);
    SDL_ReadU32LE(fp, &spr->h);

    // Read frame tags.
    SDL_ReadU32LE(fp, &spr->tags_length);
    spr->tags = SDL_malloc(sizeof(SpriteFrameTag) * spr->tags_length);
    if (!spr->tags)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory for frame tags.");
        return false;
    }

    // During iterations, we would also create the appropriate generic hash map's nodes for that.
    // This would look very ugly though.
    spr->tags_map = gen_hash_map_init();
    if (!spr->tags_map)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory for a frame tag lookup table.");
        return false;
    }
    spr->tags_map->hash = (HashFunction)strhash;
    spr->tags_map->comparator = (CompareFunction)SDL_strcmp;
    spr->tags_map->destroys_value = true;

    // Loop through each tag.
    for (uint32_t i = 0; i < spr->tags_length; i++)
    {
        uint32_t name_length;
        SDL_ReadU32LE(fp, &name_length);

        spr->tags[i].name = SDL_calloc(name_length + 1, sizeof(char));
        if (!spr->tags[i].name)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory for a frame tag's name %d", i);
            return false;
        }

        // Let's hope this doesn't fail now, cause fuck me sideways.
        SDL_ReadIO(fp, spr->tags[i].name, name_length);
        SDL_ReadU32LE(fp, &spr->tags[i].from);
        SDL_ReadU32LE(fp, &spr->tags[i].to);

        // Hopefully that didn't fail because the stuff starts here.
        // We reuse the name because we already allocated for it.
        uint32_t *index = SDL_malloc(sizeof(uint32_t));
        if (!index)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory for a number. HOW LOW ON MEMORY ARE YOU?");
            return false;
        }
        *index = i;

        gen_hash_map_put(spr->tags_map, spr->tags[i].name, index);
    }

    // Reading data for each frames
    SDL_ReadU32LE(fp, &spr->frames_length);
    spr->frames = SDL_malloc(sizeof(SpriteFrame) * spr->frames_length);
    if (!spr->frames)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate memory for sprite frames.");
        return false;
    }
    for (uint32_t i = 0; i < spr->frames_length; i++)
    {
        SDL_ReadU32LE(fp, &spr->frames[i].source_size.w);
        SDL_ReadU32LE(fp, &spr->frames[i].source_size.h);

        uint32_t x, y, w, h;
        SDL_ReadU32LE(fp, &x);
        SDL_ReadU32LE(fp, &y);
        SDL_ReadU32LE(fp, &w);
        SDL_ReadU32LE(fp, &h);
        spr->frames[i].spr_source_size.x = (int)x;
        spr->frames[i].spr_source_size.y = (int)y;
        spr->frames[i].spr_source_size.w = (int)w;
        spr->frames[i].spr_source_size.h = (int)h;

        SDL_ReadU32LE(fp, &x);
        SDL_ReadU32LE(fp, &y);
        SDL_ReadU32LE(fp, &w);
        SDL_ReadU32LE(fp, &h);
        spr->frames[i].frame.x = (int)x;
        spr->frames[i].frame.y = (int)y;
        spr->frames[i].frame.w = (int)w;
        spr->frames[i].frame.h = (int)h;

        SDL_ReadU32LE(fp, &spr->frames[i].duration);
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Successfully loaded sprite %s from v1 loader.", spr->name);
    return true;
}

Sprite *sprite_load(const char *name)
{
    SDL_Renderer *renderer = app_get()->window.renderer;
    char buf[1024] = {0};
    SDL_snprintf(buf, sizeof(buf), "assets/spr/%s.sprite", name);

    // Try to open a file in SDL's storage.
    // Why is this so complicated?
    SDL_Storage *storage = SDL_OpenTitleStorage(NULL, 0);
    while (!SDL_StorageReady(storage))
    {
        SDL_Delay(1);
    }

    uint64_t spr_len;
    if (!SDL_GetStorageFileSize(storage, buf, &spr_len))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to query file size for %s", buf);
        SDL_CloseStorage(storage);
        return NULL;
    }

    char *spr_file = SDL_malloc(sizeof(char) * spr_len);
    if (!SDL_ReadStorageFile(storage, buf, spr_file, spr_len))
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to read the sprite %s", buf);
        SDL_CloseStorage(storage);
        return NULL;
    }

    // We're done with the storage
    SDL_CloseStorage(storage);

    SDL_IOStream *fp = SDL_IOFromConstMem(spr_file, sizeof(char) * spr_len);
    if (!fp)
    {
        SDL_free(spr_file);
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Couldn't open sprite asset with name %s", buf);
        return NULL;
    }

    Sprite *spr = SDL_calloc(1, sizeof(Sprite));
    if (!spr)
    {
        SDL_free(spr_file);
        SDL_CloseIO(fp);
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to allocate enough memory for Sprite %s.", name);
        return NULL;
    }

    spr->name = SDL_strdup(name);
    spr->scale = 1;

    // Read the version and delegate.
    uint32_t spr_version;
    SDL_ReadU32LE(fp, &spr_version);
    bool status = true;

    switch (spr_version)
    {
    case 1:
        status = sprite_load_v1(spr, fp, renderer);
        break;
    }

    SDL_free(spr_file);
    SDL_CloseIO(fp);
    if (!status)
    {
        sprite_destroy(spr);
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to load sprite at path %s.", buf);
        return NULL;
    }

    return spr;
}

bool sprite_select_tag(Sprite *spr, const char *tag)
{
    SDL_assert(spr != NULL && tag != NULL);

    uint32_t *tag_idx = (uint32_t *)gen_hash_map_get(spr->tags_map, tag);
    if (!tag_idx)
    {
        spr->sel_tag = 0;
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Trying to set a sprite animation to non-existent %s.", tag);
        return false;
    }

    spr->sel_tag = *tag_idx;
    spr->frame_accum = 0;
    spr->frame_idx = spr->tags[spr->sel_tag].from;
    return true;
}

void sprite_advance_animation(Sprite *spr, double dt)
{
    SDL_assert(spr != NULL && dt >= 0);
    if (!spr->playing)
    {
        return;
    }
    if (spr->frames_length == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Can't advance animation on a sprite with no frames.");
        return;
    }

    double duration = spr->frames[spr->frame_idx].duration / 1000.0;
    if (spr->frame_accum > duration)
    {
        spr->frame_accum -= duration;
        if (spr->tags_length > 0)
        {
            SpriteFrameTag tag = spr->tags[spr->sel_tag];
            uint32_t frame_window = (tag.to - tag.from) + 1;
            spr->frame_idx = ((spr->frame_idx + 1 - tag.from) % frame_window) + tag.from;
        }
        else
        {
            spr->frame_idx = (spr->frame_idx + 1) % spr->frames_length;
        }
    }

    spr->frame_accum += dt;
}

void sprite_render(SpriteRenderProperties props)
{
    SDL_assert(props.renderer != NULL);
    SDL_assert(props.spr != NULL && props.spr->frames_length > 0);

    AppState *app = app_get();
    SpriteFrame frame = props.spr->frames[props.spr->frame_idx];

    SDL_FRect frect;
    frect.x = (float)frame.frame.x;
    frect.y = (float)frame.frame.y;
    frect.w = (float)frame.frame.w;
    frect.h = (float)frame.frame.h;

    SDL_FRect dstrect;
    dstrect.x = (float)props.pos.x;
    dstrect.y = (float)props.pos.y;
    dstrect.w = frect.w * (float)(props.spr->scale * app->settings.scale);
    dstrect.h = frect.h * (float)(props.spr->scale * app->settings.scale);

    RenderingOptions opts;
    opts.texture = props.spr->texture;
    opts.srcrect = &frect;
    opts.dstrect = &dstrect;
    opts.origin = props.origin;
    opts.rotation = props.spr->rotation;
    opts.flip_hori = props.spr->flip_hori;
    opts.flip_vert = props.spr->flip_vert;
    opts.renderer = props.renderer;
    render_aligned_texture(opts);
}

void sprite_destroy(Sprite *spr)
{
    if (!spr)
    {
        return;
    }

    if (spr->frames)
    {
        SDL_free(spr->frames);
        spr->frames = NULL;
    }
    if (spr->texture)
    {
        SDL_DestroyTexture(spr->texture);
        spr->texture = NULL;
    }
    if (spr->tags)
    {
        for (uint32_t i = 0; i < spr->tags_length; i++)
        {
            if (spr->tags[i].name)
            {
                SDL_free(spr->tags[i].name);
                spr->tags[i].name = NULL;
            }
        }
        SDL_free(spr->tags);
        spr->tags = NULL;
    }
    if (spr->name)
    {
        SDL_free(spr->name);
        spr->name = NULL;
    }
    if (spr->tags_map)
    {
        gen_hash_map_destroy(spr->tags_map);
        spr->tags_map = NULL;
    }
    SDL_free(spr);
}
