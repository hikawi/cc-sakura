#include "app.h"
#include "engine/renderer.h"
#include "engine/text.h"
#include "game/game_scenes.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3_ttf/SDL_ttf.h"

typedef struct
{
    SDL_Color color;
} SceneFPS;

void scene_fps_ondraw(Scene *scene, SDL_Renderer *renderer)
{
    (void)renderer;

    AppState *appstate = app_get();
    WindowStatus win = appstate->window;

    SceneFPS *data = (SceneFPS *)scene->data;

    char buf[10];
    SDL_snprintf(buf, 10, "%d FPS", appstate->frame_data.fps);
    text_render((FontRenderingOptions){
        .color = data->color,
        .origin = RENDER_ORIGIN_TOP_RIGHT,
        .text = buf,
        .x = win.w - 10,
        .y = 10,
        .font =
            {
                .face = FONT_FACE_DAYDREAM,
                .sp = 24,
                .style = TTF_STYLE_NORMAL,
            },
    });
}

Scene *scene_fps(const SDL_Color color)
{
    Scene *scene = scene_init();

    scene->zindex = 999;
    scene->ondraw = scene_fps_ondraw;

    SceneFPS *data = SDL_malloc(sizeof(SceneFPS));
    data->color = color;
    scene->data = data;

    return scene;
}
