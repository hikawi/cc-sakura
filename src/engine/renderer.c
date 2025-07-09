#include "engine/renderer.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "engine/text.h"

void shift_position_to_origin(RenderingOriginType type, double *x, double *y,
                              double w, double h)
{
    switch (type)
    {
    case RENDER_ORIGIN_TOP_LEFT: // Do nothing.
        break;
    case RENDER_ORIGIN_TOP_CENTER: // Move origin X to left by half.
        *x -= w / 2;
        break;
    case RENDER_ORIGIN_TOP_RIGHT: // Move origin X to left full.
        *x -= w;
        break;
    case RENDER_ORIGIN_MIDDLE_LEFT: // Move origin Y down by half.
        *y -= h / 2;
        break;
    case RENDER_ORIGIN_MIDDLE_CENTER: // Move both origins by half.
        *x -= w / 2;
        *y -= h / 2;
        break;
    case RENDER_ORIGIN_MIDDLE_RIGHT: // Move X by full and Y by half.
        *x -= w;
        *y -= h / 2;
        break;
    case RENDER_ORIGIN_BOTTOM_LEFT: // Move Y by full.
        *y -= h;
        break;
    case RENDER_ORIGIN_BOTTOM_CENTER: // Move Y full and X half.
        *x -= w / 2;
        *y -= h;
        break;
    case RENDER_ORIGIN_BOTTOM_RIGHT: // Move Y full and X full.
        *x -= w;
        *y -= h;
        break;
    }
}

void render_aligned_texture(RenderingOptions options)
{
    // This function does not check whether it was called correctly. That is the
    // job of the caller.
    double x, y, w, h;
    x = (double)options.dstrect->x;
    y = (double)options.dstrect->y;
    w = (double)options.dstrect->w;
    h = (double)options.dstrect->h;
    shift_position_to_origin(options.origin, &x, &y, w, h);

    SDL_FRect true_dstrect = {
        .x = (float)x,
        .y = (float)y,
        .w = (float)w,
        .h = (float)h,
    };

    SDL_RenderTexture(get_app_state()->renderer, options.texture,
                      options.srcrect, &true_dstrect);
}

void render_state(AppState *app)
{
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderClear(app->renderer);

    render_text((FontRenderingOptions){
        .text = "test",
        .x = app->w / 2.0,
        .y = 10,
        .origin = RENDER_ORIGIN_TOP_CENTER,
        .font =
            (Font){
                .face = FONT_FACE_DAYDREAM,
                .sp = 32,
                .style = TTF_STYLE_NORMAL,
                .hint = TTF_HINTING_LIGHT_SUBPIXEL,
            },
        .color = {.r = 22, .g = 22, .b = 255, .a = 255},
    });

    // Final present from the renderer.
    SDL_RenderPresent(app->renderer);
}
