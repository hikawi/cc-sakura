// game/scenes/scene_000_main.c
//
// This is to hold data for the main scenes of the main menu.

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "app.h"
#include "engine/collision.h"
#include "engine/renderer.h"
#include "engine/scene.h"
#include "engine/signal.h"
#include "engine/text.h"
#include "game/game_scenes.h"
#include "misc/hashmap.h"

#define SCENE_000_BTN_LEFT 0
#define SCENE_000_BTN_MID 1
#define SCENE_000_BTN_RIGHT 2

typedef struct
{
    bool left_selected;
    bool mid_selected;
    bool right_selected;
} Scene000Data;

void scene_000_oninit(Scene *s)
{
    WindowStatus winst = app_get()->window;

    // Setup colliders.
    Collider *btn_left = SDL_malloc(sizeof(Collider));
    btn_left->collider_type = COLLIDER_TYPE_AABB;
    btn_left->collision_type = COLLISION_SENSOR;
    btn_left->name = "btn_left";
    btn_left->aabb = (AABBCollider){
        .x = winst.w / 2.0 - winst.w / 4.0 + 100,
        .y = winst.h / 2.0,
        .h = 160,
        .w = winst.w / 4.0,
    };

    Collider *btn_right = SDL_malloc(sizeof(Collider));
    btn_right->collider_type = COLLIDER_TYPE_AABB;
    btn_right->collision_type = COLLISION_SENSOR;
    btn_right->name = "btn_right";
    btn_right->aabb = (AABBCollider){
        .x = winst.w / 2.0 + winst.w / 4.0 - 100,
        .y = winst.h / 2.0,
        .h = 160,
        .w = winst.w / 4.0,
    };

    Collider *btn_mid = SDL_malloc(sizeof(Collider));
    btn_mid->collider_type = COLLIDER_TYPE_AABB;
    btn_mid->collision_type = COLLISION_SENSOR;
    btn_mid->name = "btn_mid";
    btn_mid->aabb = (AABBCollider){
        .x = winst.w / 2.0,
        .y = winst.h / 2.0,
        .h = 160,
        .w = winst.w / 4.0,
    };

    hash_map_put(s->colliders, SCENE_000_BTN_LEFT, btn_left);
    hash_map_put(s->colliders, SCENE_000_BTN_MID, btn_mid);
    hash_map_put(s->colliders, SCENE_000_BTN_RIGHT, btn_right);
}

void scene_000_ondraw(Scene *s, SDL_Renderer *r)
{
    WindowStatus winst = app_get()->window;

    Collider *btn_left =
        (Collider *)hash_map_get(s->colliders, SCENE_000_BTN_LEFT);
    Collider *btn_mid =
        (Collider *)hash_map_get(s->colliders, SCENE_000_BTN_MID);
    Collider *btn_right =
        (Collider *)hash_map_get(s->colliders, SCENE_000_BTN_RIGHT);
    Scene000Data *data = (Scene000Data *)s->data;

    // Draw each button rectangles.
    SDL_FRect dstrect = {
        .w = (float)btn_left->aabb.w,
        .h = (float)btn_left->aabb.h,
        .x = (float)(btn_left->aabb.x - btn_left->aabb.w / 2),
        .y = (float)(btn_left->aabb.y - btn_left->aabb.h / 2),
    };
    SDL_SetRenderDrawColor(r, 0, 255, 0, data->left_selected ? 128 : 255);
    SDL_RenderFillRect(r, &dstrect);

    dstrect = (SDL_FRect){
        .w = (float)btn_right->aabb.w,
        .h = (float)btn_right->aabb.h,
        .x = (float)(btn_right->aabb.x - btn_right->aabb.w / 2),
        .y = (float)(btn_right->aabb.y - btn_right->aabb.h / 2),
    };
    SDL_SetRenderDrawColor(r, 0, 0, 255, data->right_selected ? 128 : 255);
    SDL_RenderFillRect(r, &dstrect);

    dstrect = (SDL_FRect){
        .w = (float)btn_mid->aabb.w,
        .h = (float)btn_mid->aabb.h,
        .x = (float)(btn_mid->aabb.x - btn_mid->aabb.w / 2),
        .y = (float)(btn_mid->aabb.y - btn_mid->aabb.h / 2),
    };
    SDL_SetRenderDrawColor(r, 255, 0, 0, data->mid_selected ? 128 : 255);
    SDL_RenderFillRect(r, &dstrect);

    // Draw the text that says which button is being pressed
    char buf[24] = {0};
    SDL_strlcat(buf, data->left_selected ? "on, " : "off, ", 256);
    SDL_strlcat(buf, data->mid_selected ? "on, " : "off, ", 256);
    SDL_strlcat(buf, data->right_selected ? "on" : "off", 256);

    font_engine_render_text((FontRenderingOptions){
        .x = winst.w / 2.0,
        .y = 20,
        .origin = RENDER_ORIGIN_TOP_CENTER,
        .font =
            {
                .face = FONT_FACE_RAINY_HEARTS,
                .sp = 24,
                .style = TTF_STYLE_NORMAL,
            },
        .text = buf,
    });
}

void scene_000_onphystick(Scene *s)
{
    Scene000Data *data = (Scene000Data *)s->data;

    // Reset data for next signal pump.
    data->left_selected = false;
    data->mid_selected = false;
    data->right_selected = false;
}

void scene_000_onsignal(Scene *s, Signal *signal)
{
    (void)s;
    AppState *appstate = app_get();
    Scene000Data *data = (Scene000Data *)s->data;

    CollisionPair btn_left_p = {
        .a = &appstate->input.mouse,
        .b = hash_map_get(s->colliders, SCENE_000_BTN_LEFT),
    };
    CollisionPair btn_mid_p = {
        .a = &appstate->input.mouse,
        .b = hash_map_get(s->colliders, SCENE_000_BTN_MID),
    };
    CollisionPair btn_right_p = {
        .a = &appstate->input.mouse,
        .b = hash_map_get(s->colliders, SCENE_000_BTN_RIGHT),
    };

    switch (signal->type)
    {
    case SIGNAL_COLLISION:
        if (collision_pair_eq(&btn_left_p, &signal->collision.pair))
        {
            data->left_selected = true;
        }
        if (collision_pair_eq(&btn_mid_p, &signal->collision.pair))
        {
            data->mid_selected = true;
        }
        if (collision_pair_eq(&btn_right_p, &signal->collision.pair))
        {
            data->right_selected = true;
        }
        break;
    default:
        break;
    }
}

void scene_000_ondestroy(Scene *s)
{
    SDL_free(hash_map_get(s->colliders, SCENE_000_BTN_LEFT));
    SDL_free(hash_map_get(s->colliders, SCENE_000_BTN_MID));
    SDL_free(hash_map_get(s->colliders, SCENE_000_BTN_RIGHT));
    SDL_free(s->data);
}

Scene *scene_000_main(void)
{
    Scene *sc = scene_init();

    sc->oninit = scene_000_oninit;
    sc->onphystick = scene_000_onphystick;
    sc->ondraw = scene_000_ondraw;
    sc->ondestroy = scene_000_ondestroy;
    sc->onsignal = scene_000_onsignal;

    Scene000Data *data = SDL_malloc(sizeof(Scene000Data));
    sc->data = data;

    return sc;
}
