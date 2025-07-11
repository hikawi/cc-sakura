#include "engine/renderer.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "engine/map.h"
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

void render_map(Map *map, Sprite *spr)
{
    // We want to align the map to the bottom of the screen. But the map
    // was written with coords relative to the top of the map.
    // Each map tile is a 16x16.
    // We need to figure out a way to translate map's local coords to the screen
    // coords.
    AppState *appstate = get_app_state();

    // Let's render (0, maxY) = bottom left. That means local_x * 16 = screen_x.
    // (0, maxY-1) = 1 off bottom => screen_y = win_y - (max_y - local_y) * 16.
    for (Uint32 x = 0; x < map->w; x++)
    {
        for (Uint32 y = 0; y < map->h; y++)
        {
            // First, we calculate the srcrect.
            // A sprite is expected to have the following order in 12:
            // solo, nw, n, ne, w, c, e, sw, s, se, vert, horiz
            MapNode node = map->tiles[y * map->w + x];

            // Ignore if is air.
            if (node.tile == TILE_AIR)
            {
                continue;
            }

            set_map_tile_sprite(spr, node.tile);

            // Get the frame we're gonna draw.
            SDL_FRect srcrect, dstrect;
            SpriteFrame frame;
            if (spr->sel_tag >= 0)
            {
                FrameTag tag = spr->tags[spr->sel_tag];
                frame = spr->frames[tag.from + (Uint32)node.dir];
            }
            else
            {
                frame = spr->frames[node.dir];
            }

            // Then we compute the srcrect and dstrect to draw.
            srcrect = frame.frame;

            dstrect.x = x * APPLICATION_MAP_TILE * APPLICATION_SCALE;
            dstrect.y = (Uint32)appstate->window.h - (map->h - y - 1) *
                                                         APPLICATION_MAP_TILE *
                                                         APPLICATION_SCALE;
            dstrect.w = APPLICATION_MAP_TILE * APPLICATION_SCALE;
            dstrect.h = APPLICATION_MAP_TILE * APPLICATION_SCALE;

            render_aligned_texture((RenderingOptions){
                .texture = spr->texture,
                .origin = RENDER_ORIGIN_BOTTOM_LEFT,
                .srcrect = &srcrect,
                .dstrect = &dstrect,
            });
        }
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

    SDL_RenderTexture(get_app_state()->window.renderer, options.texture,
                      options.srcrect, &true_dstrect);
}
