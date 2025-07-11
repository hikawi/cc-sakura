#include "SDL3/SDL_stdinc.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "app.h"
#include "engine/renderer.h"
#include "engine/text.h"
#include "game/game_scenes.h"

void scene_fps_ondraw(Scene *scene, SDL_Renderer *renderer)
{
    (void)renderer;

    if (scene->id != SCENE_ID_FPS)
        return;

    SDL_Log("FPS on draw");
    AppState *appstate = get_app_state();
    WindowStatus win = appstate->window;

    char buf[10];
    SDL_snprintf(buf, 10, "%d FPS", appstate->frame_data.fps);
    font_engine_render_text((FontRenderingOptions){
        .color = scene->data.empty.color,
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

Scene *scene_fps_init(SDL_Color color)
{
    Scene *scene = scene_init();
    scene->id = SCENE_ID_FPS;
    scene->data.fps.color = color;
    scene->enabled = true;

    scene->ondraw = scene_fps_ondraw;

    return scene;
}
