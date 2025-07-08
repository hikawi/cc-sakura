#include "app.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "engine/collision.h"
#include <string.h>

AppState *init_app_state(void)
{
    AppState *state = SDL_malloc(sizeof(AppState));

    // Setup frames data
    state->frame_data.frame_count = 0;
    state->frame_data.frame_accum = 0;
    state->frame_data.frame_time = 0;
    state->frame_data.last_frame_tick = SDL_GetTicks();
    state->frame_data.fps = 0;

    // Memset keyboard state to all 0, since it's only bools.
    SDL_memset(&state->keyboard, 0, sizeof(KeyboardStatus));

    // Initialize NULL pointers or Windows yells at me.
    state->quadtree = NULL;
    state->floor_colliders = NULL;
    return state;
}

void destroy_app_state(AppState *state)
{
    if (state->floor_colliders)
        destroy_collider_list(state->floor_colliders);
    if (state->quadtree)
        destroy_quadtree_node(state->quadtree);
    SDL_free(state);
}

char *get_resource_path(const char *subpath)
{
    char *path = SDL_calloc(1024, sizeof(char));

    strcat(path, SDL_GetBasePath());
    strcat(path, "/");
    strcat(path, subpath);

    return path;
}
