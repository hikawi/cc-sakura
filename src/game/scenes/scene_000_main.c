// game/scenes/scene_000_main.c
//
// This is to hold data for the main scenes of the main menu.

#include "app.h"
#include "engine/collision.h"
#include "engine/renderer.h"
#include "engine/scene.h"
#include "engine/signal.h"
#include "engine/text.h"
#include "game/game_scenes.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3_ttf/SDL_ttf.h"

typedef enum
{
    COLLIDER_START_BUTTON,
} Scene000Collider;

typedef struct
{
    bool started;
} Scene000Data;

void scene_000_oninit(Scene *s)
{
    Scene000Data *data = SDL_calloc(1, sizeof(Scene000Data));
    data->started      = false;

    s->data = data;

    // Setup colliders
    Collider *start_btn = SDL_calloc(1, sizeof(Collider));
}

void scene_000_onstart(Scene *s)
{
    Scene000Data *data = (Scene000Data *)s->data;
    data->started      = true;
}

void scene_000_ondraw(Scene *s, SDL_Renderer *r)
{
    (void)s;

    WindowStatus winst = app_get()->window;

    // Clear screen white
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderClear(r);

    // Render the version string on the bottom left.
    FontRenderingOptions opts;
    opts.text   = APPLICATION_IDENTIFIER " " APPLICATION_VERSION;
    opts.x      = 24;
    opts.y      = winst.h - 24;
    opts.origin = RENDER_ORIGIN_BOTTOM_LEFT;
    opts.color  = rgba(0, 0, 0, 0);
    opts.font   = (Font){
          .face  = FONT_FACE_RAINY_HEARTS,
          .sp    = 24,
          .style = TTF_STYLE_ITALIC,
    };
    text_render(opts);
}

void scene_000_onphystick(Scene *s)
{
    // Scene000Data *data = (Scene000Data *)s->data;
    (void)s;
}

void scene_000_onsignal(Scene *s, Signal *signal)
{
    // AppState *appstate = app_get();
    // Scene000Data *data = (Scene000Data *)s->data;
    (void)s;

    switch (signal->type)
    {
    case SIGNAL_COLLISION:
        break;
    default:
        break;
    }
}

Scene *scene_000_main(void)
{
    Scene *sc = scene_init();

    sc->oninit     = scene_000_oninit;
    sc->onphystick = scene_000_onphystick;
    sc->ondraw     = scene_000_ondraw;
    sc->onsignal   = scene_000_onsignal;

    return sc;
}
