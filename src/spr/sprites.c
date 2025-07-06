#include "spr/sprites.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "app.h"

/**
 * This function attempts to load a texture from a file,
 * assumed to be an image to be loaded with SDL_image.
 */
SpriteTexture load_texture_from_image(SDL_Renderer *renderer, const char *file)
{
    char *res_path = get_resource_path(file);
    SDL_Surface *surface = IMG_Load(res_path);
    SpriteTexture spr_text;
    spr_text.texture = NULL;
    SDL_free(res_path);

    if (surface == NULL)
    {
        return spr_text;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    spr_text.texture = texture;
    spr_text.srcrect.x = 0;
    spr_text.srcrect.y = 0;
    spr_text.srcrect.w = surface->w;
    spr_text.srcrect.h = surface->h;
    SDL_DestroySurface(surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);

    return spr_text;
}

void advance_animation_tick(Sprite *spr, double dt)
{
    spr->dt_accumulator += dt;
    if (spr->dt_accumulator >= (1.0 / spr->fps))
    {
        spr->dt_accumulator -= (1.0 / spr->fps);
        spr->ani_idx++;
        spr->ani_idx %= spr->textures_len;
    }
}
