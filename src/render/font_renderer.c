#include "render/font_renderer.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "render/renderer.h"
#include <string.h>

static const char *RAINYHEARTS_PATH = "assets/font/rainyhearts.ttf";
static const char *DAYDREAM_PATH = "assets/font/daydream.ttf";
static const char *UNIFONT_PATH = "assets/font/unifont.ttf";

static TTF_Font *rainyhearts_font = NULL;
static TTF_Font *daydream_font = NULL;
static TTF_Font *unifont_font = NULL;

bool init_font_faces(void)
{
    char *unifont_path = get_resource_path(UNIFONT_PATH);
    char *daydream_path = get_resource_path(DAYDREAM_PATH);
    char *rainyhearts_path = get_resource_path(RAINYHEARTS_PATH);

    unifont_font = TTF_OpenFont(unifont_path, 32);
    if (unifont_font == NULL)
    {
        SDL_free(unifont_path);
        SDL_free(daydream_path);
        SDL_free(rainyhearts_path);
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Couldn't load Unifont font. Are you in the correct path?");
        return false;
    }

    daydream_font = TTF_OpenFont(get_resource_path(DAYDREAM_PATH), 32);
    if (daydream_font == NULL)
    {
        SDL_free(unifont_path);
        SDL_free(daydream_path);
        SDL_free(rainyhearts_path);
        TTF_CloseFont(unifont_font);
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Couldn't load Daydream font. Are you in the correct path?");
        return false;
    }

    rainyhearts_font = TTF_OpenFont(get_resource_path(RAINYHEARTS_PATH), 32);
    if (rainyhearts_font == NULL)
    {
        SDL_free(unifont_path);
        SDL_free(daydream_path);
        SDL_free(rainyhearts_path);
        TTF_CloseFont(unifont_font);
        TTF_CloseFont(daydream_font);
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Couldn't load Rainy Hearts font. Are you in the correct path?");
        return false;
    }

    return true;
}

void draw_font(FontFace face, const char *str, double ori_x, double ori_y,
               SDL_Color color, RenderingOriginType origin)
{
    TTF_Font *font;
    switch (face)
    {
    case FONT_FACE_RAINY_HEARTS:
        font = rainyhearts_font;
        break;
    case FONT_FACE_DAYDREAM:
        font = daydream_font;
        break;
    default:
        font = unifont_font;
        break;
    }

    SDL_Renderer *renderer = get_current_renderer();
    SDL_Surface *surface =
        TTF_RenderText_Blended(font, str, strlen(str), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);

    SDL_FRect dstrect;
    dstrect.x = (float)ori_x;
    dstrect.y = (float)ori_y;
    dstrect.w = 0; // Let them calculate the width/height later.
    dstrect.h = 0;

    RenderingOptions opts = create_default_rendering_options();
    opts.align = origin;
    opts.texture = texture;
    opts.dstrect = &dstrect;

    // Ok now we can render.
    render_aligned_texture(opts);

    // Clean up after use.
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void destroy_font_faces(void)
{
    TTF_CloseFont(daydream_font);
    TTF_CloseFont(rainyhearts_font);
    TTF_CloseFont(unifont_font);
}
