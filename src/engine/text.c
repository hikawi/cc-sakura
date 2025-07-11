#include "engine/text.h"
#include "SDL3/SDL_assert.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "engine/renderer.h"
#include "misc/mathex.h"

#define MAX_FONT_NODES 50

/**
 * Represents a node present within a map to cache fonts.
 */
typedef struct FontNode
{
    Font font;
    TTF_Font *ttf_font;
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
    }
}

bool init_font_engine(AppState *app)
{
    (void)app;
    text_engine = TTF_CreateSurfaceTextEngine();
    return true;
}

bool equal_font(Font f1, Font f2)
{
    return f1.face == f2.face && feqf(f1.sp, f2.sp) && f1.style == f2.style;
}

/**
 * A simple, fast enough font hashing to index into the map.
 */
int hash_font(Font font)
{
    int hash = 17;
    hash = hash * 31 + (int)font.face;
    hash = hash * 31 + (int)font.style;
    hash = hash * 31 + (int)font.sp;
    return hash % MAX_FONT_NODES;
}

/**
 * Initializes a new font node.
 */
FontNode *init_font_node(Font font, TTF_Font *ttf)
{
    FontNode *node = SDL_malloc(sizeof(FontNode));
    node->font = font;
    node->ttf_font = ttf;
    node->next = NULL;
    return node;
}

/**
 * Retrieves a font node present within the map, if matched the provided font.
 */
FontNode *get_font_node(Font font)
{
    int idx = hash_font(font);
    FontNode *cur = font_nodes[idx];
    while (cur)
    {
        if (equal_font(cur->font, font))
            break;
        cur = cur->next;
    }

    return cur;
}

/**
 * Puts a new font with a TTF font. This replaces the existing node if already
 * there.
 */
void put_font_node(Font font, TTF_Font *ttf)
{
    int idx = hash_font(font);

    // Nothing in that bucket, just smash it in.
    if (!font_nodes[idx])
    {
        font_nodes[idx] = init_font_node(font, ttf);
        return;
    }

    FontNode *cur = font_nodes[idx];
    while (cur)
    {
        if (equal_font(cur->font, font))
        {
            TTF_CloseFont(cur->ttf_font);
            cur->ttf_font = ttf;
            break;
        }

        if (!cur->next)
        {
            cur->next = init_font_node(font, ttf);
            break;
        }

        cur = cur->next;
    }
}

FontNode *get_or_create_font_node(Font font)
{
    FontNode *node = get_font_node(font);

    // No cache font. We start to create it.
    if (!node)
    {
        TTF_Font *ttf = TTF_OpenFont(get_font_file_name(font.face), font.sp);
        TTF_SetFontHinting(ttf, TTF_HINTING_LIGHT_SUBPIXEL);
        TTF_SetFontStyle(ttf, font.style);

        if (!ttf)
        {
            SDL_LogError(SDL_LOG_CATEGORY_RENDER,
                         "Unable to open a TTF font. %s", SDL_GetError());
            return NULL;
        }

        put_font_node(font, ttf);
        node = get_font_node(font);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Cached a font style");
    }

    return node;
}

void remove_font_node(Font font)
{
    int idx = hash_font(font);

    FontNode *prev = NULL;
    FontNode *cur = font_nodes[idx];

    while (cur)
    {
        // We should remove here.
        if (equal_font(cur->font, font))
        {
            if (prev)
            {
                TTF_CloseFont(cur->ttf_font);
                SDL_free(cur);
                prev->next = cur->next;
            }
            else
            {
                // It's the head of the list.
                SDL_free(cur);
                font_nodes[idx] = cur->next;
            }
            break;
        }

        prev = cur;
        cur = cur->next;
    }
}

void destroy_font_node(FontNode *node)
{
    if (!node)
        return;

    TTF_CloseFont(node->ttf_font);
    destroy_font_node(node->next);
    SDL_free(node);
}

void render_text(FontRenderingOptions opts)
{
    AppState *state = get_app_state();

    FontNode *node = get_or_create_font_node(opts.font);
    SDL_assert(node != NULL);

    // Create the text.
    SDL_Surface *surface = TTF_RenderText_Solid(
        node->ttf_font, opts.text, SDL_strlen(opts.text), opts.color);

    // Calculate the position for the text.
    double x = opts.x, y = opts.y;
    int w = surface->w, h = surface->h;
    shift_position_to_origin(opts.origin, &x, &y, w, h);

    // Create the surface to render.
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(state->window.renderer, surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
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

void destroy_font_engine(void)
{
    for (int i = 0; i < MAX_FONT_NODES; i++)
    {
        if (font_nodes[i])
        {
            destroy_font_node(font_nodes[i]);
        }
    }

    TTF_DestroyRendererTextEngine(text_engine);
    text_engine = NULL;
}
