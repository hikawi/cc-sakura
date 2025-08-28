#include "engine/scene.h"
#include "game/game_scenes.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"

typedef struct
{
    SDL_Color color;
} SceneEmpty;

void scene_empty_ondraw(Scene *scene, SDL_Renderer *renderer)
{
    SceneEmpty *data = (SceneEmpty *)scene->data;
    const SDL_Color color = data->color;

    // Draw the rectangle with the specified color and alpha
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

Scene *scene_empty(const SDL_Color color)
{
    Scene *scene = scene_init();

    scene->ondraw = scene_empty_ondraw;

    SceneEmpty *data = SDL_malloc(sizeof(SceneEmpty));
    data->color = color;
    scene->data = data;

    return scene;
}
