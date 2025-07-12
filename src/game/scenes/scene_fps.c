#include "SDL3/SDL_stdinc.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "engine/renderer.h"
#include "engine/text.h"
#include "game/game_scenes.h"

typedef struct
{
    SDL_Color color;
} SceneFPSData;

void scene_fps_ondraw(Scene *scene, SDL_Renderer *renderer)
{
    (void)renderer;

    AppState *appstate = app_get();
    WindowStatus win = appstate->window;

    SceneFPSData *data = (SceneFPSData *)scene->data;

    char buf[10];
    SDL_snprintf(buf, 10, "%d FPS", appstate->frame_data.fps);
    font_engine_render_text((FontRenderingOptions){
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

void scene_fps_ondestroy(Scene *scene)
{
    SDL_free(scene->data);
}

Scene *scene_fps_init(SDL_Color color)
{
    Scene *scene = scene_init();

    scene->zindex = 999;
    scene->ondraw = scene_fps_ondraw;
    scene->ondestroy = scene_fps_ondestroy;

    SceneFPSData *data = SDL_malloc(sizeof(SceneFPSData));
    data->color = color;
    scene->data = data;

    return scene;
}
