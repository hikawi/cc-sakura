#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "engine/scene.h"
#include "game/game_scenes.h"

typedef struct
{
    SDL_Color color;
} SceneEmptyData;

void scene_empty_ondraw(Scene *scene, SDL_Renderer *renderer)
{
    SceneEmptyData *data = (SceneEmptyData *)scene->data;
    SDL_Color color = data->color;

    // Draw the rectangle with the specified color and alpha
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void scene_empty_ondestroy(Scene *scene)
{
    SDL_free(scene->data);
}

Scene *scene_empty_init(SDL_Color color)
{
    Scene *scene = scene_init();

    scene->ondraw = scene_empty_ondraw;
    scene->ondestroy = scene_empty_ondestroy;

    SceneEmptyData *data = SDL_malloc(sizeof(SceneEmptyData));
    data->color = color;
    scene->data = data;

    return scene;
}
