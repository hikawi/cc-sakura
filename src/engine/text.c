#include "engine/text.h"

#include "app.h"
#include "engine/renderer.h"
#include "misc/mathex.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_storage.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <string.h>

#define MAX_FONT_NODES 50

/**
 * Represents a node present within a map to cache fonts.
 */
typedef struct FontNode
{
    Font font;
    TTF_Font *ttf_font;
    char *ttf_buf;

    struct FontNode *next;
} FontNode;

// The map we're using as a bucket map for font nodes.
static FontNode *font_nodes[MAX_FONT_NODES] = {0};

// Other stuff from TTF to handle.
static TTF_TextEngine *text_engine = NULL;

const char *get_font_file_name(FontFace face)
{
    switch (face)
    {
    case FONT_FACE_DAYDREAM:
        return "assets/font/daydream.ttf";
    case FONT_FACE_RAINY_HEARTS:
        return "assets/font/rainyhearts.ttf";
    case FONT_FACE_UNIFONT:
        return "assets/font/unifont.ttf";
    default:
        return "";
    }
}

bool text_init(AppState *app)
{
    (void)app;
    text_engine = TTF_CreateSurfaceTextEngine();
    return true;
}

bool font_eq(Font f1, Font f2)
{
    return f1.face == f2.face && feqf(f1.sp, f2.sp) && f1.style == f2.style;
}

/**
 * A simple, fast enough font hashing to index into the map.
 */
int font_hash(Font font)
{
    int hash = 17;
    hash     = hash * 31 + (int)font.face;
    hash     = hash * 31 + (int)font.style;
    hash     = hash * 31 + (int)font.sp;
    return hash % MAX_FONT_NODES;
}

/**
 * Initializes a new font node.
 */
FontNode *font_node_init(Font font, TTF_Font *ttf, char *ttf_buf)
{
    FontNode *node = SDL_malloc(sizeof(FontNode));
    node->font     = font;
    node->ttf_font = ttf;
    node->ttf_buf  = ttf_buf;
    node->next     = NULL;
    return node;
}

/**
 * Retrieves a font node present within the map, if matched the provided font.
 */
FontNode *font_node_get(Font font)
{
    int idx       = font_hash(font);
    FontNode *cur = font_nodes[idx];
    while (cur)
    {
        if (font_eq(cur->font, font))
            break;
        cur = cur->next;
    }

    return cur;
}

/**
 * Puts a new font with a TTF font. This replaces the existing node if already
 * there.
 */
void font_node_put(Font font, TTF_Font *ttf, char *ttf_buf)
{
    int idx = font_hash(font);

    // Nothing in that bucket, just smash it in.
    if (!font_nodes[idx])
    {
        font_nodes[idx] = font_node_init(font, ttf, ttf_buf);
        return;
    }

    FontNode *cur = font_nodes[idx];
    while (cur)
    {
        if (font_eq(cur->font, font))
        {
            TTF_CloseFont(cur->ttf_font);
            SDL_free(cur->ttf_buf);

            cur->ttf_font = ttf;
            cur->ttf_buf  = ttf_buf;
            break;
        }

        if (!cur->next)
        {
            cur->next = font_node_init(font, ttf, ttf_buf);
            break;
        }

        cur = cur->next;
    }
}

FontNode *font_node_get_or_create(Font font)
{
    FontNode *node = font_node_get(font);

    // No cache font. We start to create it.
    if (!node)
    {
        const char *filepath = get_font_file_name(font.face);

        // Read the font file up using SDL_Storage
        SDL_Storage *storage = SDL_OpenTitleStorage(NULL, 0);
        while (!SDL_StorageReady(storage))
        {
            SDL_Delay(1);
        }

        uint64_t filelen;
        if (!SDL_GetStorageFileSize(storage, filepath, &filelen))
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to query for font file size %s.", filepath);
            SDL_CloseStorage(storage);
            return NULL;
        }

        // M-alloc an array for the font file.
        char *filebuf = SDL_malloc(sizeof(char) * filelen);
        if (!SDL_ReadStorageFile(storage, filepath, filebuf, filelen))
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to read storage file %s.", filepath);
            SDL_CloseStorage(storage);
            return NULL;
        }

        // We're done with the storage
        SDL_CloseStorage(storage);

        TTF_Font *ttf = TTF_OpenFontIO(SDL_IOFromConstMem(filebuf, sizeof(char) * filelen), true, font.sp);
        TTF_SetFontHinting(ttf, TTF_HINTING_LIGHT_SUBPIXEL);
        TTF_SetFontStyle(ttf, font.style);

        if (!ttf)
        {
            SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Unable to open a TTF font. %s", SDL_GetError());
            return NULL;
        }

        font_node_put(font, ttf, filebuf);
        node = font_node_get(font);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Cached a font style");
    }

    return node;
}

void font_node_remove(Font font)
{
    int idx = font_hash(font);

    FontNode *prev = NULL;
    FontNode *cur  = font_nodes[idx];

    while (cur)
    {
        // We should remove here.
        if (font_eq(cur->font, font))
        {
            TTF_CloseFont(cur->ttf_font);
            SDL_free(cur->ttf_buf);
            if (prev)
            {
                prev->next = cur->next;
            }
            else
            {
                // It's the head of the list.
                font_nodes[idx] = cur->next;
            }
            SDL_free(cur);
            break;
        }

        prev = cur;
        cur  = cur->next;
    }
}

void font_node_destroy(FontNode *node)
{
    if (!node)
        return;

    TTF_CloseFont(node->ttf_font);
    SDL_free(node->ttf_buf);
    font_node_destroy(node->next);
    SDL_free(node);
}

void text_preload(Font font)
{
    font_node_get_or_create(font);
}

void text_render(FontRenderingOptions opts)
{
    AppState *state = app_get();

    FontNode *node = font_node_get_or_create(opts.font);
    SDL_assert(node != NULL);

    // Create the text.
    SDL_Surface *surface = TTF_RenderText_Blended(node->ttf_font, opts.text, SDL_strlen(opts.text), opts.color);

    // Calculate the position for the text.
    double x = opts.x, y = opts.y;
    int w = surface->w, h = surface->h;
    shift_position_to_origin(opts.origin, &x, &y, w, h);

    // Create the surface to render.
    SDL_Texture *texture = SDL_CreateTextureFromSurface(state->window.renderer, surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_FRect dstrect = {
        .x = (float)x,
        .y = (float)y,
        .h = (float)h,
        .w = (float)w,
    };
    SDL_RenderTexture(state->window.renderer, texture, NULL, &dstrect);

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void text_destroy(void)
{
    for (int i = 0; i < MAX_FONT_NODES; i++)
    {
        if (font_nodes[i])
        {
            font_node_destroy(font_nodes[i]);
        }
    }

    TTF_DestroySurfaceTextEngine(text_engine);
    text_engine = NULL;
}
