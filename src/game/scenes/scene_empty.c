#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "engine/scene.h"
#include "game/game_scenes.h"

void scene_empty_ondraw(Scene *scene, SDL_Renderer *renderer)
{
    if (scene->id != SCENE_ID_EMPTY)
        return;

    SDL_Color color = scene->data.empty.color;

    // Draw the rectangle with the specified color and alpha
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

Scene *scene_empty_init(SDL_Color color, SDL_FRect frect)
{
    Scene *scene = scene_init();
    scene->id = SCENE_ID_EMPTY;
    scene->data.empty.color = color;
    scene->data.empty.frect = frect;
    scene->enabled = true;

    scene->ondraw = scene_empty_ondraw;

    return scene;
}
